#ifndef CUBE_H
#define CUBE_H

#include <vector>

#include <d3d11.h>
#include <xnamath.h>
#include "d3d/d3dDebug.h"
#include "d3d/d3dShader.h"
#include "d3d/Geometry.h"
#include "d3d/d3dLight.h"
#include "d3d/Camera.h"
#include "d3d/Utility.h"

namespace byhj
{

class Cube
{
public:
   Cube()
   {
	  m_pMVPBuffer    = NULL;
	  m_pInputLayout  = NULL;
	  m_VertexCount   = 0;
	  m_IndexCount    = 0;
   }
   ~Cube() {}
   void Init(ID3D11Device *pD3D11Device, ID3D11DeviceContext *pD3D11DeviceContext, HWND hWnd);
   void Render(ID3D11DeviceContext *pD3D11DeviceContext, const d3d::MatrixBuffer &matrix, D3DCamera *camera);

   void shutdown()
   {
	   ReleaseCOM(m_pMVPBuffer         )
	   ReleaseCOM(m_pInputLayout       )
   }	

    void init_buffer (ID3D11Device *pD3D11Device, ID3D11DeviceContext *pD3D11DeviceContext);
    void init_shader (ID3D11Device *pD3D11Device, HWND hWnd);
	void init_light();
	void init_texture(ID3D11Device *pD3D11Device);

private:
	struct  Vertex
	{
		XMFLOAT3 Pos;
		XMFLOAT3 Normal;
		XMFLOAT2 Tex;
	};
	d3d::MatrixBuffer cbMatrix;

	struct LightBuffer
	{
		DirectionLight g_DirLights[2];
		XMFLOAT3       g_EyePos;
		float          pad;
	};
	LightBuffer cbLight;

	Material cbMaterial;

	byhj::Shader CubeShader;

	ID3D11Buffer             *m_pMVPBuffer;
	ID3D11InputLayout        *m_pInputLayout;
	ID3D11Buffer             *m_pCubeVB;
	ID3D11Buffer             *m_pCubeIB;     
	ID3D11Buffer             *m_pLightBuffer;
	ID3D11Buffer             *m_pMaterialBuffer;
	ID3D11ShaderResourceView *m_pDiffuseTexSRV;

	DirectionLight m_DirLights[2];

	Material m_CubeMat;

	int m_VertexCount;
	int m_IndexCount;
};


}


#endif