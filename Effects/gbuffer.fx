/////////////////////////////////////////////////////////////////////////
//
//
//		GBuffer Shader
//
//
/////////////////////////////////////////////////////////////////////////

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
}

SamplerState PointSampler : register(s0);
SamplerState LinearSampler : register(s1);

static float PI = 3.14159265358f;




struct PS_GBUFFER_OUTPUT
{
	float4 Buffer0 : SV_Target0; // Albedo.r, Albedo.g, Albedo.b, Roughness
	float2 Buffer1 : SV_Target1; // Normal.x, Normal.y
};

// vsNormal - normal in view-space
PS_GBUFFER_OUT PackGBuffer(float3 albedo, float roughness, float3 vsNormal)
{
	PS_GBUFFER_OUT OUT;
	OUT.Buffer0 = float4(albedo, roughness);
	OUT.Buffer1 = vsNormal.xy * 0.5f + 0.5f;
	return OUT;
}

/////////////////////////////////////////////////////////////////////////
//	General GBuffer pass
/////////////////////////////////////////////////////////////////////////

#ifdef ENTRY_zpass

cbuffer ObjectCB : register(b1)
{
	float4x4 mtxWorld;
	float matRoughness;
}

Texture2D textureMap : register(t0);
Texture2D normalMap : register(t1);


struct VS_INPUT
{
    float3 Position : POSITION;
    float3 Normal : NORMAL;
    float3 Tangent : TANGENT;
    float3 Color : COLOR0;
    float2 TexCoord : TEXCOORD0;
};

struct VS_OUTPUT
{
    float4 Position : SV_Position;
    float3 WorldPos : TEXCOORD0;
    float3 Color : COLOR0;
    float3 VSNormal : TEXCOORD1;
    float3 VSTangent : TEXCOORD2;
    float2 TexCoord : TEXCOORD3;
};

VS_OUTPUT VS_zpass(VS_INPUT IN)
{
    VS_OUTPUT OUT;

	float4x4 mtxWorldInv = transpose(mtxWorld);

    float4 wPos = mul(mtxWorld, float4(IN.Position, 1.0f));
    OUT.WorldPos = wPos.xyz;
    OUT.Position = mul(mtxProj, mul(mtxView, wPos));

    OUT.VSNormal = mul(mtxView, mul(mtxWorld, normalize(float4(IN.Normal, 0.0f)))).xyz;
    OUT.VSTangent = mul(mtxView, mul(mtxWorldInv, normalize(float4(IN.Tangent, 0.0f)))).xyz;
    OUT.TexCoord = IN.TexCoord;
    OUT.Color = IN.Color;

    return OUT;
}

PS_GBUFFER_OUTPUT PS_zpass(VS_OUTPUT IN)
{
	float3 albedo = textureMap.Sample(LinearSampler, IN.TexCoord).rgb;

	// TODO: Normal-mapping 
	float3 vsNormal = normalize(IN.VSNormal);

	return PackGBuffer(albedo, matRoughness, vsNormal);
}

#endif



/////////////////////////////////////////////////////////////////////////
//	Terrain GBuffer pass
/////////////////////////////////////////////////////////////////////////

#ifdef ENTRY_TerrainZPass

cbuffer TerrainCB : register(b1)
{
	float3 sunIntensity;
	float terrainDMFadeRadius;
	float terrainMaxHeight;
	uint terrainHeightmapSz;
	float terrainSegSz;
	float2 detailmapSz;
}

Texture2D vtxHeightMap : register(t0);
Texture2D colorMap : register(t1);
Texture2D detailMap : register(t2);
Texture2D alphaMask : register(t3);


struct VS_INPUT
{
	float3 Position : POSITION;
	float3 Normal : NORMAL;
	float2 TexCoord : TEXCOORD0;
};

struct VS_OUTPUT
{
	float4 Position : SV_Position;
	float3 Normal : TEXCOORD0;
	float3 WorldPos : TEXCOORD1;
	float2 TexCoord : TEXCOORD2;
	float3 ViewSpacePos : TEXCOORD3;
};

//-----------------------------------------------------------------------

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

