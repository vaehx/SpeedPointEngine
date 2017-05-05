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
    float4x4 mtxProjInv;
    float4 sunPos;
	float4x4 mtxSunViewProj;
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
Texture2D depthBuffer : register(t1);

SamplerState PointSampler
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
    float3 ClipCoords : TEXCOORD2; // x,y are clip coords, z is view-depth
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

    float4 vtxPosVS = mul(mtxView, float4(vtxPosWS, 1.0f));
    OUT.Position = mul(mtxProj, vtxPosVS);
    OUT.ClipCoords = OUT.Position.xyz / OUT.Position.w;
    OUT.ClipCoords.z = -vtxPosVS.z;
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

float Contrast(float input, float power)
{
    bool inputAboveHalf = input > 0.5;
    float output = 0.5 * pow(saturate(2 * (inputAboveHalf ? 1 - input : input)), power);
    return inputAboveHalf ? 1 - output : output;
}

PS_OUTPUT PS_ParticleEmitter(VS_OUTPUT IN)
{
    PS_OUTPUT OUT;

    float4 color = textureMap.Sample(PointSampler, IN.TexCoord) * IN.Color;

    float2 normClipCoords = IN.ClipCoords.xy * 0.5f + 0.5f;
    normClipCoords.y *= -1.0f;

    float depthSample = depthBuffer.Sample(PointSampler, normClipCoords).r;
    float4 viewSpaceCoords = mul(float4(normClipCoords.x, normClipCoords.y, depthSample, 1.0f), mtxProjInv);
    float sceneDepth = -viewSpaceCoords.z / viewSpaceCoords.w;

    float softAlpha = Contrast((sceneDepth - IN.ClipCoords.z) * 0.4f, 4.0f);


    color.a *= softAlpha;

    OUT.Color = color;
    //OUT.Color = float4(softAlpha, softAlpha, softAlpha, 1.0f);

    return OUT;
}

