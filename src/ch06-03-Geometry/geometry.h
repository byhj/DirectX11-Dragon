#ifndef CUBE_H
#define CUBE_H

#include <vector>

#include <d3d11.h>
#include <xnamath.h>

#include "d3d/d3dDebug.h"
#include "d3d/d3dShader.h"
#include "d3d/Geometry.h"
#include "d3d/Utility.h"

namespace byhj
{


class Geometry
{
public:
   Geometry()
   {
      m_pVertexBuffer = NULL;
	  m_pIndexBuffer  = NULL;
	  m_pMVPBuffer    = NULL;
	  m_pInputLayout  = NULL;
	  m_VertexCount   = 0;
	  m_IndexCount    = 0;
   }
   ~Geometry() {}

   void Init(ID3D11Device *pD3D11Device, ID3D11DeviceContext *pD3D11DeviceContext, HWND hWnd);
   void Render(ID3D11DeviceContext *pD3D11DeviceContext, const d3d::MatrixBuffer &matrix)
   {
	   cbMatrix.model = matrix.model;	
	   cbMatrix.view  = matrix.view;	
	   cbMatrix.proj  = matrix.proj;

	   // Set vertex buffer stride and offset
	   unsigned int stride;
	   unsigned int offset;
	   stride = sizeof(Vertex); 
	   offset = 0;
	   pD3D11DeviceContext->IASetVertexBuffers(0, 1, &m_pVertexBuffer, &stride, &offset);
	   pD3D11DeviceContext->IASetIndexBuffer(m_pIndexBuffer, DXGI_FORMAT_R32_UINT, 0);

	   CubeShader.use(pD3D11DeviceContext);

	   // Draw the grid.
	   pD3D11DeviceContext->UpdateSubresource(m_pMVPBuffer, 0, NULL, &cbMatrix, 0, 0 );
	   pD3D11DeviceContext->VSSetConstantBuffers( 0, 1, &m_pMVPBuffer);

	   pD3D11DeviceContext->DrawIndexed(mGridIndexCount, mGridIndexOffset, mGridVertexOffset);

	   // Draw the box.
	   XMMATRIX world ;
	   world = XMLoadFloat4x4(&mBoxWorld);
	   XMStoreFloat4x4(&cbMatrix.model, XMMatrixTranspose(world) );	
	   pD3D11DeviceContext->UpdateSubresource(m_pMVPBuffer, 0, NULL, &cbMatrix, 0, 0 );
	   pD3D11DeviceContext->VSSetConstantBuffers( 0, 1, &m_pMVPBuffer);
	   pD3D11DeviceContext->DrawIndexed(mBoxIndexCount, mBoxIndexOffset, mBoxVertexOffset);

	   // Draw center sphere.
	   world = XMLoadFloat4x4(&mCenterSphere);
	   XMStoreFloat4x4(&cbMatrix.model, XMMatrixTranspose(world));
	   pD3D11DeviceContext->UpdateSubresource(m_pMVPBuffer, 0, NULL, &cbMatrix, 0, 0 );
	   pD3D11DeviceContext->VSSetConstantBuffers( 0, 1, &m_pMVPBuffer);
	   pD3D11DeviceContext->DrawIndexed(mSphereIndexCount, mSphereIndexOffset, mSphereVertexOffset);

	   // Draw the cylinders.
	   for(int i = 0; i < 10; ++i)
	   {
		   world = XMLoadFloat4x4(&mCylWorld[i]);
		   XMStoreFloat4x4(&cbMatrix.model, XMMatrixTranspose(world));
		   pD3D11DeviceContext->UpdateSubresource(m_pMVPBuffer, 0, NULL, &cbMatrix, 0, 0 );
		   pD3D11DeviceContext->VSSetConstantBuffers( 0, 1, &m_pMVPBuffer);
		   pD3D11DeviceContext->DrawIndexed(mCylinderIndexCount, mCylinderIndexOffset, mCylinderVertexOffset);
	   }

	   // Draw the spheres.
	   for(int i = 0; i < 10; ++i)
	   {
		   world = XMLoadFloat4x4(&mSphereWorld[i]);
		   XMStoreFloat4x4(&cbMatrix.model, XMMatrixTranspose(world));
		   pD3D11DeviceContext->UpdateSubresource(m_pMVPBuffer, 0, NULL, &cbMatrix, 0, 0 );
		   pD3D11DeviceContext->VSSetConstantBuffers( 0, 1, &m_pMVPBuffer);
		   pD3D11DeviceContext->DrawIndexed(mSphereIndexCount, mSphereIndexOffset, mSphereVertexOffset);
	   }

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

private:
	struct  Vertex
	{
		XMFLOAT3 Pos;
		XMFLOAT3 Normal;
	};

	d3d::MatrixBuffer cbMatrix;
	byhj::Shader CubeShader;

	ID3D11Buffer        *m_pVertexBuffer;
	ID3D11Buffer        *m_pIndexBuffer;
	ID3D11Buffer        *m_pMVPBuffer;
	ID3D11InputLayout   *m_pInputLayout;

	std::vector<Vertex>  m_VertexData;
	std::vector<UINT>    m_IndexData;

	int m_VertexCount;
	int m_IndexCount;

	int mBoxVertexOffset;
	int mGridVertexOffset;
	int mSphereVertexOffset;
	int mCylinderVertexOffset;

	UINT mBoxIndexOffset;
	UINT mGridIndexOffset;
	UINT mSphereIndexOffset;
	UINT mCylinderIndexOffset;

	UINT mBoxIndexCount;
	UINT mGridIndexCount;
	UINT mSphereIndexCount;
	UINT mCylinderIndexCount;

	// Define transformations from local spaces to world space.
	XMFLOAT4X4 mSphereWorld[10];
	XMFLOAT4X4 mCylWorld[10];
	XMFLOAT4X4 mBoxWorld;
	XMFLOAT4X4 mGridWorld;
	XMFLOAT4X4 mCenterSphere;
};


}
#endif