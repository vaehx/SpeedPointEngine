

cbuffer Matrices : register(b0)
{
    float4x4 worldMtx;
    float4x4 viewProjMtx;
    float4x4 lightViewProjMtx;
    float4 camPos;
    float4 sunDirection;            
};

cbuffer Material : register(b1)
{
    float materialAmbience; // global illumination fake
    
};


struct VS_INPUT
{
    float4 Position : POSITION;           
};

struct VS_OUTPUT
{
    float4 Position : SV_POSITION;
    float4 LightPos : TEXCOORD0;
};

VS_OUTPUT vs_shadow(VS_INPUT Input)
{
    VS_OUTPUT Output;    
    
    float4 worldPos = mul(Input.Position, worldMtx);
    Output.Position = mul(worldPos, viewProjMtx);
    
    Output.LightPos = mul(worldPos, lightViewProjMtx);
    
    return Output;    
}

/////////////////////////////////////////////////////////////////////////////////////////////////////////////////











/////////////////////////////////////////////////////////////////////////////////////////////////////////////////



struct PS_INPUT
{
    float4 Position : SV_POSITION;
    float4 LightPos : TEXCOORD0;
};
struct PS_OUTPUT
{
    float4 Color : SV_TARGET;
};


// ------------------------------------------------------------------------------------------------------------
//      L A M B E R T
// ------------------------------------------------------------------------------------------------------------
float calc_lambert(float3 position, float3 normal)
{

}


// ------------------------------------------------------------------------------------------------------------
//      P H O N G
//
//  N - Normal of the surface at the fragment (normalized)
//  V - Direction from the fragment TO the viewer (normalized)
//  L - Direction from the fragment TO the Light source, or the negated sunlight direction (normalized)
//  diffuseColor - Diffuse Color of the fragment, e.g. sampled from texture map
//  ambience - Ambient factor  
//  roughness - 1.0 means fully diffuse, 0.0 means full specularity
//  specularExponent - the specular exponent
// ------------------------------------------------------------------------------------------------------------
float calc_phong(float3 N, float3 V, float3 L, float4 diffuseColor, float ambience, float roughness, float specularExponent)
{
    // Calculate the components of the Blinn-Phong model
    float ambient = ambience;    
    float diffuse = diffuseColor * dot(N, L); // lambertian diffuse
    float specular = pow(saturate(dot(N, E)), spepcularExponent);
    
    return ambient + diffuse + (1 - roughness) * specular;
}


// ------------------------------------------------------------------------------------------------------------
//      B L I N N - P H O N G
//
//  N - Normal of the surface at the fragment (normalized)
//  V - Direction from the fragment TO the viewer (normalized)
//  L - Direction from the fragment TO the Light source, or the negated sunlight direction (normalized)
//  diffuseColor - Diffuse Color of the fragment, e.g. sampled from texture map
//  ambience - Ambient factor  
//  roughness - 1.0 means fully diffuse, 0.0 means full specularity
//  specularExponent - the specular exponent
// ------------------------------------------------------------------------------------------------------------
float calc_blinn_phong(float3 N, float3 V, float3 L, float4 diffuseColor, float ambience, float roughness, float specularExponent)
{
    // Halfway vector calculation.
    // This could be done in VS and be interpolated. However, when switching to deferred rendering,
    // there is no possibility to interpolate it anymore.
    float3 H = normalize(L + V);
    
    // Calculate the components of the Blinn-Phong model
    float ambient = ambience;    
    float diffuse = diffuseColor * dot(N, L); // lambertian diffuse
    float specular = pow(saturate(dot(N, H)), spepcularExponent);
    
    return ambient + diffuse + (1 - roughness) * specular;  
}


PS_OUTPUT ps_shadow(PS_INPUT Input)
{
    PS_OUTPUT Output;
    
    float2 projTexCoord; // in [0;1]-Space        
    projTexCoord.x = Input.LightPos.x / Input.LightPos.w * 0.5f + 0.5f;
    projTexCoord.y = Input.LightPos.y / Input.LightPos.w * 0.5f + 0.5f;
    
    
    
    
    float reflectedIntensity = 0.0f;
                
    
    // do only sample if in lights view
    if (saturate(projTexCoord.x) == projTexCoord.x && saturate(projTexCoord.y) == projTexCoord.y)
    {
        // sample the depthmap
        float depthMapDepth = DepthMap.Sample(DepthMapSampler, projTexCoord).r;
        
        // get light depth to the current fragment that is actually to be rendered
        float lightDepth = Input.LightPos.z / Input.LightPos.w;
        
        if (lightDepth > depthMapDepth)
        {
            // shadow intensity: ambient only
            reflectedIntensity = materialAmbience; 
        }
        else
        {
                 
        }             
    }
}