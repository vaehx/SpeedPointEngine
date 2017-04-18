// SpeedPoint Effect for helpers

cbuffer SceneCB : register(b0)
{
    float4x4 mtxView;
    float4x4 mtxProj;
    float4x4 mtxProjInv;
    float4 sunPos;
    float4 eyePos;
};

cbuffer ObjectCB : register(b1)
{
    float4x4 mtxWorld;
    float matAmbient;
    float3 helperColor;
};

// -----------------

struct a2v
{
    float3 Position : POSITION;
    float3 Color : COLOR0;
};

struct v2f
{
    float4 Position : SV_Position;
    float3 Color : COLOR0;
};

/////////////////////////// vertex shader //////////////////////////////
v2f VS_helper(a2v IN)
{
    v2f OUT;
    float4 wPos = mul(mtxWorld, float4(IN.Position.xyz, 1.0f));
    OUT.Position = mul(mtxProj, mul(mtxView, wPos));
    OUT.Color = IN.Color;

    return OUT;
}

//////////////////////////// pixel shader ////////////////////////////////
float4 PS_helper(v2f IN) : SV_Target0
{
    float3 col = IN.Color * helperColor.rgb;
    return float4(col.x, col.y, col.z, 0.5f);
}

