/////////////////////////////////////////////////////////////////////////
//
//
//		GUI Shader
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
cbuffer ObjectCB : register(b1)
{
    float4x4 mtxWorld;
}
float4x4 mtxWorldViewProj;
Texture2D textureMap : register(t0);
SamplerState PointSampler : register(s0);

/////////////////////////////////////////////////////////////////////////

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
    float2 TexCoord : TEXCOORD0;
};

VS_OUTPUT VS_GUI(VS_INPUT IN)
{
    VS_OUTPUT OUT;

    float4 wPos = mul(mtxWorld, float4(IN.Position, 1.0f));
    OUT.Position = mul(mtxProj, mul(mtxView, wPos));
    OUT.Position.z = 0.1f;
    OUT.TexCoord = IN.TexCoord;

    return OUT;
}

/////////////////////////////////////////////////////////////////////////

struct PS_INPUT
{
    float4 Position : SV_Position;
    float2 TexCoord : TEXCOORD0;
};

struct PS_OUTPUT
{
    float4 Color : SV_Target0;
};

PS_OUTPUT PS_GUI(PS_INPUT IN)
{
    PS_OUTPUT OUT;
//	OUT.Color = float4(1.0f, 0, 0, 1.0f);
    float3 sample = textureMap.Sample(PointSampler, IN.TexCoord).rgb;
    float alpha = 1.0f;
    if (dot(sample, sample) <= 0.00001f)
        alpha = 0.0f;

    OUT.Color = float4(sample, alpha);
	OUT.Color = float4(sample, 0.5f);
    return OUT;
}
