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
cbuffer IllumCB : register(b1)
{
    float4x4 mtxWorld;
    float matAmbience;
    float3 matEmissive;
}
float4x4 mtxWorldViewProj;
TextureCube textureMap : register(t0);
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
};

struct VS_OUTPUT
{
    float4 Position : SV_Position; 
    float3 ObjPos : TEXCOORD0;
};

VS_OUTPUT VS_skybox(VS_INPUT IN)
{
    VS_OUTPUT OUT;        

    float4x4 mtxWorldInv = transpose(mtxWorld);

    // Convert Position from Object into World-, Camera- and Projection Space
    float4 wPos = mul(mtxWorld, float4(IN.Position,1.0f));        
    OUT.ObjPos = IN.Position;
	OUT.Position = mul(mtxViewProj, wPos);
    
    return OUT;
}

// ---------------------------------------------------------

struct PS_INPUT
{
    float4 Position : SV_Position;
    float3 ObjPos : TEXCOORD0;
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

	float3 D = normalize(IN.ObjPos);

	float3 texSample = textureMap.Sample(TextureMapSampler, D).rgb;

	OUT.Color = float4(texSample.r, texSample.g, texSample.b, 1.0f);
	//OUT.Color = float4(D.x, D.y, D.z, 1.0f);
	return OUT;
}