float4x4 mtxWorld : WORLD;
float4x4 mtxView : VIEW;
float4x4 mtxProjection : PROJECTION;

sampler2D tex0;

struct VS_INPUT
{
	float4 Position : POSITION;
	float2 Texcoord : TEXCOORD0;
};

struct VS_OUTPUT
{
	float4 Position : POSITION;
	float2 Texcoord : TEXCOORD0;
};

VS_OUTPUT VS_main(VS_INPUT IN)
{
	VS_OUTPUT OUT;

	// Position
	float4 WorldPos = mul(IN.Position, mtxWorld);
		float4 CamPos = mul(WorldPos, mtxView);
		OUT.Position = mul(CamPos, mtxProjection);

	// pass Texcoord
	OUT.Texcoord = IN.Texcoord;


	return OUT;    
}

// ------------------------------------------------------------

struct PS_INPUT
{
	float2 Texcoord : TEXCOORD0;
};

struct PS_OUTPUT
{
	float4 Color : COLOR0;
};

PS_OUTPUT PS_main(PS_INPUT IN)
{
    PS_OUTPUT OUT;

    OUT.Color.rgb = tex2D(tex0, IN.Texcoord).rgb;
    OUT.Color.a = 1.0f;

    return OUT;
}


// ------------------------------------------------------------

technique LightingTechnique
{
    pass P0
    {
        VertexShader = compile vs_3_0 VS_main();
        PixelShader = compile ps_3_0 PS_main();
    }
}