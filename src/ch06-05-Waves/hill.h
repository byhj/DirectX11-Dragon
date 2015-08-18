#ifndef CUBE_H
#define CUBE_H

#include <vector>

#include <d3d11.h>

#include "d3d/d3dDebug.h"
#include "d3d/d3dShader.h"
#include "d3d/d3dGeometry.h"
#include "d3d/d3dWave.h"
#include "d3d/d3dTimer.h"
#include "d3d/d3dUtility.h"
#include "d3d/d3dUtil.h"

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
   void Render(ID3D11DeviceContext *pD3D11DeviceContext, const byhj::MatrixBuffer &matrix, D3DTimer *timer)
   {

	   // Every quarter second, generate a random wave.

	   static float t_base = 0.0f;
	   if( (timer->GetTotalTime() - t_base) >= 0.25f )
	   {
		   t_base += 0.25f;

		   DWORD i = 5 + rand() % 190;
		   DWORD j = 5 + rand() % 190;

		   float r = MathHelper::RandF(1.0f, 2.0f);
		   wave.Disturb(i, j, r);
	   }

	   wave.Update(timer->GetDeltaTime());

	   // Update the wave vertex buffer with the new solution.

	   D3D11_MAPPED_SUBRESOURCE mappedData;
	   pD3D11DeviceContext->Map(m_pWaveVB, 0, D3D11_MAP_WRITE_DISCARD, 0, &mappedData);

	   Vertex* v = reinterpret_cast<Vertex*>(mappedData.pData);
	   for(UINT i = 0; i < wave.VertexCount(); ++i)
	   {
		   v[i].Pos = wave[i];
		   v[i].Color = XMFLOAT4(0.0f, 0.0f, 0.5f, 1.0f);
	   }

	   pD3D11DeviceContext->Unmap(m_pWaveVB, 0);

	   cbMatrix.model = matrix.model;	
	   cbMatrix.view  = matrix.view;	
	   cbMatrix.proj  = matrix.proj;
	   pD3D11DeviceContext->UpdateSubresource(m_pMVPBuffer, 0, NULL, &cbMatrix, 0, 0 );
	   pD3D11DeviceContext->VSSetConstantBuffers( 0, 1, &m_pMVPBuffer);

	   // Set vertex buffer stride and offset
	   unsigned int stride;
	   unsigned int offset;
	   stride = sizeof(Vertex); 
	   offset = 0;
	   pD3D11DeviceContext->IASetVertexBuffers(0, 1, &m_pLandVB, &stride, &offset);
	   pD3D11DeviceContext->IASetIndexBuffer(m_pLandIB, DXGI_FORMAT_R32_UINT, 0);

	   CubeShader.use(pD3D11DeviceContext);
	   pD3D11DeviceContext->DrawIndexed(m_IndexCount, 0, 0);


	   pD3D11DeviceContext->IASetVertexBuffers(0, 1, &m_pWaveVB, &stride, &offset);
	   pD3D11DeviceContext->IASetIndexBuffer(m_pWaveIB, DXGI_FORMAT_R32_UINT, 0);
	   pD3D11DeviceContext->DrawIndexed(3 * wave.TriangleCount(), 0, 0);
   }

   void shutdown()
   {
	   ReleaseCOM(m_pMVPBuffer   )
	   ReleaseCOM(m_pInputLayout )
   }	

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
	ID3D11Buffer        *m_pLandVB;
	ID3D11Buffer        *m_pLandIB;
	ID3D11Buffer        *m_pWaveVB;
	ID3D11Buffer        *m_pWaveIB;
	ID3D11Buffer        *m_pMVPBuffer;
	ID3D11InputLayout   *m_pInputLayout;

	std::vector<Vertex>  m_VertexData;
	std::vector<UINT>    m_IndexData;

	int m_VertexCount;
	int m_IndexCount;

	D3DGeometry geometry;
	D3DWave     wave;
};

}
#endif