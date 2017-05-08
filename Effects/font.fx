/////////////////////////////////////////////////////////////////////////
//
//
//		Font Shader
//
//
/////////////////////////////////////////////////////////////////////////

cbuffer FontCB : register(b0)
{
    uint2 screenResolution;
};

Texture2D tex0 : register(t0);
SamplerState PointSampler : register(s0);

// ---------------------------------------------------------

struct VS_INPUT
{
    float4 Position : POSITION;
    float3 Normal : NORMAL;
    float3 Tangent : TANGENT;
    float2 TexCoord : TEXCOORD0;
};

struct VS_OUTPUT
{
    float4 Position : SV_Position;
    float2 TexCoord : TEXCOORD0;
};

VS_OUTPUT VS_font(VS_INPUT IN)
{
    VS_OUTPUT OUT;

    OUT.Position = IN.Position;
    OUT.Position.z = 0.1f;
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

////////////////////////////////////////////////////////////////////////////////
////////////////////////////////////////////////////////////////////////////////
//
//  MAIN FONT PIXEL SHADER
//
////////////////////////////////////////////////////////////////////////////////
////////////////////////////////////////////////////////////////////////////////
PS_OUTPUT PS_font(PS_INPUT IN)
{
    PS_OUTPUT OUT;

    float4 sample = tex0.Sample(PointSampler, IN.TexCoord);
    OUT.Color = sample;

    return OUT;
}
