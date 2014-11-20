/////////////////////////////////////////////////////////////////////////
//
//
//		GBuffer Shader
//
//
/////////////////////////////////////////////////////////////////////////

cbuffer SceneCB : register(cb0)
{    
    float4x4 mtxView;
    float4x4 mtxProjection;
}
cbuffer ObjectCB : register(cb1)
{
    float4x4 mtxWorld;
}
float4x4 mtxWorldViewProj;
Texture2D textureMap : register(t0);
SamplerState TextureMapSampler
{
    Filter = MIN_MAG_MIP_POINT;
    AddressU = WRAP;
    AddressV = WRAP;
};

// ---------------------------------------------------------

struct VS_INPUT
{
    float4 Position : POSITION;
    float3 Normal : NORMAL;
    float2 TexCoord : TEXCOORD0;
};

struct VS_OUTPUT
{
    float4 Position : SV_Position;
    float3 WorldPos : TEXCOORD0;
    float3 Normal : TEXCOORD1;
    float2 TexCoord : TEXCOORD2;
};

VS_OUTPUT VS_forward(VS_INPUT IN)
{
    VS_OUTPUT OUT;

    float4 objPos = IN.Position;
    float4 scaledObjPos = normalize(objPos) * 0.3f;    
    float3 normal = normalize(IN.Normal);
    //normal = normalize(normal + scaledObjPos.xyz);
    //normal = normalize(objPos.xyz);    

    // Convert Position from Object into World-, Camera- and Projection Space
    float4 wPos = mul(mtxWorld, IN.Position);
    OUT.WorldPos = wPos.xyz;
    float4 sPos = mul(mtxView, wPos);
    float4 rPos = mul(mtxProjection, sPos);
    OUT.Position = rPos;
    OUT.Normal = mul(mtxWorld, float4(normal,0.0f)).xyz;         
    OUT.TexCoord = IN.TexCoord;
    
    return OUT;
}

// ---------------------------------------------------------

struct PS_INPUT
{
    float4 Position : SV_Position;
    float3 WorldPos : TEXCOORD0;
    float3 Normal : TEXCOORD1;    
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

float calc_phong(float3 N, float3 lightDirOut, float3 dirToEye, float roughness, float intensityIn)
{
    float diffuseFactor = roughness;
    float specularFactor = 1.0f - roughness;
    
    // Diffuse:
    float lambert = calc_lambert(N, lightDirOut);
    
    // Specular:
    float alpha = 2.0f;
    if (roughness > 0.0f)
        alpha = 1.0f / saturate(roughness); // the rougher the surface, the wider the specular highlight
        
    float3 R = 2.0f * saturate(dot(lightDirOut, N)) * N - lightDirOut; 
    float specular = pow(saturate(dot(R, dirToEye)), alpha) * 1.0f;
    
    // compose:
    float intensityOut = intensityIn * (diffuseFactor * lambert + specularFactor * specular);
    return intensityOut;
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
    
    // Get texture map color
    float4 texColor = textureMap.Sample(TextureMapSampler, IN.TexCoord);
    
    // Surface constants
    float glossiness = 0.0f;
    float roughness = 0.3f;       
    
    // Calculate lighting factor. Using a fixed light dir and eye pos for now
    float3 eyePos = float3(0, 5.0f, -10.0f);
    float3 lightDir = normalize(float3(1.0f, -1.0f, 0.4f));        
    float phong = calc_phong(normalize(IN.Normal), -lightDir, normalize(eyePos - IN.WorldPos), roughness, 1.0f);  
    
    // Global illumination "Ambient" fake
    float ambient = 0.3f;
    
    // Final lighting factor
    float lightingFactor = ambient + phong;    
    
    OUT.Color = texColor * lightingFactor;
    //OUT.Color = lightingFactor; 
    
    return OUT;
}