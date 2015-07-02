/////////////////////////////////////////////////////////////////////////
//
//
//		Illum Shader
//
//
/////////////////////////////////////////////////////////////////////////

cbuffer SceneCB : register(b0)
{    
    float4x4 mtxViewProj;    
    float4 sunPos;
    float4 eyePos;
}
cbuffer ObjectCB : register(b1)
{
    float4x4 mtxWorld;
    float matAmbience;
    float3 matEmissive;
}
float4x4 mtxWorldViewProj;
Texture2D textureMap : register(t0);
SamplerState TextureMapSampler
{
    Filter = MIN_MAG_MIP_POINT;
    AddressU = WRAP;
    AddressV = WRAP;
};

static float PI = 3.14159265358f;

// ---------------------------------------------------------

struct VS_INPUT
{
    float3 Position : POSITION;
    float2 TexCoord : TEXCOORD0;
};

struct VS_OUTPUT
{
    float4 Position : SV_Position; 
    float2 TexCoord : TEXCOORD0;
    float3 WorldPos : TEXCOORD1;
};

VS_OUTPUT VS_skybox(VS_INPUT IN)
{
    VS_OUTPUT OUT;        

    float4x4 mtxWorldInv = transpose(mtxWorld);

    // Convert Position from Object into World-, Camera- and Projection Space
    float4 wPos = mul(mtxWorld, float4(IN.Position,1.0f));        
    OUT.WorldPos = wPos.xyz;
    OUT.Position = mul(mtxViewProj, wPos);

    OUT.TexCoord = IN.TexCoord;
    
    return OUT;
}

// ---------------------------------------------------------

struct PS_INPUT
{
    float4 Position : SV_Position;
    float2 TexCoord : TEXCOORD0;
    float3 WorldPos : TEXCOORD1;
};

struct PS_OUTPUT
{
    float4 Color : SV_Target0;
};

////////////////////////////////////////////////////////////////////////////////
////////////////////////////////////////////////////////////////////////////////
//
//  MAIN ILLUM PIXEL SHADER
//
////////////////////////////////////////////////////////////////////////////////
////////////////////////////////////////////////////////////////////////////////
PS_OUTPUT PS_skybox(PS_INPUT IN)
{
	PS_OUTPUT OUT;
	float3 texSample = textureMap.Sample(TextureMapSampler, IN.WorldPos).rgb;

	OUT.Color = float4(texSample.r, texSample.g, texSample.b, 1.0f);
	return OUT;
}