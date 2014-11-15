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
    float2 TexCoord : TEXCOORD0;
};

struct VS_OUTPUT
{
    float4 Position : SV_POSITION;
    float2 TexCoord : TEXCOORD0;
};

VS_OUTPUT VS_forward(VS_INPUT IN)
{
    VS_OUTPUT OUT;

    // Convert Position from Object into World-, Camera- and Projection Space
    float4 wPos = mul(mtxWorld, IN.Position);
    float4 sPos = mul(mtxView, wPos);
    float4 rPos = mul(mtxProjection, sPos);
    OUT.Position = rPos;
    OUT.TexCoord = IN.TexCoord;
    
    return OUT;
}

// ---------------------------------------------------------

struct PS_INPUT
{
    float4 Position : SV_Position;
    float2 TexCoord : TEXCOORD0;
};

struct PS_OUTPUT
{
    float4 Color : SV_Target0;
};

PS_OUTPUT PS_forward(PS_INPUT IN)
{
    PS_OUTPUT OUT;
    OUT.Color = textureMap.Sample(TextureMapSampler, IN.TexCoord);
    return OUT;
}