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
    float4x4 mtxProjection;
    float4 sunPos;
    float4 eyePos;
}

cbuffer TerrainCB : register(b1)
{
    float terrainTexRatioU;
    float terrainTexRatioV;
    float terrainDMFadeRadius;
    float terrainMaxHeight;
    uint2 terrainHeightmapSz;
    float2 terrainSegSz;
}

Texture2D vtxHeightMap : register(t0);
Texture2D colorMap : register(t1);
Texture2D detailMap : register(t2);
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
    uint2 pos = { texcoord.x * terrainHeightmapSz.x, texcoord.y * terrainHeightmapSz.y };
    return vtxHeightMap[pos].r;
}

// Used to interpolate the vertex heightmap values
float SampleVertexHeightmapBilinear(float2 texcoord)
{    
    float2 pixelSizeInTexcoords;
    pixelSizeInTexcoords.x = 1.0f / terrainHeightmapSz.x;
    pixelSizeInTexcoords.y = 1.0f / terrainHeightmapSz.y;
    
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
    wPos.y = SampleVertexHeightmapBilinear(IN.TexCoord) * 10.0f;                                
    OUT.WorldPos = wPos.xyz;
        
    float4 sPos = mul(mtxView, wPos);
    float4 rPos = mul(mtxProjection, sPos);            
    OUT.Position = rPos;
    
    
    // Calculate normal
    uint2 vtxTexelPos = { IN.TexCoord.x * terrainHeightmapSz.x, IN.TexCoord.y * terrainHeightmapSz.y };
    float4 neighborSamples = float4(
        vtxHeightMap[uint2(vtxTexelPos.x - 1, vtxTexelPos.y)].r * terrainMaxHeight,
        vtxHeightMap[uint2(vtxTexelPos.x, vtxTexelPos.y - 1)].r * terrainMaxHeight,
        vtxHeightMap[uint2(vtxTexelPos.x + 1, vtxTexelPos.y)].r * terrainMaxHeight,
        vtxHeightMap[uint2(vtxTexelPos.x, vtxTexelPos.y + 1)].r * terrainMaxHeight
    );
    
    float3 neighbor1 = float3(wPos.x - terrainSegSz.x, neighborSamples.x, wPos.z);
    float3 neighbor2 = float3(wPos.x, neighborSamples.y, wPos.z - terrainSegSz.y);
    float3 neighbor3 = float3(wPos.x + terrainSegSz.x, neighborSamples.z, wPos.z);
    float3 neighbor4 = float3(wPos.x, neighborSamples.w, wPos.z + terrainSegSz.y);
        
    float3 normal1 = normalize(cross(neighbor1 - wPos, neighbor2 - wPos));
    float3 normal2 = normalize(cross(neighbor2 - wPos, neighbor3 - wPos));
    float3 normal3 = normalize(cross(neighbor3 - wPos, neighbor4 - wPos));
    float3 normal4 = normalize(cross(neighbor4 - wPos, neighbor1 - wPos));             
    float3 N = normalize(normal1 + normal2 + normal3 + normal4 + float3(0, 1.0f, 0));
    
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
    float specularFactor = 1.0f - roughness;
    
    // Diffuse:
    float lambert = calc_lambert(N, lightDirOut);
    
    // Specular:
    float alpha = 2.0f;
    if (roughness > 0.0f)
        alpha = 1.0f / saturate(roughness - 0.5f); // the rougher the surface, the wider the specular highlight
        
    float3 R = -lightDirOut + 2.0f * saturate(dot(lightDirOut, N)) * N; 
    float specular = pow(saturate(dot(R, dirToEye)), 16.0f);     
    
    // compose:
    float intensityOut = (intensityIn * (diffuseFactor * lambert + specularFactor * specular));
    return float3(intensityOut, intensityOut, intensityOut);
}

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
    
    
    //OUT.Color = float4(0.0f, 1.0f, 0.0f, 1.0f);
    //return OUT;    
    
    // Sample CM and DM
    float4 sampleCM = colorMap.Sample(MapSampler, IN.TexCoord);
    float4 sampleDM = detailMap.Sample(MapSampler, IN.WorldPos.xz);
    
    float vtxHeight = SampleVertexHeightmapBilinear(IN.TexCoord);
    
    uint2 pos = { IN.TexCoord.x * terrainHeightmapSz.x, IN.TexCoord.y * terrainHeightmapSz.y };
    vtxHeight = vtxHeightMap[pos];
    
    // Calculate lighting factor. Using a fixed light dir    
    float3 lightDir = normalize(float3(0, -0.3f, 0.8f));
    float lightIntensity = 10.0f;                    
    float lambert = max(dot(-lightDir, normal),0);  
    
    //OUT.Color = float4(normal, 1.0f);
    //return OUT;
    
    // Global illumination "Ambient" fake
    float ambient = 0.1f;
    
    // Final lighting factor
    float lightingFactor = lambert / PI + ambient;        
        
    float dirln = length(eyePos.xz - IN.WorldPos.xz);
    float terrainFadeFactor = terrain_fade_factor(dirln);
    OUT.Color = (sampleCM * 0.5f * (sampleDM + ((1.0f - terrainFadeFactor) * (1.0f - sampleDM)))) * (lightingFactor * lightIntensity); 

    //OUT.Color = float4(IN.Normal, 1.0f);
               
    return OUT;
}


// Terrain Pixel Shader without detailmap
PS_OUTPUT PS_terrain_nodm(PS_INPUT IN)
{
}