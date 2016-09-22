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

	//float3 texSample = textureMap.Sample(TextureMapSampler, D).rgb;

	//OUT.Color = float4(texSample.r, texSample.g, texSample.b, 1.0f);
	//OUT.Color = float4(D.x, D.y, D.z, 1.0f);

	float intensity = dot(D, normalize(sunPos.xyz));

	float4 blue = float4(0.6016f, 0.746f, 0.7539f, 1.0f);
	float4 yellow = float4(0.9218f, 0.9063f, 0.5156f, 1.0f) * 0.50f;
	OUT.Color = blue * (1.3f + 0.25f * intensity) + pow(saturate(intensity), 50.0f) * yellow;

//	OUT.Color = float4(intensity, intensity, intensity, 1.0f);
//	OUT.Color = float4(1.0f, 0, 0, 1.0f);
	

	return OUT;
}