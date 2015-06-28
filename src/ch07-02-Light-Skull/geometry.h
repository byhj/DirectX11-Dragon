#ifndef CUBE_H
#define CUBE_H

#include <vector>

#include <d3d11.h>
#include <xnamath.h>

#include "d3d/d3dDebug.h"
#include "d3d/d3dShader.h"
#include "d3d/d3dGeometry.h"
#include "d3d/d3dLight.h"
#include "d3d/d3dCamera.h"

struct  Vertex
{
	XMFLOAT3 Pos;
	XMFLOAT3 Normal;
};

class Geometry
{
public:
   Geometry()
   {
	  m_pMVPBuffer    = NULL;
	  m_pInputLayout  = NULL;
	  m_VertexCount   = 0;
	  m_IndexCount    = 0;
	  mLightCount     = 0;
   }
   ~Geometry() {}

   void Render(ID3D11DeviceContext *pD3D11DeviceContext, XMMATRIX &model,  
	           XMMATRIX &view, XMMATRIX &proj, D3DCamera *camera)
   {
	   //
	   // Switch the number of lights based on key presses.
	   //
	   if( GetAsyncKeyState('0') & 0x8000 )
		   mLightCount = 0; 

	   if( GetAsyncKeyState('1') & 0x8000 )
		   mLightCount = 1; 

	   if( GetAsyncKeyState('2') & 0x8000 )
		   mLightCount = 2; 

	   if( GetAsyncKeyState('3') & 0x8000 )
		   mLightCount = 3; 

	   XMMATRIX world ;
	   world = XMLoadFloat4x4(&mSkullWorld);
	   cbMatrix.model = XMMatrixTranspose(world);	
	   cbMatrix.view  = XMMatrixTranspose(view);	
	   cbMatrix.proj  = XMMatrixTranspose(proj);
	   pD3D11DeviceContext->UpdateSubresource(m_pMVPBuffer, 0, NULL, &cbMatrix, 0, 0 );
	   pD3D11DeviceContext->VSSetConstantBuffers( 0, 1, &m_pMVPBuffer);

	   cbLight.g_DirLight   = m_DirLights[mLightCount];
	   cbLight.g_EyePos     = camera->GetPos();
	   pD3D11DeviceContext->UpdateSubresource(m_pLightBuffer, 0, NULL, &cbLight, 0, 0 );
	   pD3D11DeviceContext->PSSetConstantBuffers( 0, 1, &m_pLightBuffer);

	   CubeShader.use(pD3D11DeviceContext);
	   // Set vertex buffer stride and offset
	   unsigned int stride;
	   unsigned int offset;
	   stride = sizeof(Vertex); 
	   offset = 0;
	   pD3D11DeviceContext->IASetVertexBuffers(0, 1, &m_pSkullVB, &stride, &offset);
	   pD3D11DeviceContext->IASetIndexBuffer(m_pSkullIB, DXGI_FORMAT_R32_UINT, 0);
	   cbMaterial = m_SkullMat;
	   pD3D11DeviceContext->UpdateSubresource(m_pMaterialBuffer, 0, NULL, &cbMaterial, 0, 0 );
	   pD3D11DeviceContext->PSSetConstantBuffers(1, 1, &m_pMaterialBuffer);
	   pD3D11DeviceContext->DrawIndexed(m_IndexCount, 0, 0);


	   //////////////////////////////Draw the Shade//////////////////////////
	   pD3D11DeviceContext->IASetVertexBuffers(0, 1, &m_pShapesVB, &stride, &offset);
	   pD3D11DeviceContext->IASetIndexBuffer(m_pShapesIB, DXGI_FORMAT_R32_UINT, 0);

	   // Draw the grid.
	   world = XMLoadFloat4x4(&mGridWorld);
	   cbMatrix.model = XMMatrixTranspose(world);	
	   pD3D11DeviceContext->UpdateSubresource(m_pMVPBuffer, 0, NULL, &cbMatrix, 0, 0 );
	   pD3D11DeviceContext->VSSetConstantBuffers( 0, 1, &m_pMVPBuffer);
	   cbMaterial = m_GridMat;
	   pD3D11DeviceContext->UpdateSubresource(m_pMaterialBuffer, 0, NULL, &cbMaterial, 0, 0 );
	   pD3D11DeviceContext->DrawIndexed(mGridIndexCount, mGridIndexOffset, mGridVertexOffset);

	   // Draw the box.
	   world = XMLoadFloat4x4(&mBoxWorld);
	   cbMatrix.model = XMMatrixTranspose(world);	
	   pD3D11DeviceContext->UpdateSubresource(m_pMVPBuffer, 0, NULL, &cbMatrix, 0, 0 );
	   pD3D11DeviceContext->VSSetConstantBuffers( 0, 1, &m_pMVPBuffer);
	   cbMaterial = m_BoxMat;
	   pD3D11DeviceContext->UpdateSubresource(m_pMaterialBuffer, 0, NULL, &cbMaterial, 0, 0 );
	   pD3D11DeviceContext->DrawIndexed(mBoxIndexCount, mBoxIndexOffset, mBoxVertexOffset);
	   	 
	   
	   // Draw the cylinders.
	   cbMaterial = m_CylinderMat;
	   pD3D11DeviceContext->UpdateSubresource(m_pMaterialBuffer, 0, NULL, &cbMaterial, 0, 0 );
	   for(int i = 0; i < 10; ++i)
	   {
		   world = XMLoadFloat4x4(&mCylWorld[i]);
		   cbMatrix.model = XMMatrixTranspose(world);	
		   pD3D11DeviceContext->UpdateSubresource(m_pMVPBuffer, 0, NULL, &cbMatrix, 0, 0 );
		   pD3D11DeviceContext->VSSetConstantBuffers( 0, 1, &m_pMVPBuffer);
		   pD3D11DeviceContext->DrawIndexed(mCylinderIndexCount, mCylinderIndexOffset, mCylinderVertexOffset);
	   }

	   // Draw the spheres.
	   cbMaterial = m_SphereMat;
	   pD3D11DeviceContext->UpdateSubresource(m_pMaterialBuffer, 0, NULL, &cbMaterial, 0, 0 );
	   for(int i = 0; i < 10; ++i)
	   {
		   world = XMLoadFloat4x4(&mSphereWorld[i]);
		   cbMatrix.model = XMMatrixTranspose(world);	
		   pD3D11DeviceContext->UpdateSubresource(m_pMVPBuffer, 0, NULL, &cbMatrix, 0, 0 );
		   pD3D11DeviceContext->VSSetConstantBuffers( 0, 1, &m_pMVPBuffer);
		   pD3D11DeviceContext->DrawIndexed(mSphereIndexCount, mSphereIndexOffset, mSphereVertexOffset);
	   }

   }

