///////////////////////////////////////////////////////////////////////////////////////////////////////////////////////
//	Illum Shader
///////////////////////////////////////////////////////////////////////////////////////////////////////////////////////

cbuffer SceneCB : register(b0)
{
    float4x4 mtxView;
	float4x4 mtxViewInv;
    float4x4 mtxProj;
    float4x4 mtxProjInv;
    float4 sunPos;
	float4x4 mtxSunViewProj;
	uint2 shadowMapRes;
	uint2 screenRes;
    float4 eyePos;
	float fogStart;
	float fogEnd;
}

SamplerState PointSampler : register(s0);
SamplerState LinearSampler : register(s1);
SamplerComparisonState ShadowMapSampler : register(s2);

static float PI = 3.14159265358f;
static float ONE_OVER_PI = 0.31830988618f;


// --------------------------------------------------------------------------------------------------------------------
// Helper methods




// --------------------------------------------------------------------------------------------------------------------
// Physically Based Rendering

float RoughnessToAlpha(float roughness)
{
	return 2.0f / (roughness * roughness) - 2.0f;
}

// Normalized phong for rendering equation
void BRDF_Phong(float3 L, float3 V, float3 N, float alpha, out float d, out float s)
{
	float3 R = normalize(2.0f * dot(L, N) * N - L);
	d = ONE_OVER_PI;
	s = ((alpha + 2.0f) / (2.0f * PI)) * pow(saturate(dot(R, V)), alpha);
}

// Normalized blinn phong (approximation)
void BRDF_Blinn(float3 L, float3 V, float3 N, float3 H, float alpha, out float d, out float s)
{
	d = ONE_OVER_PI;
	s = (alpha + 8.0f) / (8.0f * PI) * pow(saturate(dot(N, H)), alpha);
}


// --------------------------------------------------------------------------------------------------------------------
// Shadowmapping

float CalculateShadowMapFactor(float3 WorldPos);




////////////////////////////////////////////////////////////////////////////////
////////////////////////////////////////////////////////////////////////////////
//
//  FORWARD SHADER
//
////////////////////////////////////////////////////////////////////////////////
////////////////////////////////////////////////////////////////////////////////

#ifdef ENTRY_forward
#define HAS_SHADOWMAP

cbuffer ObjectCB : register(b1)
{
	float4x4 mtxWorld;
	float matRoughness;
};

Texture2D textureMap : register(t0);
Texture2D normalMap : register(t1);
Texture2D shadowMap : register(t2);

struct VS_INPUT
{
	float3 Position : POSITION;
	float3 Normal : NORMAL;
	float3 Tangent : TANGENT;
	float3 Color : COLOR0;
	float2 TexCoord : TEXCOORD0;
};

struct VS_OUTPUT_FORWARD
{
    float4 Position : SV_Position;
    float3 WorldPos : TEXCOORD0;
    float3 Color : COLOR0;
    float3 Normal : TEXCOORD1;
    float3 Tangent : TEXCOORD2;
    float2 TexCoord : TEXCOORD3;
};

VS_OUTPUT_FORWARD VS_forward(VS_INPUT IN)
{
	VS_OUTPUT_FORWARD OUT;

    float4x4 mtxWorldInv = transpose(mtxWorld);

    float4 wPos = mul(mtxWorld, float4(IN.Position,1.0f));
    OUT.WorldPos = wPos.xyz;
    OUT.Position = mul(mtxProj, mul(mtxView, wPos));

    OUT.Normal = normalize(mul(mtxWorld, normalize(float4(IN.Normal,0.0f))).xyz);
    OUT.Tangent = normalize(mul(mtxWorldInv, normalize(float4(IN.Tangent,0.0f))).xyz);
    OUT.TexCoord = IN.TexCoord;
    OUT.Color = IN.Color;

    return OUT;
}

// ---------------------------------------------------------

struct PS_INPUT_FORWARD
{
    float4 Position : SV_Position;
    float3 WorldPos : TEXCOORD0;
    float3 Color : COLOR0;
    float3 Normal : TEXCOORD1;
    float3 Tangent : TEXCOORD2;
    float2 TexCoord : TEXCOORD3;
};

struct PS_OUTPUT_FORWARD
{
    float4 Color : SV_Target0;
};

float quadr_fade(float val)
{
    float a = (val - 1.0f);
    return -(a * a) + 1.0f;
}

// N should be normalized already
float calc_lambert(float3 N, float3 lightDirOut)
{
    return saturate(dot(N, lightDirOut));
}

