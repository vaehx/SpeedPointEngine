/////////////////////////////////////////////////////////////////////////
//
//
//		GBuffer Shader
//
//
/////////////////////////////////////////////////////////////////////////

cbuffer MatrixCB : register(b0)
{
    float4x4 mtxWorld;
    float4x4 mtxView;
    float4x4 mtxProjection;
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
    float3 Normal : TEXCOORD0;
    float2 TexCoord : TEXCOORD1;
};

VS_OUTPUT VS_forward(VS_INPUT IN)
{
    VS_OUTPUT OUT;

    float4 objPos = IN.Position;
    float4 scaledObjPos = normalize(objPos) * 0.3f;    
    float3 normal = normalize(IN.Normal);
    normal = normalize(normal + scaledObjPos.xyz);    

    // Convert Position from Object into World-, Camera- and Projection Space
    float4 wPos = mul(mtxWorld, IN.Position);
    float4 sPos = mul(mtxView, wPos);
    float4 rPos = mul(mtxProjection, sPos);
    OUT.Position = rPos;
    OUT.Normal = mul(mtxWorld, float4(normal,0.0f)).xyz;
    //OUT.Normal = mul(mtxWorld, float4(normalize(IN.Normal + scaledObjPos.xyz),0.0f)).xyz;        
    OUT.TexCoord = IN.TexCoord;
    
    return OUT;
}

// ---------------------------------------------------------

struct PS_INPUT
{
    float4 Position : SV_Position;
    float3 Normal : TEXCOORD0;
    float2 TexCoord : TEXCOORD1;
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
float calc_lambert(float3 N, float3 lightDirIn)
{
    return quadr_fade(dot(lightDirIn, N) * 0.5f + 0.5f);
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
    
    // Calculate lighting factor. Using a fixed vector for now
    float3 lightDir = normalize(float3(1.0f, -1.0f, -0.5f));
    float lambert = calc_lambert(normalize(IN.Normal), -lightDir);
    
    // Global illumination "Ambient" fake
    float ambient = 0.0f;
    
    // Final lighting factor
    float lightingFactor = ambient + lambert;    
    
    OUT.Color = texColor * lightingFactor; 
    
    return OUT;
}