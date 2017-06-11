/////////////////////////////////////////////////////////////////////////
//
//
//		Illum Shader
//
//
/////////////////////////////////////////////////////////////////////////

cbuffer SceneCB : register(b0)
{
    float4x4 mtxView;
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

float4x4 mtxWorldViewProj;

SamplerState PointSampler : register(s0);
SamplerState LinearSampler : register(s1);
SamplerComparisonState ShadowMapSampler : register(s2);

static float PI = 3.14159265358f;

// ---------------------------------------------------------

struct VS_INPUT
{
    float3 Position : POSITION;
    float3 Normal : NORMAL;
    float3 Tangent : TANGENT;
    float3 Color : COLOR0;
    float2 TexCoord : TEXCOORD0;
};

struct VS_INPUT_SIMPLE
{
	float3 Position : POSITION;
	float3 Normal : NORMAL;
};

// ---------------------------------------------------------

float RoughnessToAlpha(float roughness)
{
	return 2.0f / (roughness * roughness) - 2.0f;
}

// Normalized phong for rendering equation
float BRDF_Phong(float3 L, float3 V, float3 N, float alpha, float cspec)
{
	float3 R = 2.0f * dot(L, N) - L;
	return (1.0f - cspec) / PI + cspec * (alpha + 2.0f) / (2.0f * PI) * pow(dot(R, V), alpha);
}

// Normalized blinn phong (approximation)
float BRDF_Blinn(float3 L, float3 V, float3 N, float3 H, float alpha, float cspec)
{
	return (1.0f - cspec) / PI + cspec * (alpha + 8.0f) / (8.0f * PI) * pow(dot(N, H), alpha);
}



////////////////////////////////////////////////////////////////////////////////
////////////////////////////////////////////////////////////////////////////////
//
//  FORWARD SHADER
//
////////////////////////////////////////////////////////////////////////////////
////////////////////////////////////////////////////////////////////////////////

#if defined(ENTRY_forward)
cbuffer ObjectCB : register(b1)
{
	float4x4 mtxWorld;
	float matRoughness;
}

Texture2D textureMap : register(t0);
Texture2D normalMap : register(t1);
Texture2D shadowMap : register(t2);
#endif

struct VS_OUTPUT_FORWARD
{
    float4 Position : SV_Position;
    float3 WorldPos : TEXCOORD0;
    float3 Color : COLOR0;
    float3 Normal : TEXCOORD1;
    float3 Tangent : TEXCOORD2;
    float2 TexCoord : TEXCOORD3;
};

VS_OUTPUT VS_forward(VS_INPUT IN)
{
    VS_OUTPUT OUT;

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

struct PS_INPUT
{
    float4 Position : SV_Position;
    float3 WorldPos : TEXCOORD0;
    float3 Color : COLOR0;
    float3 Normal : TEXCOORD1;
    float3 Tangent : TEXCOORD2;
    float2 TexCoord : TEXCOORD3;
};

struct PS_OUTPUT
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

PS_OUTPUT PS_forward(PS_INPUT IN)
{
    PS_OUTPUT OUT;

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





////////////////////////////////////////////////////////////////////////////////
////////////////////////////////////////////////////////////////////////////////
//
//  DEFERRED LIGHT PREPASS
//
////////////////////////////////////////////////////////////////////////////////
////////////////////////////////////////////////////////////////////////////////
#ifdef ENTRY_LightPrepass
cbuffer LightCB : register(b1)
{
	float4x4 mtxWorld;
	float3 lightPos;
	float3 lightIntensity;
	float lightMaxDistance;
	float lightDecay;
};

Texture2D GBuffer0 : register(t0);
Texture2D GBuffer1 : register(t1);
Texture2D GBufferDepth : register(t2);
#endif

struct VS_INPUT_LIGHT_PREPASS
{
	float3 Position : POSITION;
};

struct VS_OUTPUT_LIGHT_PREPASS
{
	float4 Position : SV_Position;
	float3 H : TEXCOORD0;
};

VS_OUTPUT VS_LightPrepass(VS_INPUT IN)
{
	VS_OUTPUT OUT;

	float4 wPos = mul(mtxWorld, float4(IN.Position, 1.0f));
	OUT.Position = mul(mtxProj, mul(mtxView, wPos));
	OUT.H = normalize((lightPos - wPos) + (eyePos - wPos));

	return OUT;
}

float4 PS_LightPrepass(VS_OUTPUT_LIGHT_PREPASS IN) : SV_Target0
{
	// TODO
	float2 screenTC;
	
	float4 G0 = GBuffer0.Sample(PointSampler, screenTC);
	float4 G1 = GBuffer1.Sample(PointSampler, screenTC);
	float GDepth = GBufferDepth.Sample(PointSampler, screenTC).r;

	float matRoughness = G0.z;
	float3 N = float3(G0.x, G0.y, sqrt(1.0f - G0.x*G0.x - G0.y*G0.y));

	// TODO: Reconstruct from SV_Position and GBufferDepth sample
	float3 wpos;

	float3 V = normalize(eyePos - wpos);
	float3 L = lightPos - wpos;
	float distance = length(L);
	L /= distance;

	float alpha = RoughnessToAlpha(matRoughness);
	float cspec = 0.2f; // TODO

	float brdf = BRDF_Blinn(L, V, N, IN.H, alpha, cspec);

	float epsilon = 0.01f; // 1cm
	float falloff = pow(saturate(1 - pow(distance / lightMaxDistance, 4)), 2) / pow(max(distance, epsilon), lightDecay);

	float3 irradiance = lightIntensity * falloff;

	return float4(brdf * irradiance * dot(V, N), 0);
}




////////////////////////////////////////////////////////////////////////////////
////////////////////////////////////////////////////////////////////////////////
//
//  DEFERRED SHADING
//
////////////////////////////////////////////////////////////////////////////////
////////////////////////////////////////////////////////////////////////////////
#ifdef ENTRY_DeferredShading
cbuffer DeferredShadingCB : register(b1)
{
};

Texture2D GBuffer0 : register(t0);
Texture2D GBuffer1 : register(t1);
Texture2D GBufferDepth : register(t2);
Texture2D LightBuffer : register(t3);
#endif

struct VS_INPUT_DEFERRED_SHADING
{
	float3 Position : POSITION;
};

struct VS_OUTPUT_DEFERRED_SHADING
{
	float4 Position : SV_Position;
};

struct PS_OUTPUT_DEFERRED_SHADING
{
	float4 Color : SV_Target0;
	float Depth : SV_Depth;
};

PS_OUTPUT_DEFERRED_SHADING PS_DeferredShading(PS_INPUT IN)
{
	PS_OUTPUT OUT;

	// TODO: Determine FBO texture coordinates from SV_Position
	float2 screenTC;

	// Determine final pixel color
	float3 lightBufferSample = LightBuffer.Sample(PointSampler, screenTC);
	float3 Lemissive	= float3(0, 0, 0);
	float3 Lglobal		= float3(0.1f); // TODO
	float3 Lout			= Lemissive + Lglobal + lightBufferSample;
	
	OUT.Color = float4(Lout, 1.0f);

	// Copy depth from gbuffer
	float GDepth = GBufferDepth.Sample(PointSampler, screenTC);
	OUT.Depth = GDepth;

	return OUT;
}
