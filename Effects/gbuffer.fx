float4x4 mtxWorld : WORLD;
float4x4 mtxView : VIEW;
float4x4 mtxProjection : PROJECTION;
float4x4 mtxWorldViewProj;

sampler2D tex0;

// ---------------------------------------------------------

struct VS_INPUT
{
    float3 Position : POSITION;
    float3 Normal : NORMAL;
    float3 Tangent : TANGENT;
    float2 Texcoord : TEXCOORD0;
};

struct VS_OUTPUT
{
    float4 Position : POSITION;
    float2 Texcoord : TEXCOORD0;
    float2 Depth : TEXCOORD1;
    float3 Normal : TEXCOORD2;
    float3 Tangent : TEXCOORD3;
    float3 Binormal : TEXCOORD4;
};

VS_OUTPUT VS_main(VS_INPUT IN)
{
    VS_OUTPUT OUT;

    // Convert Position from Object into World-, Camera- and Projection Space
    float4 wPos = mul(float4(IN.Position, 1.0f), mtxWorld);
    float4 sPos = mul(wPos, mtxView);
    OUT.Position = mul(sPos, mtxProjection);

    // simply pass tex coords
    OUT.Texcoord = IN.Texcoord;

    // calc the depth
    OUT.Depth.x = OUT.Position.z;
    OUT.Depth.y = OUT.Position.w;
    
    return OUT;
}

// ---------------------------------------------------------

struct PS_OUTPUT
{
    float4 Albedo : COLOR0;
    float4 Position : COLOR1;
    float4 Normal : COLOR2;
    float4 Tangent : COLOR3;
};

PS_OUTPUT PS_main(VS_OUTPUT IN)
{
    PS_OUTPUT OUT;
    OUT.Albedo = tex2D(tex0, IN.Texcoord);
    OUT.Position = IN.Position;
    OUT.Position.w = IN.Depth.x / IN.Depth.y;
    OUT.Normal.rgb = (normalize(IN.Normal) * 1.0f) * 0.5f; // from [-1;1] to [0;1]

    OUT.Normal.a = 1.0f; // TODO: What could this be used for?
}

// ---------------------------------------------------------

technique GBufferTechniqueTex
{
	pass P0
	{
		VertexShader = compile vs_3_0 VS_main();
		PixelShader = compile ps_3_0 PS_main();
	}
}

technique GBufferTechniqueDiffuse
{
	pass P0
	{
		VertexShader = compile vs_3_0 VS_main();
		PixelShader = compile ps_3_0 PS_main();
	}
}