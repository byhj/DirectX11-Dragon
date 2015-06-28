#ifndef D3DLIGHT_H
#define D3DLIGHT_H

#include <xnamath.h>

struct DirectionLight 
{
	DirectionLight() { ZeroMemory(this, sizeof(this)); }

	XMFLOAT4 Ambient;
	XMFLOAT4 Diffuse;
	XMFLOAT4 Specular; 
	XMFLOAT3 Direction;
	FLOAT    padding;
};

struct PointLight
{
	PointLight() { ZeroMemory(this, sizeof(this)); }

	XMFLOAT4 Ambient;
	XMFLOAT4 Diffuse;
	XMFLOAT4 Specular;
	XMFLOAT3 Position;
	FLOAT    Range;
	XMFLOAT3 Att;
	FLOAT    padding;
};

struct SpotLight
{
	SpotLight() { ZeroMemory(this, sizeof(this)); }

	XMFLOAT4 Ambient;
	XMFLOAT4 Diffuse;
	XMFLOAT4 Specular;
	XMFLOAT3 Position;
	FLOAT    Range;
	XMFLOAT3 Direction;
	FLOAT    Spot;
	XMFLOAT3 Att;
	FLOAT    Pad; 
};

struct Material
{
   Material() { ZeroMemory(this, sizeof(this)); }

   XMFLOAT4 Ambient;
   XMFLOAT4 Diffuse;
   XMFLOAT4 Specular; // w = SpecPower
   XMFLOAT4 Reflect;
};

#endif