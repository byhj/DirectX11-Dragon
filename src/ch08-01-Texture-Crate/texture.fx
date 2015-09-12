
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
  float3 TexCoord : TEXCOORD; 
};

struct VS_OUT

{
  float4 Pos : SV_POSITION;
  float3 Normal: NORMAL;
  float2 TexCoord: TEXCOORD;
};


VS_OUT VS(VS_IN vs_in) 
{
  VS_OUT vs_out;

  vs_out.Pos = mul( (float4)(vs_in.Pos, 1.0f), g_World);
  vs_out.Normal = vs_in.Normal;
}