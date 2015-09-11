#include "lightHelper.fx"

cbuffer MatrixBuffer : register(b0)
{
	float4x4 g_World;
	float4x4 g_View;
	float4x4 g_Proj;
};

 struct VS_IN
{
    float4 Pos   : POSITION;
    float3 Normal : NORMAL;
};

struct VS_OUT
{
    float4 Pos   : SV_POSITION;
	float3 WorldPos : POSITION;
    float3 Normal   : NORMAL;
};

VS_OUT VS( VS_IN vs_in)
{	
 
   VS_OUT vs_out;
   vs_in.Pos.w = 1.0f;
   vs_out.Pos = mul(vs_in.Pos,  g_World);
   vs_out.Pos = mul(vs_out.Pos, g_View);
   vs_out.Pos = mul(vs_out.Pos, g_Proj);

   vs_out.WorldPos = mul(vs_in.Pos, g_World).xyz;
   vs_out.Normal   = mul(vs_in.Normal, (float3x3)g_World );

   return vs_out;
}



cbuffer LightBuffer : register(b0)
{
	DirectionLight g_DirLight;
	PointLight     g_PointLight;
	SpotLight      g_SpotLight;
	float3         g_EyePos;
	float          pad;
};


cbuffer MaterialBuffer	: register(b1)
{	
  Material g_Mat;
};


float4 PS(VS_OUT ps_in) : SV_TARGET
{
    ps_in.Normal = normalize(ps_in.Normal);
    float3 eyeDir = normalize(g_EyePos - ps_in.WorldPos);
  
	float4 ambient  = float4(0.0f, 0.0f, 0.0f, 0.0f);
	float4 diffuse  = float4(0.0f, 0.0f, 0.0f, 0.0f);
	float4 specular = float4(0.0f, 0.0f, 0.0f, 0.0f);

	// Sum the light contribution from each light source.
	float4 A, D, S;
	CalcDirectionLight(g_Mat, g_DirLight, ps_in.Normal, eyeDir, A, D, S);
	ambient   += A;  
	diffuse   += D;
	specular  += S;

	CalcPointLight(g_Mat, g_PointLight, ps_in.WorldPos, ps_in.Normal, eyeDir, A, D, S);
	ambient   += A;  
	diffuse   += D;
	specular  += S;

	CalcSpotLight(g_Mat, g_SpotLight, ps_in.WorldPos, ps_in.Normal, eyeDir, A, D, S);
	ambient   += A;  
	diffuse   += D;
	specular  += S;

	float4 lightColor = ambient + diffuse + specular;

	// Common to take alpha from diffuse material.
	lightColor.a = g_Mat.Diffuse.a;

	return  lightColor;
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