float saturate_negpos(float val)
{
    return val * 0.5f + 0.5f;
}

float3 calc_phong(float3 N, float3 lightDirOut, float3 dirToEye, float roughness, float intensityIn)
{
    float diffuseFactor = roughness;
    float specularFactor = clamp(1.0f - roughness, 0, 0.6f);

    // Diffuse:
    float lambert = calc_lambert(N, lightDirOut);

    // Specular:
    float alpha = 2.0f;
    if (roughness > 0.0f)
        alpha = 1.0f / saturate(roughness - 0.5f); // the rougher the surface, the wider the specular highlight

    float3 R = -lightDirOut + 2.0f * saturate(dot(lightDirOut, N)) * N;
    float specular = pow(saturate(dot(R, dirToEye)), alpha);

    // compose:
    float intensityOut = (intensityIn * (diffuseFactor * lambert + specularFactor * specular));
    return float3(intensityOut, intensityOut, intensityOut);
}

PS_OUTPUT_FORWARD PS_forward(PS_INPUT_FORWARD IN)
{
	PS_OUTPUT_FORWARD OUT;

    // calc binormal. Everything in World space!
    float3 normal = IN.Normal;
    float3 tangent = IN.Tangent;
    float3 binormal = cross(normal, tangent);

    // Calculate Tangent to World Space Rotation Matrix
    float3x3 matWT = float3x3(tangent, binormal, normal);
    float3x3 matTW = transpose(matWT);

    // Sample normal change in tangent space from NormalMap
    float3 sampledNormal = normalMap.Sample(LinearSampler, IN.TexCoord).rgb;
    sampledNormal.rg = 2.0f * sampledNormal.rg - 1.0f;
    float3 bumpNormal = mul(matTW, normalize(sampledNormal));

    //normal = bumpNormal;



    // Get texture map color
    float3 albedo = textureMap.Sample(LinearSampler, IN.TexCoord).rgb;

    // Calculate lighting factor. Using a fixed light dir and eye pos for now
    float3 L = normalize(sunPos.xyz);
    float3 V = normalize(eyePos.xyz - IN.WorldPos);

    float lambertBRDF = 1 / PI;

	float3 irradiance = float3(1.0f, 1.0f, 1.0f) * 4.0f;
	irradiance *= CalculateShadowMapFactor(IN.WorldPos);
	float3 ambient = float3(0.8f, 0.8f, 0.8f) * 0.2f;

    float3 LOut = IN.Color * saturate(albedo * (lambertBRDF * saturate(dot(normal, L)) * irradiance + ambient));


	// Fog
	float viewZ = -dot(mtxView[2], float4(IN.WorldPos, 1.0f));
	float fogAmount = saturate((viewZ - fogStart) / (fogEnd - fogStart));
	fogAmount *= fogAmount;
	float3 fogColor = float3(0.6016f, 0.746f, 0.7539f); // blue from skybox shader


    OUT.Color = float4(lerp(LOut, fogColor, fogAmount), 0.0f);

    return OUT;
}


#endif




////////////////////////////////////////////////////////////////////////////////
////////////////////////////////////////////////////////////////////////////////
//
//  DEFERRED LIGHT PREPASS
//
////////////////////////////////////////////////////////////////////////////////
////////////////////////////////////////////////////////////////////////////////
#ifdef ENTRY_LightPrepass
#define HAS_SHADOWMAP

cbuffer LightCB : register(b1)
{
	float4x4 mtxWorld;
	float4x4 mtxViewportProjInv;
	float3 lightPos;
	float lightMaxDistance;
	float3 lightDirection;
	float lightDecay;
	float3 lightIntensity;
	uint lightType;
};

Texture2D GBuffer0 : register(t0);
Texture2D GBuffer1 : register(t1);
Texture2D shadowMap : register(t2);



// Computes World-Space position from Clip-Space position
// cs.xy - clip space position [-1,1]
// cs.z - non-linear depth
float3 ReconstructWSFromCS(float3 cs)
{
	float4 vs = mul(mtxViewportProjInv, float4(cs, 1.0f));
	vs /= vs.w;
	vs.y *= -1.0f;

	return mul(mtxViewInv, vs).xyz;
}



// pos - screen-space pos (pixels)
void UnpackGBuffer(float2 pos,
	out float depth,
	out float3 normal,
	out float roughness)
{
	float2 tc = pos / screenRes;

	depth = GBuffer0.Sample(PointSampler, tc).r;
	
	float4 G1 = GBuffer1.Sample(PointSampler, tc);
	normal = normalize(float3(G1.r, G1.g, G1.b) * 2.0f - 1.0f);
	roughness = G1.a;
}




