
///////////////////////////////////////////////////////////////////////////////////////////

			Render-Equation

///////////////////////////////////////////////////////////////////////////////////////////

struct Light
{
	float3 intensity; // on cpu: float intensity multiplied with float3 color
};


float3 RenderEquation(float3 N, float3 V, float L, float roughness, float3 albedo, float specularIntensity, float3 F0)
{
	float3 lightAccum;
	for (int i = 0; i < nLights; i++)
	{
		float3 intensity = CookTorrance(N, L, V, roughness, F0, albedo, specularIntensity) * lights[i].intensity * dot(N, L);
		lightAccum += intensity;
	}

	// lightAccum += ambient;
	return lightAccum;
}



///////////////////////////////////////////////////////////////////////////////////////////

			Lambert

///////////////////////////////////////////////////////////////////////////////////////////

float LambertBRDF(float3 diffuse)
{
	return diffuse / PI;
}



///////////////////////////////////////////////////////////////////////////////////////////

			Phong / Blinn-Phong

///////////////////////////////////////////////////////////////////////////////////////////

float NormalizedPhongBRDF(float3 N, float3 V, float3 L, float specExp)
{
	float3 R = 2 * dot(N, V) * N - V;
	float normalizationFactor = (specExp + 2.0f) / (2.0f * PI);
	return pow(dot(R, L), specExp) * normalizationFactor;
}



///////////////////////////////////////////////////////////////////////////////////////////

			Cook-Torrance

///////////////////////////////////////////////////////////////////////////////////////////

// m - roughness
float SchlickBeckmannNDF(float m, float NoH)
{
	float x = NoH + m - 1.0f;
	float xSq = x * x;
	return (pow(m, 3.0f) * x) / (NoH * pow(m * xSq - xSq + pow(m, 2.0f), 2.0f));
}

float GGXNDF(float roughness, float NoH)
{
	float alphaSq = pow(roughness, 4.0f); // remapped alpha = roughness^2
	return alphaSq / (PI * pow(pow(NoH, 2.0f) * (alphaSq - 1.0f) + 1.0f, 2.0f));
}

float SchlickGeomTerm(float roughness, float NoL, float NoV)
{
	float k = pow(roughness + 1.0f, 2.0f) / 8.0f;
	float GL = NoL / (NoL * (1.0f - k) + k);
	float GV = NoV / (NoV * (1.0f - k) + k);
	return GL * GV;
}

float3 SchlickFresnel(float3 F0, float VoN)
{
	return F0 + (1.0f - F0) * pow(1.0f - VoN, 5.0f);
}

float3 CookTorranceBRDF(float3 N, float3 L, float3 V, float roughness, float3 F0)
{
	float3 H = normalize(L + V);
	float NoL = saturate(dot(N, L));
	float NoH = saturate(dot(N, H));
	float NoV = saturate(dot(N, V));

	float D = SchlickBeckmannNDF(roughness, NoH);
	float G = SchlickGeomTerm(roughness, NoL, NoV);
	float3 F = SchlickFresnel(F0, NoV);
	return (D * F * G) / (4.0f * NoL * NoV);
}

// The model
float3 CookTorrance(float3 N, float3 L, float3 V, float roughness, float3 F0, float3 albedo, float specularIntensity)
{
	float3 diffuse = LambertBRDF(albedo) * (1.0f - specularIntensity);
	float3 specular = CookTorranceBRDF(N, L, V, roughness, F0) * specularIntensity;
	return diffuse + specular;
}