   void shutdown()
   {
	   ReleaseCOM(m_pMVPBuffer         )
	   ReleaseCOM(m_pInputLayout       )
   }	

    void init_buffer (ID3D11Device *pD3D11Device, ID3D11DeviceContext *pD3D11DeviceContext);
    void init_shader (ID3D11Device *pD3D11Device, HWND hWnd);

private:
	struct MatrixBuffer
	{
		XMMATRIX  model;
		XMMATRIX  view;
		XMMATRIX  proj;
	};
	MatrixBuffer cbMatrix;

	struct LightBuffer
	{
		DirectionLight g_DirLight;
		XMFLOAT3       g_EyePos;
		float          pad;
	};
	LightBuffer cbLight;

	Material cbMaterial;

	D3DShader CubeShader;

	ID3D11Buffer        *m_pMVPBuffer;
	ID3D11InputLayout   *m_pInputLayout;
	ID3D11Buffer        *m_pShapesVB;
	ID3D11Buffer        *m_pShapesIB;     
	ID3D11Buffer        *m_pSkullVB;
	ID3D11Buffer        *m_pSkullIB;
	ID3D11Buffer        *m_pLightBuffer;
	ID3D11Buffer        *m_pMaterialBuffer;

	DirectionLight m_DirLights[3];
	Material m_GridMat;
	Material m_BoxMat;
	Material m_CylinderMat;
	Material m_SphereMat;
	Material m_SkullMat;
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
	XMFLOAT4X4 mSkullWorld;

	UINT mSkullIndexCount;
	UINT mLightCount;
};

#endif