struct VS_INPUT_LIGHT_PREPASS
{
	float3 Position : POSITION;
};

struct VS_OUTPUT_LIGHT_PREPASS
{
	float4 Position : SV_Position;
};

VS_OUTPUT_LIGHT_PREPASS VS_LightPrepass(VS_INPUT_LIGHT_PREPASS IN)
{
	VS_OUTPUT_LIGHT_PREPASS OUT;

	float4 wPos = mul(mtxWorld, float4(IN.Position, 1.0f));
	OUT.Position = mul(mtxProj, wPos);

	return OUT;
}

#define FALLOFF_EPSILON 0.01f


struct PS_OUTPUT_LIGHT_PREPASS
{
	float4 Diffuse : SV_Target0;
	float Specular : SV_Target1;
};

PS_OUTPUT_LIGHT_PREPASS PS_LightPrepass(VS_OUTPUT_LIGHT_PREPASS IN)
{
	// --------------------------
	float GDepth;
	float3 N;
	float matRoughness;
	UnpackGBuffer(IN.Position.xy, GDepth, N, matRoughness);

	if (!(GDepth < 1.0f))
		discard;

	float3 clippos;
	clippos.xy = (IN.Position.xy - 0.5f) / screenRes.xy;
	clippos.xy = clippos.xy * 2.0f - 1.0f;
	clippos.z = GDepth;
	float3 wpos = ReconstructWSFromCS(clippos);

	// --------------------------
	float3 V = normalize(eyePos.xyz - wpos);
	float3 L;
	float falloff = 1.0f;
	float shadowFactor = 1.0f;
	if (lightType == 0 || true) // sun
	{
		L = -normalize(-sunPos);
		falloff = 1.0f;
		shadowFactor = CalculateShadowMapFactor(wpos);
	}
	else // point, unknown
	{
		L = lightPos - wpos;
		float distance = length(L);
		L /= distance;

		falloff = pow(saturate(1 - pow(distance / lightMaxDistance, 4)), 2) / pow(max(distance, FALLOFF_EPSILON), lightDecay);
	}

	float glossiness = RoughnessToAlpha(matRoughness);
	float3 H = normalize(L + V); // computed in PS as we need the world position from the GBuffer


	// Calculate BRDF
	float diff, spec;

	BRDF_Blinn(L, V, N, H, glossiness, diff, spec);
//	BRDF_Phong(L, V, N, glossiness, diff, spec);


	// Calculate radiometric irradiance
	float3 irradiance = lightIntensity * falloff * shadowFactor;
	float incidentIrradiance = irradiance * saturate(dot(N, L)); // Lambert

	// --------------------------
	
	PS_OUTPUT_LIGHT_PREPASS OUT;

	OUT.Diffuse.rgb = diff * incidentIrradiance;
	OUT.Specular = spec * incidentIrradiance;

	return OUT;
}


#endif



////////////////////////////////////////////////////////////////////////////////
////////////////////////////////////////////////////////////////////////////////
//
//  DEFERRED SHADING
//
////////////////////////////////////////////////////////////////////////////////
////////////////////////////////////////////////////////////////////////////////

#if defined(ENTRY_DeferredShading) || defined(ENTRY_DeferredShadingTerrain)

static float3 fakeGI = float3(0.18f, 0.18f, 0.18f);

Texture2D GBufferDepth : register(t0); // Depth
Texture2D GBuffer1 : register(t1); // Normal + Roughness
Texture2D LightDiffuse : register(t2);
Texture2D LightSpecular : register(t3);

struct VS_INPUT_DEFERRED_SHADING
{
	float3 Position : POSITION;
	float2 TexCoord : TEXCOORD0;
};

struct VS_OUTPUT_DEFERRED_SHADING
{
	float4 Position : SV_Position;
	float2 TexCoord : TEXCOORD0;
};

struct PS_OUTPUT_DEFERRED_SHADING
{
	float4 Color : SV_Target0;
};

#endif


#ifdef ENTRY_DeferredShading
cbuffer DeferredShadingCB : register(b1)
{
	float4x4 mtxWorld;
	float matMetalness;
};

Texture2D TextureMap : register(t4);

