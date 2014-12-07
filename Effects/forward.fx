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
    float4x4 mtxProjection;
    float4 sunPos;
    float4 eyePos;
}
cbuffer ObjectCB : register(b1)
{
    float4x4 mtxWorld;
}
float4x4 mtxWorldViewProj;
Texture2D textureMap : register(t0);
Texture2D normalMap : register(t1);
SamplerState TextureMapSampler
{
    Filter = MIN_MAG_MIP_POINT;
    AddressU = WRAP;
    AddressV = WRAP;
};
SamplerState NormalMapSampler
{
    Filter = MIN_MAG_MIP_LINEAR;
    AddressU = WRAP;
    AddressV = WRAP;
};

// ---------------------------------------------------------

struct VS_INPUT
{
    float3 Position : POSITION;
    float3 Normal : NORMAL;
    float3 Tangent : TANGENT;
    float2 TexCoord : TEXCOORD0;
};

struct VS_OUTPUT
{
    float4 Position : SV_Position;
    float3 WorldPos : TEXCOORD0;
    float3 Normal : TEXCOORD1;
    float3 Tangent : TEXCOORD2;    
    float2 TexCoord : TEXCOORD3;
};

VS_OUTPUT VS_forward(VS_INPUT IN)
{
    VS_OUTPUT OUT;        

    float4x4 mtxWorldInv = transpose(mtxWorld);

    // Convert Position from Object into World-, Camera- and Projection Space
    float4 wPos = mul(mtxWorld, float4(IN.Position,1.0f));
    OUT.WorldPos = wPos.xyz;
    
    float4 sPos = mul(mtxView, wPos);
    float4 rPos = mul(mtxProjection, sPos);
    OUT.Position = rPos;
    
    
    
    
    OUT.Normal = normalize(mul(mtxWorld, normalize(float4(IN.Normal,0.0f))).xyz);
    OUT.Tangent = normalize(mul(mtxWorldInv, normalize(float4(IN.Tangent,0.0f))).xyz);         
    OUT.TexCoord = IN.TexCoord;
    
    return OUT;
}

// ---------------------------------------------------------

struct PS_INPUT
{
    float4 Position : SV_Position;
    float3 WorldPos : TEXCOORD0;
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
    float3 sampledNormal = normalMap.Sample(NormalMapSampler, IN.TexCoord).rgb;
    sampledNormal = 2.0f * sampledNormal - 1.0f;    
    float3 bumpNormal = mul(matTW, normalize(sampledNormal));    
    
    normal = bumpNormal;
    
    //OUT.Color = float4(normalize(normal), 1.0f);
    //return OUT;
    
    
    
    // Get texture map color
    float4 texColor = textureMap.Sample(TextureMapSampler, IN.TexCoord);
    
    // Surface constants
    float roughness = 0.5f;       
    
    // Calculate lighting factor. Using a fixed light dir and eye pos for now    
    float3 lightDir = normalize(float3(0, -0.8f, 0.8f));        
    
    float3 dirToEye = normalize(eyePos.xyz - IN.WorldPos);
            
    float3 phong = calc_phong(normalize(normal), -lightDir, dirToEye, roughness, 1.0f);  
    
    // Global illumination "Ambient" fake
    float ambient = 0.3f;
    
    // Final lighting factor
    float3 lightingFactor = phong + float3(ambient, ambient, ambient);        
    
    OUT.Color = texColor * float4(lightingFactor, 1.0f);
    //OUT.Color = lightingFactor; 
    
    return OUT;
}