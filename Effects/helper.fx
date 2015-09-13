// SpeedPoint Effect for helpers

cbuffer SceneCB : register(b0)
{    
    float4x4 mtxViewProj;    
    float4 sunPos;
    float4 eyePos;
};

cbuffer ObjectCB : register(b1)
{
    float4x4 mtxWorld;
    float3 diffColor;
};

// -----------------

struct a2v
{
	float4 Position : POSITION;
	float3 Color : COLOR;
};

struct v2f
{
	float4 Position : SV_Position;
	float3 Color : COLOR;
};

/////////////////////////// vertex shader //////////////////////////////
v2f VS_helper(a2v IN)
{
    v2f OUT;
    float4 wPos = mul(mtxWorld, float4(IN.Position.xyz, 1.0f));
    OUT.Position = mul(mtxViewProj, wPos);
	OUT.Color = IN.Color;

    return OUT;
}

//////////////////////////// pixel shader ////////////////////////////////
float4 PS_helper(v2f IN) : SV_Target0
{
    //return float4(1.0f, 0, 0, 0);
	
	float3 col = IN.Color.rgb * diffColor.rgb;
    return float4(col.x, col.y, col.z, 0);
}