VS_OUTPUT_DEFERRED_SHADING VS_DeferredShading(VS_INPUT_DEFERRED_SHADING IN)
{
	VS_OUTPUT_DEFERRED_SHADING OUT;
	
	OUT.Position = mul(mtxProj, mul(mtxView, mul(mtxWorld, float4(IN.Position, 1.0f))));
	OUT.TexCoord = IN.TexCoord;

	return OUT;
}

PS_OUTPUT_DEFERRED_SHADING PS_DeferredShading(VS_OUTPUT_DEFERRED_SHADING IN)
{
	PS_OUTPUT_DEFERRED_SHADING OUT;

	// Note: IN.Position.xy is offset by 0.5
	float2 screenTC = IN.Position.xy / screenRes;

	// Determine albedo
	float3 albedo = TextureMap.Sample(LinearSampler, IN.TexCoord).rgb;

	// Determine final radiance
	float3 Ldiff		= LightDiffuse.Sample(PointSampler, screenTC).rgb;
	float3 Lspec		= LightSpecular.Sample(PointSampler, screenTC).r;
	float3 Lemissive	= float3(0, 0, 0);
	float3 Lglobal		= fakeGI; // TODO

	float3 kd = albedo * (1.0f - matMetalness);
	float3 ks = lerp(float3(0.03f, 0.03f, 0.03f), albedo, matMetalness);

	float3 Lout = Lemissive + albedo * Lglobal + kd * Ldiff + ks * Lspec;

	/*
	// Fog
	float viewZ = -dot(mtxView[2], float4(IN.WorldPos, 1.0f));
	float fogAmount = saturate((viewZ - fogStart) / (fogEnd - fogStart));
	fogAmount *= fogAmount;
	float3 fogColor = float3(0.6016f, 0.746f, 0.7539f); // blue from skybox shader
	*/

	// TODO: Lout may not be in [0,1] range (HDR) -> Apply tone mapping here
	OUT.Color = float4(Lout, 1.0f);

	return OUT;
}

#endif


////////////////////////////////////////////////////////////////////////////////
////////////////////////////////////////////////////////////////////////////////
//
//  TERRAIN DEFERRED SHADING
//
////////////////////////////////////////////////////////////////////////////////
////////////////////////////////////////////////////////////////////////////////
#ifdef ENTRY_DeferredShadingTerrain

cbuffer TerrainCB : register(b1)
{
	float terrainDMFadeRadius;
	float terrainMaxHeight;
	uint terrainHeightmapSz;
	float terrainSegSz;
	float2 detailmapSz;
	uint terrainNumLayers;
}

// vs
Texture2D vtxHeightMap : register(t0);

// ps
Texture2DArray TextureMap : register(t4);
Texture2DArray terrainLayerMask : register(t5);
Texture2D terrainColorMap : register(t6);



struct VS_INPUT
{
	float3 Position : POSITION;
	float2 TexCoord : TEXCOORD0;
};

struct VS_OUTPUT
{
	float4 Position : SV_Position;
	float3 WorldPos : TEXCOORD0;
	float2 TexCoord : TEXCOORD1;
};


// TODO: Get this out of here and create the terrain geometry on cpu !!!

float SampleVertexHeightmap(float2 texcoord)
{
	uint2 pos = { texcoord.x * terrainHeightmapSz, texcoord.y * terrainHeightmapSz };
	return vtxHeightMap[pos].r;
}

// Used to interpolate the vertex heightmap values
float SampleVertexHeightmapBilinear(float2 texcoord)
{
	float2 pixelSizeInTexcoords;
	pixelSizeInTexcoords.x = 1.0f / terrainHeightmapSz;
	pixelSizeInTexcoords.y = 1.0f / terrainHeightmapSz;

	// round to floor
	float2 remainder = texcoord % pixelSizeInTexcoords;
	float2 roundedTC = texcoord - remainder;

	// transform remainder from [0;pixelSizeInTexcoords] to [0;1]
	remainder /= pixelSizeInTexcoords;

	// linearly interpolate top and bottom samples
	float interpolated[2];
	interpolated[0] = lerp(
		SampleVertexHeightmap(roundedTC + float2(-0.5, -0.5) * pixelSizeInTexcoords),
		SampleVertexHeightmap(roundedTC + float2(0.5, -0.5) * pixelSizeInTexcoords), remainder.x);

	interpolated[1] = lerp(
		SampleVertexHeightmap(roundedTC + float2(-0.5, 0.5) * pixelSizeInTexcoords),
		SampleVertexHeightmap(roundedTC + float2(0.5, 0.5) * pixelSizeInTexcoords), remainder.x);

	return lerp(interpolated[0], interpolated[1], remainder.y);
}

