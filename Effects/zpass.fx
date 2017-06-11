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
	float matRoughness;
}

float4x4 mtxWorldViewProj;
Texture2D textureMap : register(t0);
Texture2D normalMap : register(t1);

SamplerState PointSampler : register(s0);
SamplerState LinearSampler : register(s1);

static float PI = 3.14159265358f;



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
    float3 WorldPos : TEXCOORD0;
    float3 Color : COLOR0;
    float3 Normal : TEXCOORD1;
    float3 Tangent : TEXCOORD2;
    float2 TexCoord : TEXCOORD3;
};

VS_OUTPUT VS_zpass(VS_INPUT IN)
{
    VS_OUTPUT OUT;

	float4x4 mtxWorldInv = transpose(mtxWorld);

    float4 wPos = mul(mtxWorld, float4(IN.Position, 1.0f));
    OUT.WorldPos = wPos.xyz;
    OUT.Position = mul(mtxProj, mul(mtxView, wPos));

    OUT.Normal = normalize(mul(mtxWorld, normalize(float4(IN.Normal, 0.0f))).xyz);
    OUT.Tangent = normalize(mul(mtxWorldInv, normalize(float4(IN.Tangent, 0.0f))).xyz);
    OUT.TexCoord = IN.TexCoord;
    OUT.Color = IN.Color;

    return OUT;
}



/////////////////////////////////////////////////////////////////////////

struct PS_INPUT
{
    float4 Position : SV_Position;
    float3 WorldPos : TEXCOORD0;
    float3 Color : COLOR0;
    float3 Normal : TEXCOORD1;
    float3 Tangent : TEXCOORD2;
    float2 TexCoord : TEXCOORD3;
};

struct PS_OUTPUT
{
    float4 Buffer0 : SV_Target0;
	float2 Buffer1 : SV_Target1;
};

PS_OUTPUT PS_zpass(PS_INPUT IN)
{
	PS_OUTPUT OUT;

	float3 albedo = textureMap.Sample(LinearSampler, IN.TexCoord).rgb;
	OUT.Buffer0 = float4(albedo, matRoughness);

	// TODO: Normal-mapping 
	float3 N = IN.Normal;

	// Pack normal into two 16bit components
	OUT.Buffer1 = normalize(N.xy) * sqrt(N.z * 0.5 + 0.5);

	return OUT;
}
