float4x4 mtxWorld : WORLD;
float4x4 mtxView : VIEW;
float4x4 mtxProjection : PROJECTION;
float4x4 mtxWorldViewProj;

sampler2D tex0;

// ---------------------------------------------------------

struct VS_INPUT
{
	float4 Position : POSITION;
	float2 TexCoord : TEXCOORD0;
};

struct VS_OUTPUT
{
	float4 Position : POSITION;
	float2 TexCoord : TEXCOORD0;	
};

VS_OUTPUT VS_main(VS_INPUT IN)
{
	VS_OUTPUT OUT;

	// Convert Position from Object into World-, Camera- and Projection Space
	float4 wPos = mul(IN.Position, mtxWorld);
	float4 cPos = mul(wPos, mtxView);
	OUT.Position = mul(cPos, mtxProjection);	

	// simply pass tex coords
	OUT.TexCoord = IN.TexCoord;

	return OUT;
}

// ---------------------------------------------------------

// we are not allowed to use POSITION semantic for ps input anymore, so use another
struct PS_INPUT
{
	float2 TexCoord : TEXCOORD0;
};

struct PS_OUTPUT
{
	float4 Color : COLOR0;
};

PS_OUTPUT PS_main(PS_INPUT IN)
{
	PS_OUTPUT OUT;

	// Get the color of the input (probably the lighting buffer)
	OUT.Color.rgb = tex2D(tex0, IN.TexCoord).rgb;
	OUT.Color.a = 1.0;

	return OUT;
}

// ---------------------------------------------------------

technique PostTechnique
{
	pass P0
	{
		VertexShader = compile vs_3_0 VS_main();
		PixelShader = compile ps_3_0 PS_main();
	}
}