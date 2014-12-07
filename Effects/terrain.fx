/////////////////////////////////////////////////////////////////////////
//
//
//		SpeedPoint Terrain Shader
//
//
/////////////////////////////////////////////////////////////////////////

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
    float2 padding;
}

Texture2D colorMap : register(t0);
Texture2D detailMap : register(t1);
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

VS_OUTPUT VS_terrain(VS_INPUT IN)
{
    VS_OUTPUT OUT;    

    // Convert Position from Object into World-, Camera- and Projection Space
    float4 wPos = float4(IN.Position, 1.0f);
    OUT.WorldPos = wPos.xyz;
        
    float4 sPos = mul(mtxView, wPos);
    float4 rPos = mul(mtxProjection, sPos);
    OUT.Position = rPos;
    
    OUT.Normal = normalize(IN.Normal);             
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
    
    // Calculate lighting factor. Using a fixed light dir    
    float3 lightDir = normalize(float3(0, -0.3f, 0.8f));
    float lightIntensity = 3.0f;                    
    float lambert = max(dot(-lightDir, normal),0);  
    
    //OUT.Color = float4(normal, 1.0f);
    //return OUT;
    
    // Global illumination "Ambient" fake
    float ambient = 0.3f;
    
    // Final lighting factor
    float lightingFactor = lambert + ambient;        
        
    OUT.Color = (sampleCM * sampleDM) * (lightingFactor * lightIntensity);
    //OUT.Color = lightingFactor; 
    
    return OUT;
}