
cbuffer MatrixBuffer
{
	float4x4 World; 
	float4x4 View;
	float4x4 Proj;
};

struct VS_IN
{
	float3 Pos  : POSITION;
    float4 Color : COLOR;
};

struct VS_OUT
{
	float4 Pos  : SV_POSITION;
    float4 Color : COLOR;
};

VS_OUT VS(VS_IN vs_in)
{
	VS_OUT vs_out;
	
	// Transform to homogeneous clip space.
	vs_out.Pos = mul(float4(vs_in.Pos, 1.0f), World);
	vs_out.Pos = mul( vs_out.Pos, View );
	vs_out.Pos = mul( vs_out.Pos, Proj );
	
	// Just pass vertex color into the pixel shader.
    vs_out.Color = vs_in.Color;
    
    return vs_out;
}

float4 PS(VS_OUT vs_out) : SV_Target
{
    return vs_out.Color;
}

technique11 ColorTech
{
    pass P0
    {
        SetVertexShader( CompileShader( vs_5_0, VS() ) );
		SetGeometryShader( NULL );
        SetPixelShader( CompileShader( ps_5_0, PS() ) );
    }
}
