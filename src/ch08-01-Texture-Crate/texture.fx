
cbuffer MatrixBuffer
{
   float4x4 g_World;
   float4x4 g_View;
   float4x4 g_Proj;
};

Texture2D g_DiffuseMap;

SamplerState samAnisotropic
{
	Filter = ANISOTROPIC;
	MaxAnisotropy = 4;

	AddressU = WRAP;
	AddressV = WRAP;
};

struct VS_IN
{
  float3 Pos      : POSITION;
  float3 Normal   : NORMAL;
  float2 TexCoord : TEXCOORD; 
};

struct VS_OUT

{
  float4 Pos      : SV_POSITION;
  float3 Normal   : NORMAL;
  float2 TexCoord : TEXCOORD;
};


VS_OUT VS(VS_IN vs_in) 
{
  VS_OUT vs_out;

  vs_out.Pos = mul(float4(vs_in.Pos, 1.0f), g_World);
  vs_out.Pos = mul(vs_out.Pos, g_View);
  vs_out.Pos = mul(vs_out.Pos, g_Proj);

  vs_out.Normal = vs_in.Normal;
  vs_out.TexCoord = vs_in.TexCoord;

  return vs_out;
}

float4 PS(VS_OUT vs_out) : SV_TARGET
{
   vs_out.Normal = normalize(vs_out.Normal);
   float4 textureColor = g_DiffuseMap.Sample(samAnisotropic, vs_out.TexCoord);

   return textureColor;
}
   
technique11 TextureTech
{
  pass p0
  {
     SetVertexShader( CompileShader( vs_5_0, VS() ) );
	 SetGeometryShader( NULL );
     SetPixelShader( CompileShader( ps_5_0, PS() ) );
  }
}

