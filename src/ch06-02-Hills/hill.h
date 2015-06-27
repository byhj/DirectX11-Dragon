#ifndef CUBE_H
#define CUBE_H

#include <vector>

#include <d3d11.h>
#include <DirectXMath.h>

#include "d3d/d3dDebug.h"
#include "d3d/d3dShader.h"
#include "d3d/d3dGeometry.h"

using namespace DirectX;

struct  Vertex
{
	XMFLOAT3 Pos;
	XMFLOAT4 Color;
};

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

   void Render(ID3D11DeviceContext *pD3D11DeviceContext, XMMATRIX &model,  
	           XMMATRIX &view, XMMATRIX &proj)
   {
	   cbMatrix.model = XMMatrixTranspose(model);	
	   cbMatrix.view  = XMMatrixTranspose(view);	
	   cbMatrix.proj  = XMMatrixTranspose(proj);
	   pD3D11DeviceContext->UpdateSubresource(m_pMVPBuffer, 0, NULL, &cbMatrix, 0, 0 );
	   pD3D11DeviceContext->VSSetConstantBuffers( 0, 1, &m_pMVPBuffer);

	   // Set vertex buffer stride and offset
	   unsigned int stride;
	   unsigned int offset;
	   stride = sizeof(Vertex); 
	   offset = 0;
	   pD3D11DeviceContext->IASetVertexBuffers(0, 1, &m_pVertexBuffer, &stride, &offset);
	   pD3D11DeviceContext->IASetIndexBuffer(m_pIndexBuffer, DXGI_FORMAT_R32_UINT, 0);

	   CubeShader.use(pD3D11DeviceContext);
	   pD3D11DeviceContext->DrawIndexed(m_IndexCount, 0, 0);

   }

   void shutdown()
   {
	   ReleaseCOM(m_pMVPBuffer         )
	   ReleaseCOM(m_pVertexBuffer      )
	   ReleaseCOM(m_pIndexBuffer       )
	   ReleaseCOM(m_pInputLayout       )
   }	

    void init_buffer (ID3D11Device *pD3D11Device, ID3D11DeviceContext *pD3D11DeviceContext);
    void init_shader (ID3D11Device *pD3D11Device, HWND hWnd);
	float GetHeight(float x, float z) const;

private:
	struct MatrixBuffer
	{
		XMMATRIX  model;
		XMMATRIX  view;
		XMMATRIX  proj;
	};
	MatrixBuffer cbMatrix;

	D3DShader CubeShader;
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

#endif