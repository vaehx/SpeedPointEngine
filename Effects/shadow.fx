/////////////////////////////////////////////////////////////////////////
//
//
//		Shadowmap Shader
//
//
/////////////////////////////////////////////////////////////////////////

cbuffer SceneCB : register(b0)
{
    float4x4 mtxView;
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

/////////////////////////////////////////////////////////////////////////

struct VS_INPUT
{
    float3 Position : POSITION;
};

struct VS_OUTPUT
{
    float4 Position : SV_Position;
};

VS_OUTPUT VS_shadowmap(VS_INPUT IN)
{
    VS_OUTPUT OUT;
    OUT.Position = mul(mtxSunViewProj, mul(mtxWorld, float4(IN.Position, 1.0f)));
    return OUT;
}

/////////////////////////////////////////////////////////////////////////

float4 PS_shadowmap(VS_OUTPUT IN) : SV_Target0
{
	return float4(1.0f, 1.0f, 1.0f, 1.0f);
}
