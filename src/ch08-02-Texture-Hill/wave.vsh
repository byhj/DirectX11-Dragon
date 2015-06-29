
cbuffer MatrixBuffer : register(b0)
{
	float4x4 model;
	float4x4 view;
	float4x4 proj;
	float4x4 texTrans;
};

 struct VS_IN
{
    float4 Pos    : POSITION;
    float3 Normal : NORMAL;
    float2 Tex    : TEXCOORD;
};

struct VS_OUT
{
    float4 Pos      : SV_POSITION;
    float3 Normal   : NORMAL;
	float2 Tex      : TEXCOORD;
	float3 WorldPos : POSITION;
};

VS_OUT VS( VS_IN vs_in)
{	
 
   VS_OUT vs_out;
   vs_in.Pos.w = 1.0f;
   vs_out.Pos = mul(vs_in.Pos,  model);
   vs_out.Pos = mul(vs_out.Pos, view);
   vs_out.Pos = mul(vs_out.Pos, proj);

   vs_out.Tex      = mul(float4(vs_in.Tex, 0.0f, 1.0f), texTrans).xy;
   vs_out.WorldPos = mul(vs_in.Pos, model).xyz;
   vs_out.Normal   = mul(vs_in.Normal, (float3x3)model );
 
   return vs_out;
}
