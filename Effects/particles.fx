/////////////////////////////////////////////////////////////////////////
//
//
//		Particles Shader
//
//
/////////////////////////////////////////////////////////////////////////

cbuffer SceneCB : register(b0)
{    
	float4x4 mtxView;
	float4x4 mtxProj; 
	float4 sunPos;
	float4 eyePos;
}

cbuffer EmitterCB : register(b1)
{
	float4x4 mtxWorld;
	uint time;
	uint particleLifetime;
	float particleSize;
	float particleSpeed; // units/sec
}

Texture2D textureMap : register(t0);

SamplerState TextureMapSampler
{
   	Filter = MIN_MAG_MIP_POINT;
   	AddressU = WRAP;
   	AddressV = WRAP;
};

// ---------------------------------------------------------

struct VS_INPUT
{
	float3 Position : POSITION;
	float3 Normal : NORMAL;
	float2 TexCoord : TEXCOORD0;
	float3 ParticleDirection : TEXCOORD1;
	uint ParticleStartTime : TEXCOORD2;
};

struct VS_OUTPUT
{
	float4 Position : SV_Position;
	float4 Color : COLOR0;
	float3 Normal : TEXCOORD0;
	float2 TexCoord : TEXCOORD1;
};

VS_OUTPUT VS_ParticleEmitter(VS_INPUT IN)
{
	VS_OUTPUT OUT;

	uint particleTime = time - IN.ParticleStartTime;
	float particleTimeFrac = (float)particleTime / (float)particleLifetime;

	// Animate particle position
	float3 particlePos = IN.ParticleDirection * particleSpeed * (float)particleTime * 0.000001f; // us -> sec
	float3 vtxPos = particlePos + IN.Position * particleSize;

	// Align particle to camera
	float3 cameraRightWS	= float3(mtxView[0][0], mtxView[0][1], mtxView[0][2]);
	float3 cameraUpWS		= float3(mtxView[1][0], mtxView[1][1], mtxView[1][2]);
	float3 cameraForwardWS	= float3(mtxView[2][0], mtxView[2][1], mtxView[2][2]);
	float3 emitterPosWS = float3(mtxWorld[3][0], mtxWorld[3][1], mtxWorld[3][2]);
	float3 vtxPosWS = emitterPosWS + cameraRightWS * vtxPos.x + cameraUpWS * vtxPos.y + cameraForwardWS * vtxPos.z;

	OUT.Position = mul(mtxProj, mul(mtxView, float4(vtxPosWS, 1.0f)));
	OUT.Normal = normalize(mul(mtxWorld, normalize(float4(IN.Normal, 0.0f))).xyz);
	OUT.TexCoord = IN.TexCoord;

	// Animate particle alpha
	OUT.Color = float4(1.0f, 1.0f, 1.0f, max(0.0f, 0.5f - 0.5f * particleTimeFrac));

	return OUT;
}

// ---------------------------------------------------------

struct PS_OUTPUT
{
	float4 Color : SV_Target0;
};

PS_OUTPUT PS_ParticleEmitter(VS_OUTPUT IN)
{
	PS_OUTPUT OUT;

	// Sample particle texture, ....	
	OUT.Color = textureMap.Sample(TextureMapSampler, IN.TexCoord) * IN.Color;
	//OUT.Color = float4(1.0f, 0.0f, 1.0f, 0.5f);

	return OUT;
}

