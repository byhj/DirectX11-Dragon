#include "lightHelper.fx"

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
cbuffer LightBuffer : register(b0)
{
	DirectionLight g_DirLights[2];
	float4         g_EyePos;
};


cbuffer MaterialBuffer	: register(b1)
{	
  Material g_Mat;
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
  float3 WorldPos : POSITION;
};


VS_OUT VS(VS_IN vs_in) 
{
  VS_OUT vs_out;

  vs_out.Pos = mul(float4(vs_in.Pos, 1.0f), g_World);
  vs_out.Pos = mul(vs_out.Pos, g_View);
  vs_out.Pos = mul(vs_out.Pos, g_Proj);

  vs_out.Normal = vs_in.Normal;
  vs_out.TexCoord = vs_in.TexCoord;
  vs_out.WorldPos = mul(float4(vs_in.Pos, 1.0f), g_World).xyz;

  return vs_out;
}

float4 PS(VS_OUT vs_out, uniform int g_LightCount, uniform bool g_UseTexture) : SV_TARGET
{
   vs_out.Normal = normalize(vs_out.Normal);
   float4 texColor = float4(1.0f, 1.0f, 1.0f, 1.0f);
    if(g_UseTexture)
	{
		texColor =  g_DiffuseMap.Sample(samAnisotropic, vs_out.TexCoord);
	}

	// Lighting.
	float4 lightColor = texColor;
	if( g_LightCount > 0  )
	{  
 
         float3 eyeDir = normalize( float3(0.0f, 0.0f, 0.0f) - vs_out.WorldPos);
        
	     float4 ambient  = float4(0.0f, 0.0f, 0.0f, 0.0f);
	     float4 diffuse  = float4(0.0f, 0.0f, 0.0f, 0.0f);
	     float4 specular = float4(0.0f, 0.0f, 0.0f, 0.0f);

	// Sum the light contribution from each light source.
	    for (int i = 0; i != g_LightCount; ++i)
	    {
	       float4 A, D, S;
	       CalcDirectionLight(g_Mat, g_DirLights[i], vs_out.Normal, eyeDir, A, D, S);
	       ambient   += A;  
	       diffuse   += D;
	       specular  += S;
	    }

	    float4 lightColor = texColor * (ambient + diffuse) + specular;
	}

	// Common to take alpha from diffuse material and texture.
	lightColor.a = g_Mat.Diffuse.a * texColor.a;

    return lightColor;
}
   

technique11 Light1
{
    pass P0
    {
        SetVertexShader( CompileShader( vs_5_0, VS() ) );
		SetGeometryShader( NULL );
        SetPixelShader( CompileShader( ps_5_0, PS(1, false) ) );
    }
}

technique11 Light2
{
    pass P0
    {
        SetVertexShader( CompileShader( vs_5_0, VS() ) );
		SetGeometryShader( NULL );
        SetPixelShader( CompileShader( ps_5_0, PS(2, false) ) );
    }
}

technique11 Light3
{
    pass P0
    {
        SetVertexShader( CompileShader( vs_5_0, VS() ) );
		SetGeometryShader( NULL );
        SetPixelShader( CompileShader( ps_5_0, PS(3, false) ) );
    }
}

technique11 Light0Tex
{
    pass P0
    {
        SetVertexShader( CompileShader( vs_5_0, VS() ) );
		SetGeometryShader( NULL );
        SetPixelShader( CompileShader( ps_5_0, PS(0, true) ) );
    }
}

technique11 Light1Tex
{
    pass P0
    {
        SetVertexShader( CompileShader( vs_5_0, VS() ) );
		SetGeometryShader( NULL );
        SetPixelShader( CompileShader( ps_5_0, PS(1, true) ) );
    }
}

technique11 Light2Tex
{
    pass P0
    {
        SetVertexShader( CompileShader( vs_5_0, VS() ) );
		SetGeometryShader( NULL );
        SetPixelShader( CompileShader( ps_5_0, PS(2, true) ) );
    }
}

technique11 Light3Tex
{
    pass P0
    {
        SetVertexShader( CompileShader( vs_5_0, VS() ) );
		SetGeometryShader( NULL );
        SetPixelShader( CompileShader( ps_5_0, PS(3, true) ) );
    }
}
