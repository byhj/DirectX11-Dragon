
cbuffer MatrixBuffer : register(b0)
{
	float4x4 g_World;
	float4x4 g_View;
	float4x4 g_Proj;
};

 struct VS_IN
{
    float4 Pos   : POSITION;
    float4 Color : COLOR0;
};

struct VS_OUT
{
    float4 Pos   : SV_POSITION;
    float4 Color : COLOR0;
};

VS_OUT VS( VS_IN vs_in)
{	
 
   VS_OUT vs_out;
   vs_in.Pos.w = 1.0f;
   vs_out.Pos = mul(vs_in.Pos,  g_World);
   vs_out.Pos = mul(vs_out.Pos, g_View);
   vs_out.Pos = mul(vs_out.Pos, g_Proj);

   vs_out.Color = vs_in.Color;
 
   return vs_out;
}


float4 PS(VS_OUT ps_in) : SV_TARGET
{
   float4 pixelColor =  ps_in.Color;

   return pixelColor;
}

technique11 ModelTech
{
    pass P0
    {
        SetVertexShader( CompileShader( vs_5_0, VS() ) );
		SetGeometryShader( NULL );
        SetPixelShader( CompileShader( ps_5_0, PS() ) );
    }
}
