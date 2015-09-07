#ifndef CUBE_H
#define CUBE_H

#include <vector>

#include <d3d11.h>

#include "d3d/d3dDebug.h"
#include "d3d/d3dShader.h"
#include "d3d/Geometry.h"
#include "d3d/d3dWave.h"
#include "d3d/Timer.h"

#include "d3d/d3dLight.h"
#include "d3d/Camera.h"
#include "d3d/Utility.h"

namespace byhj
{

class Hill 
{
public:
   Hill()
   {
	  m_pMVPBuffer    = NULL;
	  m_pInputLayout  = NULL;
	  m_VertexCount   = 0;
	  m_IndexCount    = 0;
   }
   ~Hill() {}

   void Init(ID3D11Device *pD3D11Device, ID3D11DeviceContext *pD3D11DeviceContext, HWND hWnd);
   void Render(ID3D11DeviceContext *pD3D11DeviceContext, const d3d::MatrixBuffer &matrix, D3DTimer *timer, D3DCamera *camera);

   void shutdown()
   {
	   ReleaseCOM(m_pMVPBuffer   )
	   ReleaseCOM(m_pInputLayout )
   }	

    void init_buffer (ID3D11Device *pD3D11Device, ID3D11DeviceContext *pD3D11DeviceContext);
    void init_shader (ID3D11Device *pD3D11Device, HWND hWnd);
	void init_light();

	XMFLOAT3 GetHillNormal(float x, float z) const;
	float    GetHillHeight(float x, float z) const;

private:
	struct  Vertex
	{
		XMFLOAT3 Pos;
		XMFLOAT3 Normal;
	};

	d3d::MatrixBuffer cbMatrix;

	struct LightBuffer
	{
		DirectionLight g_DirLight;
		PointLight     g_PointLight;
		SpotLight      g_SpotLight;
		XMFLOAT3       g_EyePos;
		float          pad;
	};
	LightBuffer cbLight;

	Material cbMaterial;

	byhj::Shader CubeShader;
	ID3D11Buffer        *m_pLandVB;
	ID3D11Buffer        *m_pLandIB;
	ID3D11Buffer        *m_pWaveVB;
	ID3D11Buffer        *m_pWaveIB;
	ID3D11Buffer        *m_pMVPBuffer;
	ID3D11Buffer        *m_pLightBuffer;
	ID3D11Buffer        *m_pMaterialBuffer;
	ID3D11InputLayout   *m_pInputLayout;

	std::vector<Vertex>  m_VertexData;
	std::vector<UINT>    m_IndexData;

	int m_VertexCount;
	int m_IndexCount;

	d3d::Geometry m_Geometry;
	D3DWave     wave;

	//Light and Material
	DirectionLight m_DirLight;
	PointLight     m_PointLight;
	SpotLight      m_SpotLight;
	Material       m_LandMat;
	Material       m_WavesMat;
};

}
#endif