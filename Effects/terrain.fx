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
    float4x4 mtxViewProj;    
    float4 sunPos;
    float4 eyePos;
}

cbuffer TerrainCB : register(b1)
{    
    float terrainDMFadeRadius;
    float terrainMaxHeight;
    uint terrainHeightmapSz;
    float terrainSegSz;
}

Texture2D vtxHeightMap : register(t0);
Texture2D colorMap : register(t1);
Texture2D detailMap : register(t2);
Texture2D alphaMask : register(t3);
SamplerState MapSampler
{
    Filter = MIN_MAG_MIP_POINT;
    AddressU = WRAP;
    AddressV = WRAP;
};

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
    OUT.Position = mul(mtxViewProj, wPos);
    
    
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
    float3 sampleCM = colorMap.Sample(MapSampler, IN.TexCoord).rgb;
    float3 sampleDM = detailMap.Sample(MapSampler, IN.WorldPos.xz).rgb;

	// Calculate blended Diffuse Color
    float dirln = length(eyePos.xz - IN.WorldPos.xz);
    float terrainFadeFactor = saturate(terrain_fade_factor(dirln));

	float3 coloredDiffuse = BlendColor(sampleDM, sampleCM);
	float4 blendedDiffuse = float4(coloredDiffuse.r, coloredDiffuse.g, coloredDiffuse.b, 0) * terrainFadeFactor + float4(sampleCM,0) * (1.0f - terrainFadeFactor);
    
	// Sample vtx Height
    float vtxHeight = SampleVertexHeightmapBilinear(IN.TexCoord);
    
    // Calculate lighting factor. Using a fixed light dir.
	// Light Dir is assumed to be INCOMING directed
    float3 lightDir = normalize(float3(0, -0.8f, 0.1f));

	float monoLightIntensity = 5.5f;
	float4 lightIntensity = float4(monoLightIntensity, monoLightIntensity, monoLightIntensity, 0.0f);

    float monoAmbient = 0.9f;
	float4 ambient = float4(monoAmbient, monoAmbient, monoAmbient, 0);

    float lambert = saturate(dot(normal, -lightDir));

	OUT.Color = lambert * (blendedDiffuse / PI + ambient * blendedDiffuse) * lightIntensity;

	// Sample alpha value
	float4 sampleMask = alphaMask.Sample(MapSampler, IN.TexCoord);
	//OUT.Color.a = 1.0f;
	//OUT.Color = float4(terrainFadeFactor, 0, 0, 1.0f);
	OUT.Color.a = sampleMask.r;
	//OUT.Color.r = sampleMask.r;
	//OUT.Color.gb = float2(0,0);

	//OUT.Color = (sampleCM * 0.5f * (sampleDM + ((1.0f - terrainFadeFactor) * (1.0f - sampleDM)))) * (lightingFactor * lightIntensity);

    //OUT.Color = float4(IN.Normal, 0.0f);
	//OUT.Color = vtxHeight.rrrr;
               
    return OUT;
}


// Terrain Pixel Shader without detailmap
PS_OUTPUT PS_terrain_nodm(PS_INPUT IN)
{
}