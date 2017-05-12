/////////////////////////////////////////////////////////////////////////
//
//
//		SpeedPoint Terrain Shader
//
//
/////////////////////////////////////////////////////////////////////////

static const float PI = 3.14159265f;

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

cbuffer TerrainCB : register(b1)
{
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
Texture2D shadowMap : register(t4);

SamplerState PointSampler : register(s0);
SamplerState LinearSampler : register(s1);
SamplerComparisonState ShadowMapSampler : register(s2);

// ---------------------------------------------------------

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
        SampleVertexHeightmap(roundedTC + float2(-0.5,-0.5) * pixelSizeInTexcoords),
        SampleVertexHeightmap(roundedTC + float2( 0.5,-0.5) * pixelSizeInTexcoords), remainder.x);

    interpolated[1] = lerp(
        SampleVertexHeightmap(roundedTC + float2(-0.5, 0.5) * pixelSizeInTexcoords),
        SampleVertexHeightmap(roundedTC + float2( 0.5, 0.5) * pixelSizeInTexcoords), remainder.x);

    return lerp(interpolated[0], interpolated[1], remainder.y);
}



VS_OUTPUT VS_terrain(VS_INPUT IN)
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

// ---------------------------------------------------------

struct PS_INPUT
{
    float4 Position : SV_Position;
    float3 Normal : TEXCOORD0;
    float3 WorldPos : TEXCOORD1;
    float2 TexCoord : TEXCOORD2;
    float3 ViewSpacePos : TEXCOORD3;
};

struct PS_OUTPUT
{
    float4 Color : SV_Target0;
};

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


float CalculateShadowMapFactor(float3 WorldPos)
{
	float4 sunPos = mul(mtxSunViewProj, float4(WorldPos, 1.0f));
	sunPos /= sunPos.w;
	sunPos.x = (sunPos.x + 1.0f) * 0.5f;
	sunPos.y = 1.0f - (sunPos.y + 1.0f) * 0.5f;
	sunPos.z = saturate(sunPos.z - 0.001f);

	float2 smTCOffset = 1.0f / screenRes;

	float avgSample = 0.0f;
	for (int x = -2; x <= 2; ++x)
		for (int y = -2; y <= 2; ++y)
			avgSample += saturate(shadowMap.SampleCmpLevelZero(ShadowMapSampler, sunPos.xy + float2(x, y) * smTCOffset, sunPos.z));

	return (avgSample / 16.0f);
}


////////////////////////////////////////////////////////////////////////////////
////////////////////////////////////////////////////////////////////////////////
//
//  MAIN FORWARD PIXEL SHADER
//
////////////////////////////////////////////////////////////////////////////////
////////////////////////////////////////////////////////////////////////////////
PS_OUTPUT PS_terrain(PS_INPUT IN)
{
    PS_OUTPUT OUT;

    float3 normal = normalize(IN.Normal);

    // Sample CM and DM
    float3 sampleCM = colorMap.Sample(LinearSampler, IN.TexCoord).rgb;
    float3 sampleDM = detailMap.Sample(LinearSampler, IN.WorldPos.xz / detailmapSz).rgb;

    // Calculate blended Diffuse Color
    float dirln = length(eyePos.xyz - IN.WorldPos.xyz);
    float terrainFadeFactor = saturate(terrain_fade_factor(dirln));

	float3 coloredDiffuse = saturate(BlendOverlay(sampleDM, sampleCM));
    float3 blendedDiffuse = lerp(sampleCM, coloredDiffuse, terrainFadeFactor);

    // Sample vtx Height
    float vtxHeight = SampleVertexHeightmapBilinear(IN.TexCoord);

    // Light Dir is assumed to be OUTGING directed
    float3 lightDir = normalize(sunPos.xyz);

    float monoLightIntensity = 2.0f;
    float3 lightIntensity = monoLightIntensity * float3(1.0f, 1.0f, 1.0f);
	lightIntensity *= CalculateShadowMapFactor(IN.WorldPos);

    float monoAmbient = 0.40f;
    float3 ambient = float3(monoAmbient, monoAmbient, monoAmbient);

    float lambert = saturate(dot(normal, lightDir));

	// Fog
	float viewZ = -dot(mtxView[2], float4(IN.WorldPos, 1.0f));
	float fogAmount = saturate((viewZ - fogStart) / (fogEnd - fogStart));
	fogAmount *= fogAmount;
	float3 fogColor = float3(0.6016f, 0.746f, 0.7539f); // blue from skybox shader

	// Final color
	float3 finalColor = lambert * (blendedDiffuse / PI) * lightIntensity + ambient * blendedDiffuse;
	OUT.Color = float4((1.0f - fogAmount) * finalColor + fogAmount * fogColor, 0.0f);


    // Sample alpha value
    float4 sampleMask = alphaMask.Sample(PointSampler, IN.TexCoord);
    //OUT.Color.a = 1.0f;
    OUT.Color.a = sampleMask.r;

    //OUT.Color = float4(terrainFadeFactor, 0, 0, 1.0f);
    //OUT.Color.r = sampleMask.r;
    //OUT.Color.gb = float2(0,0);

    //OUT.Color = (sampleCM * 0.5f * (sampleDM + ((1.0f - terrainFadeFactor) * (1.0f - sampleDM)))) * (lightingFactor * lightIntensity);

    //OUT.Color = float4(IN.Normal, 0.0f);
    //OUT.Color = vtxHeight.rrrr;

    //OUT.Color = lambert * (blendedDiffuse / PI) * lightIntensity;

    return OUT;
}


// Terrain Pixel Shader without detailmap
PS_OUTPUT PS_terrain_nodm(PS_INPUT IN)
{
}