VS_OUTPUT VS_DeferredShadingTerrain(VS_INPUT IN)
{
	VS_OUTPUT OUT;

	// Sample vertex height from vertex heightmap texture
	float4 wPos = float4(IN.Position, 1.0f);
	wPos.y = SampleVertexHeightmapBilinear(IN.TexCoord) * terrainMaxHeight;

	OUT.WorldPos = wPos.xyz;
	OUT.Position = mul(mtxProj, mul(mtxView, wPos));
	OUT.TexCoord = IN.TexCoord;

	return OUT;
}

// ---------------------------

float terrain_fade_factor(float radius)
{
	float factor = -0.05f * (radius - terrainDMFadeRadius) + 1.0f;
	return saturate(factor);
}

float3 BlendOverlay(float3 b, float3 a)
{
	return (a > 0.5f) * (1.0f - (1.0f - 2.0f * (a - 0.5f)) * (1.0f - b)) + (a <= 0.5f) * ((2.0f * a) * b);
}

PS_OUTPUT_DEFERRED_SHADING PS_DeferredShadingTerrain(VS_OUTPUT IN)
{
	PS_OUTPUT_DEFERRED_SHADING OUT;

	// Note: IN.Position.xy is offset by 0.5
	float2 screenTC = IN.Position.xy / screenRes;


	// Determine albedo
	float3 detailMapAlbedo = float3(0, 0, 0);
	float3 terrainTC;
	terrainTC.xy = IN.TexCoord;
	float3 detailmapTC;
	detailmapTC.xy = IN.WorldPos.xz / detailmapSz;
	for (uint iLayer = 0; iLayer < 3; ++iLayer)
	{
		terrainTC.z = (float)iLayer;
		float maskSample = terrainLayerMask.Sample(LinearSampler, terrainTC).r;

		detailmapTC.z = (float)iLayer;
		float3 textureMapSample = TextureMap.Sample(LinearSampler, detailmapTC).rgb;
		detailMapAlbedo = lerp(detailMapAlbedo, textureMapSample, maskSample);
	}

	float3 colorMapAlbedo = terrainColorMap.Sample(LinearSampler, terrainTC).rgb;

	float dirln = length(eyePos.xyz - IN.WorldPos.xyz);
	float terrainFadeFactor = saturate(terrain_fade_factor(dirln));

	float3 albedo = lerp(colorMapAlbedo, saturate(BlendOverlay(detailMapAlbedo, colorMapAlbedo)), terrainFadeFactor);

	// Determine final radiance
	float3 Ldiff		= LightDiffuse.Sample(PointSampler, screenTC).rgb;
	float3 Lspec		= LightSpecular.Sample(PointSampler, screenTC).r;
	float3 Lemissive	= float3(0, 0, 0);
	float3 Lglobal		= fakeGI; // TODO

	float ks = 0; // todo?

	float3 Lout = Lemissive + albedo * Lglobal + albedo * Ldiff + ks * Lspec;

	// Fog
//	float viewZ = -dot(mtxView[2], float4(IN.WorldPos, 1.0f));
//	float fogAmount = saturate((viewZ - fogStart) / (fogEnd - fogStart));
//	fogAmount *= fogAmount;
//	float3 fogColor = float3(0.6016f, 0.746f, 0.7539f); // blue from skybox shader

	// TODO: Lout may not be in [0,1] range (HDR) -> Apply tone mapping here
	OUT.Color = float4(Lout, 1.0f);

	return OUT;
}

#endif






//

#ifdef HAS_SHADOWMAP
float CalculateShadowMapFactor(float3 WorldPos)
{
	float4 sunPos = mul(mtxSunViewProj, float4(WorldPos, 1.0f));
	sunPos /= sunPos.w;
	sunPos.x = (sunPos.x + 1.0f) * 0.5f;
	sunPos.y = 1.0f - (sunPos.y + 1.0f) * 0.5f;
	sunPos.z = saturate(sunPos.z - 0.01f);

	float2 smTCOffset = 1.0f / screenRes;

	float avgSample = 0.0f;
	for (int x = -2; x <= 2; ++x)
		for (int y = -2; y <= 2; ++y)
			avgSample += saturate(shadowMap.SampleCmpLevelZero(ShadowMapSampler, sunPos.xy + float2(x, y) * smTCOffset, sunPos.z));

	return (avgSample / 16.0f);
}
#endif


