/////////////////////////////////////////////////////////////////////////
//
//
//		GBuffer Shader
//
//
/////////////////////////////////////////////////////////////////////////

float4x4 mtxWorld : WORLD;
float4x4 mtxView : VIEW;
float4x4 mtxProjection : PROJECTION;
float4x4 mtxWorldViewProj;

// ---------------------------------------------------------

struct VS_INPUT
{
    float4 Position : POSITION;
};

struct VS_OUTPUT
{
    float4 Position : SV_POSITION;
};

VS_OUTPUT VS_forward(VS_INPUT IN)
{
    VS_OUTPUT OUT;

    // Convert Position from Object into World-, Camera- and Projection Space
    float4 wPos = IN.Position;
    float4 sPos = mul(mtxView, wPos);
    float4 rPos = mul(mtxProjection, sPos);
    OUT.Position = rPos;
    
    return OUT;
}

// ---------------------------------------------------------

struct PS_INPUT
{
    float4 Position : SV_Position;
};

struct PS_OUTPUT
{
    float4 Color : SV_Target0;
};

PS_OUTPUT PS_forward(PS_INPUT IN)
{
    PS_OUTPUT OUT;
    OUT.Color = float4(1.0f, 0, 0, 0.5f);
    return OUT;
}