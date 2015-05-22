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
};

struct v2f
{
	float4 Position : SV_Position;
};

/////////////////////////// vertex shader //////////////////////////////
v2f VS_helper(a2v IN)
{
    v2f OUT;
    float4 wPos = mul(mtxWorld, float4(IN.Position.xyz, 1.0f));
    OUT.Position = mul(mtxViewProj, wPos);

    return OUT;
}

//////////////////////////// pixel shader ////////////////////////////////
float4 PS_helper(v2f IN) : SV_Target0
{
    return float4(1.0f, 0, 0, 0);
    return float4(diffColor.x, diffColor.y, diffColor.z, 0);
}

