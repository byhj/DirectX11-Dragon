#ifndef HILL_H
#define HILL_H

#include <vector>

#include <d3d11.h>
#include <xnamath.h>

#include "d3d/d3dDebug.h"
#include "d3d/d3dShader.h"
#include "d3d/d3dGeometry.h"
#include "d3d/d3dUtility.h"

namespace byhj
{

class Hill 
{
public:
   Hill()
   {
      m_pVertexBuffer = NULL;
	  m_pIndexBuffer  = NULL;
	  m_pMVPBuffer    = NULL;
	  m_pInputLayout  = NULL;
	  m_VertexCount   = 0;
	  m_IndexCount    = 0;
   }
   ~Hill() {}

   void Render(ID3D11DeviceContext *pD3D11DeviceContext, const byhj::MatrixBuffer &matrix);
   void Shutdown();


    void Init(ID3D11Device *pD3D11Device, ID3D11DeviceContext *pD3D11DeviceContext, HWND hWnd);
    void init_buffer (ID3D11Device *pD3D11Device, ID3D11DeviceContext *pD3D11DeviceContext);
    void init_shader (ID3D11Device *pD3D11Device, HWND hWnd);
	float GetHeight(float x, float z) const;

private:
	struct  Vertex
	{
		XMFLOAT3 Pos;
		XMFLOAT4 Color;
	};

	byhj::MatrixBuffer cbMatrix;

	byhj::Shader CubeShader;
	ID3D11Buffer        *m_pVertexBuffer;
	ID3D11Buffer        *m_pIndexBuffer;
	ID3D11Buffer        *m_pMVPBuffer;
	ID3D11InputLayout   *m_pInputLayout;

	std::vector<Vertex>  m_VertexData;
	std::vector<UINT>    m_IndexData;

	int m_VertexCount;
	int m_IndexCount;

	D3DGeometry geometry;
};

}

#endif