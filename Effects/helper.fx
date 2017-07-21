// SpeedPoint Effect for helpers

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
};

cbuffer ObjectCB : register(b1)
{
    float4x4 mtxWorld;
    float3 helperColor;
};

// -----------------

struct a2v
{
    float3 Position : POSITION;
	float3 Normal : NORMAL;
    float3 Color : COLOR0;
};

struct v2f
{
    float4 Position : SV_Position;
    float3 Color : COLOR0;
	float3 Normal : TEXCOORD0;
};

/////////////////////////// vertex shader //////////////////////////////
v2f VS_helper(a2v IN)
{
    v2f OUT;
    float4 wPos = mul(mtxWorld, float4(IN.Position.xyz, 1.0f));
    OUT.Position = mul(mtxProj, mul(mtxView, wPos));
	OUT.Normal = normalize(mul(mtxWorld, float4(IN.Normal, 0.0f)).xyz);
	OUT.Color = IN.Color;

    return OUT;
}

//////////////////////////// pixel shader ////////////////////////////////
float4 PS_helper(v2f IN) : SV_Target0
{
	float lambert = 1.0f;
	if (length(IN.Normal) > 0)
		lambert = saturate(dot(IN.Normal, normalize(float3(1.0f, 1.0f, 1.0f))) * 0.48f + 0.5f);

	lambert = 1.0f;

	float3 col = IN.Color * helperColor.rgb * lambert;
	//col = float3(1.0f, 0, 0);
    return float4(col.x, col.y, col.z, 1.0f);
}

