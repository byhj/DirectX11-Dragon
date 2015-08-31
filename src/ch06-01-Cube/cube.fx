cbuffer Matribuffer
{
	float4x4 World; 
};

struct VS_IN
{
	float3 Pos  : POSITION;
    float4 Color : COLOR0;
};

struct VS_OUT
{
	float4 Pos   : SV_POSITION;
    float4 Color : COLOR0;
};


VS_OUT VS_MAIN(VS_IN vs_in)
{
   VS_OUT vs_out;

   vs_out.Pos = mul( float4(vs_in.Pos, 1.0f), World);
   vs_out.Color = vs_in.Color;

    return vs_out;
}

float4 PS_MAIN(VS_OUT vs_out) : SV_TARGET
{
    return vs_out.Color;
}


technique CubeTech
{
    pass P0
    {
        SetVertexShader( CompileShader( vs_5_0, VS_MAIN() ) );
		SetGeometryShader( NULL );
        SetPixelShader( CompileShader(  ps_5_0, PS_MAIN() ) );
    }
}
