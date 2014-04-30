float4x4 mtxWorld : WORLD;
float4x4 mtxView : VIEW;
float4x4 mtxProjection : PROJECTION;
float4x4 mtxWorldViewProj;

sampler2D tex0;

// ---------------------------------------------------------

struct VS_INPUT
{
    float4 Position : POSITION;
    float3 Normal : NORMAL;
    float3 Tangent : TANGENT;
    float2 Texcoord : TEXCOORD0;
};

struct VS_OUTPUT
{
    float4 Position : POSITION;
    float3 PosOut : TEXCOORD0;
    float2 Texcoord : TEXCOORD1;
    float2 Depth : TEXCOORD2;
    float3 Normal : TEXCOORD3;
    float3 Tangent : TEXCOORD4;
};

VS_OUTPUT VS_main(VS_INPUT IN)
{
    VS_OUTPUT OUT;

    // Convert Position from Object into World-, Camera- and Projection Space
    float4 wPos = mul(IN.Position, mtxWorld);
    float4 sPos = mul(wPos, mtxView);
    OUT.Position = mul(sPos, mtxProjection);
    OUT.PosOut = OUT.Position;

    // simply pass tex coords
    OUT.Texcoord = IN.Texcoord;

    // calc the depth
    OUT.Depth.x = OUT.Position.z;
    OUT.Depth.y = OUT.Position.w;

    // pass the normal, tangent and calculate binormal
    OUT.Normal = IN.Normal;
    OUT.Tangent = IN.Tangent;
    
    return OUT;
}

// ---------------------------------------------------------

// we are not allowed to use POSITION semantic for ps input anymore, so use another
struct PS_INPUT
{
    float4 PosOut : TEXCOORD0;
    float2 Texcoord : TEXCOORD1;
    float2 Depth : TEXCOORD2;
    float3 Normal : TEXCOORD3;
    float3 Tangent : TEXCOORD4;
};

struct PS_OUTPUT
{
    float4 Albedo : COLOR0;
    float4 Position : COLOR1;
    float4 Normal : COLOR2;
    float4 Tangent : COLOR3;
};

PS_OUTPUT PS_main(PS_INPUT IN)
{
    PS_OUTPUT OUT;

    // Get the albedo color
    //OUT.Albedo = tex2D(tex0, IN.Texcoord);
    OUT.Albedo = float4(1.0f, 0.0f, 1.0f, 0.0f);

    // pass position
    OUT.Position = IN.PosOut;
    OUT.Position.w = IN.Depth.x / IN.Depth.y;

    // pass normal, tangent and binormal
    // As render target is a texture, we have to scale it from [-1;1] to [0;1] range!
    OUT.Normal = float4(IN.Normal * 0.5f + 0.5f, 1.0f); 
    OUT.Tangent = float4(IN.Tangent * 0.5f + 0.5f, 1.0f);

    return OUT;
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