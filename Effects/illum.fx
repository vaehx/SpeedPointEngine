/////////////////////////////////////////////////////////////////////////
//
//
//		Illum Shader
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
	uint2 shadowMapRes;
	uint2 screenRes;
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
Texture2D normalMap : register(t1);
Texture2D shadowMap : register(t2);
SamplerState PointSampler
{
    Filter = MIN_MAG_MIP_POINT;
    AddressU = WRAP;
    AddressV = WRAP;
};
SamplerState LinearSampler
{
    Filter = MIN_MAG_MIP_LINEAR;
    AddressU = WRAP;
    AddressV = WRAP;
};
SamplerState ShadowMapSampler
{
	Filter = MIN_MAG_MIP_LINEAR;
	AddressU = BORDER;
	AddressV = BORDER;
	BorderColor = float4(1.0f, 1.0f, 1.0f, 1.0f);
};

static float PI = 3.14159265358f;

// ---------------------------------------------------------

struct VS_INPUT
{
    float3 Position : POSITION;
    float3 Normal : NORMAL;
    float3 Tangent : TANGENT;
    float3 Color : COLOR0;
    float2 TexCoord : TEXCOORD0;
};

struct VS_OUTPUT
{
    float4 Position : SV_Position;
    float3 WorldPos : TEXCOORD0;
    float3 Color : COLOR0;
    float3 Normal : TEXCOORD1;
    float3 Tangent : TEXCOORD2;
    float2 TexCoord : TEXCOORD3;
};

VS_OUTPUT VS_forward(VS_INPUT IN)
{
    VS_OUTPUT OUT;

    float4x4 mtxWorldInv = transpose(mtxWorld);

    // Convert Position from Object into World-, Camera- and Projection Space
    float4 wPos = mul(mtxWorld, float4(IN.Position,1.0f));
    OUT.WorldPos = wPos.xyz;

    OUT.Position = mul(mtxProj, mul(mtxView, wPos));




    OUT.Normal = normalize(mul(mtxWorld, normalize(float4(IN.Normal,0.0f))).xyz);
    OUT.Tangent = normalize(mul(mtxWorldInv, normalize(float4(IN.Tangent,0.0f))).xyz);
    OUT.TexCoord = IN.TexCoord;
    OUT.Color = IN.Color;

    return OUT;
}

VS_OUTPUT VS_illum(VS_INPUT IN)
{
    VS_OUTPUT OUT;
    OUT.WorldPos = float3(0, 0, 0);
    OUT.Position = float4(0, 0, 0, 0);
    OUT.Normal = float3(0, 0, 0);
    OUT.Tangent = float3(0, 0, 0);
    OUT.TexCoord = float2(0, 0);
    OUT.Color = float3(0, 0, 0);
    return OUT;
}

// ---------------------------------------------------------

struct PS_INPUT
{
    float4 Position : SV_Position;
    float3 WorldPos : TEXCOORD0;
    float3 Color : COLOR0;
    float3 Normal : TEXCOORD1;
    float3 Tangent : TEXCOORD2;
    float2 TexCoord : TEXCOORD3;
};

struct PS_OUTPUT
{
    float4 Color : SV_Target0;
};

float quadr_fade(float val)
{
    float a = (val - 1.0f);
    return -(a * a) + 1.0f;
}

// N should be normalized already
float calc_lambert(float3 N, float3 lightDirOut)
{
    return saturate(dot(N, lightDirOut));
}

float saturate_negpos(float val)
{
    return val * 0.5f + 0.5f;
}

float3 calc_phong(float3 N, float3 lightDirOut, float3 dirToEye, float roughness, float intensityIn)
{
    float diffuseFactor = roughness;
    float specularFactor = clamp(1.0f - roughness, 0, 0.6f);

    // Diffuse:
    float lambert = calc_lambert(N, lightDirOut);

    // Specular:
    float alpha = 2.0f;
    if (roughness > 0.0f)
        alpha = 1.0f / saturate(roughness - 0.5f); // the rougher the surface, the wider the specular highlight

    float3 R = -lightDirOut + 2.0f * saturate(dot(lightDirOut, N)) * N;
    float specular = pow(saturate(dot(R, dirToEye)), alpha);

    // compose:
    float intensityOut = (intensityIn * (diffuseFactor * lambert + specularFactor * specular));
    return float3(intensityOut, intensityOut, intensityOut);
}

////////////////////////////////////////////////////////////////////////////////
////////////////////////////////////////////////////////////////////////////////
//
//  MAIN ILLUM PIXEL SHADER
//
////////////////////////////////////////////////////////////////////////////////
////////////////////////////////////////////////////////////////////////////////
PS_OUTPUT PS_forward(PS_INPUT IN)
{
    PS_OUTPUT OUT;
    //OUT.Color = textureMap.Sample(PointSampler, IN.TexCoord);
    //return OUT;


    // calc binormal. Everything in World space!
    float3 normal = IN.Normal;
    float3 tangent = IN.Tangent;
    float3 binormal = cross(normal, tangent);

    // Calculate Tangent to World Space Rotation Matrix
    float3x3 matWT = float3x3(tangent, binormal, normal);
    float3x3 matTW = transpose(matWT);

    // Sample normal change in tangent space from NormalMap
    float3 sampledNormal = normalMap.Sample(LinearSampler, IN.TexCoord).rgb;
    sampledNormal.rg = 2.0f * sampledNormal.rg - 1.0f;
    float3 bumpNormal = mul(matTW, normalize(sampledNormal));

    //normal = bumpNormal;



    // Get texture map color
    float3 albedo = textureMap.Sample(PointSampler, IN.TexCoord).rgb;

    // Surface constants
    float matRoughness = 0.8f;

    // Calculate lighting factor. Using a fixed light dir and eye pos for now
    float3 L = normalize(sunPos.xyz);
    float3 V = normalize(eyePos.xyz - IN.WorldPos);

    float lambertBRDF = 1 / PI;

	float3 irradiance = float3(1.0f, 1.0f, 1.0f) * 4.0f;
	float3 ambient = float3(1.0f, 1.0f, 1.0f) * 0.2f;

	// Shadowing
	float4 sunPos = mul(mtxSunViewProj, float4(IN.WorldPos, 1.0f));
	sunPos /= sunPos.w;
	sunPos.x = (sunPos.x + 1.0f) * 0.5f;
	sunPos.y = -(sunPos.y + 1.0f) * 0.5f;
	float shadowMapSample = shadowMap.Sample(ShadowMapSampler, sunPos.xy).r;
	float shadowMapFactor = 1.0f;
	if (saturate(sunPos.z - 0.001f) > shadowMapSample)
		shadowMapFactor = 0.0f;

	irradiance *= shadowMapFactor;

    float3 LOut = IN.Color * saturate(albedo * (lambertBRDF * saturate(dot(normal, L)) * irradiance + ambient));

    OUT.Color = float4(LOut.r, LOut.g, LOut.b, 1.0f);
    return OUT;
}

PS_OUTPUT PS_illum(PS_INPUT IN)
{
    PS_OUTPUT OUT;
    OUT.Color = float4(1.0f, 1.0f, 1.0f, 1.0f);
    return OUT;
}
