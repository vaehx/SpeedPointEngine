float4x4 mtxWorld : WORLD;
float4x4 mtxView : VIEW;
float4x4 mtxProjection : PROJECTION;

sampler2D tex0;

struct VS_INPUT
{
};

struct VS_OUTPUT
{
};

VS_OUTPUT VS_main(VS_INPUT IN)
{
    VS_OUTPUT OUT;
    return OUT;    
}

// ------------------------------------------------------------

struct PS_OUTPUT
{
};

PS_OUTPUT PS_main(VS_OUTPUT IN)
{
    PS_OUTPUT OUT;
    return OUT;
}


// ------------------------------------------------------------

technique MergeTechnique
{
    pass P0
    {
        VertexShader = compile vs_3_0 VS_main();
        PixelShader = compile ps_3_0 PS_main();
    }
}