VS_OUTPUT VS_TerrainZPass(VS_INPUT IN)
{
	VS_OUTPUT OUT;

	// Sample vertex height from vertex heightmap texture
	float4 wPos = float4(IN.Position, 1.0f);
	wPos.y = SampleVertexHeightmapBilinear(IN.TexCoord) * terrainMaxHeight;
	OUT.WorldPos = wPos.xyz;

	float4 vsPos = mul(mtxView, wPos);
	OUT.ViewSpacePos = vsPos.xyz;
	OUT.Position = mul(mtxProj, vsPos);


	// Calculate normal
	uint2 vtxTexelPos = { IN.TexCoord.x * terrainHeightmapSz, IN.TexCoord.y * terrainHeightmapSz };
	float4 neighborSamples = float4(
		vtxHeightMap[uint2(vtxTexelPos.x - 1, vtxTexelPos.y)].r * terrainMaxHeight,
		vtxHeightMap[uint2(vtxTexelPos.x, vtxTexelPos.y - 1)].r * terrainMaxHeight,
		vtxHeightMap[uint2(vtxTexelPos.x + 1, vtxTexelPos.y)].r * terrainMaxHeight,
		vtxHeightMap[uint2(vtxTexelPos.x, vtxTexelPos.y + 1)].r * terrainMaxHeight
		);

	float3 neighbor1 = float3(wPos.x - terrainSegSz, neighborSamples.x, wPos.z);
	float3 neighbor2 = float3(wPos.x, neighborSamples.y, wPos.z - terrainSegSz);
	float3 neighbor3 = float3(wPos.x + terrainSegSz, neighborSamples.z, wPos.z);
	float3 neighbor4 = float3(wPos.x, neighborSamples.w, wPos.z + terrainSegSz);

	float3 normal1 = normalize(cross(neighbor1 - wPos, neighbor2 - wPos));
	float3 normal2 = normalize(cross(neighbor2 - wPos, neighbor3 - wPos));
	float3 normal3 = normalize(cross(neighbor3 - wPos, neighbor4 - wPos));
	float3 normal4 = normalize(cross(neighbor4 - wPos, neighbor1 - wPos));
	float3 N = normalize(normal1 + normal2 + normal3 + normal4);

	OUT.Normal = -N;



	OUT.TexCoord = IN.TexCoord;

	return OUT;
}

//-----------------------------------------------------------------------

float terrain_fade_factor(float radius)
{
	float factor = -0.05f * (radius - terrainDMFadeRadius) + 1.0f;
	return saturate(factor);
}

float3 BlendOverlay(float3 b, float3 a)
{
	return (a > 0.5f) * (1.0f - (1.0f - 2.0f * (a - 0.5f)) * (1.0f - b))
		+ (a <= 0.5f) * ((2.0f * a) * b);
	/*return float3(
	(a.r < 0.5f ? 2.f * a.r * b.r : 1.f - 2.f * (1.f - a.r) * (1.f - b.r)),
	(a.g < 0.5f ? 2.f * a.g * b.g : 1.f - 2.f * (1.f - a.g) * (1.f - b.g)),
	(a.b < 0.5f ? 2.f * a.b * b.b : 1.f - 2.f * (1.f - a.b) * (1.f - b.b))
	);*/
}

PS_GBUFFER_OUTPUT PS_TerrainZPass(VS_OUTPUT IN)
{
	// Sample color- and detailmap
	float3 sampleCM = colorMap.Sample(LinearSampler, IN.TexCoord).rgb;
	float3 sampleDM = detailMap.Sample(LinearSampler, IN.WorldPos.xz / detailmapSz).rgb;

	// Sample alphamask
	float sampleAlpha = alphaMask.Sample(PointSampler, IN.TexCoord).r;

	// Calculate blended albedo
	float dirln = length(eyePos.xyz - IN.WorldPos.xyz);
	float terrainFadeFactor = saturate(terrain_fade_factor(dirln));

	float3 coloredAlbedo = saturate(BlendOverlay(sampleDM, sampleCM));
	float3 blendedAlbedo = lerp(sampleCM, coloredAlbedo, terrainFadeFactor);


	return PackGBuffer(blendedAlbedo, )

	OUT.Color = float4(blendedAlbdeo, sampleAlpha);

	return OUT;
}


#endif
