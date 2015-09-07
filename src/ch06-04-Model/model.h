#ifndef CUBE_H
#define CUBE_H

#include <d3d11.h>
#include <xnamath.h>
#include <vector>

#include "d3dx11effect.h"
#include "d3d/d3dDebug.h"
#include "d3d/Utility.h"

namespace byhj
{


class Model 
{
public:
   Model() {}
   ~Model() {}
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
	   pD3D11DeviceContext->IASetInputLayout(m_pInputLayout);

	   m_pWorld->SetMatrix(reinterpret_cast< float* >( &cbMatrix.model ));
	   m_pView->SetMatrix(reinterpret_cast< float* >(  &cbMatrix.view ));
	   m_pProj->SetMatrix(reinterpret_cast< float* >(  &cbMatrix.proj ));

	   D3DX11_TECHNIQUE_DESC techDesc;
	   m_pEffectTechnique->GetDesc(&techDesc);
	   for ( UINT p = 0; p<techDesc.Passes; ++p )
	   {
		   m_pEffectTechnique->GetPassByIndex(p)->Apply(0, pD3D11DeviceContext);
		   pD3D11DeviceContext->DrawIndexed(m_IndexCount, 0, 0);
	   }

   }

   void Shutdown()
   {
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
	ID3D11Buffer        *m_pVertexBuffer;
	ID3D11Buffer        *m_pIndexBuffer;
	ID3D11InputLayout   *m_pInputLayout;

	ID3DX11Effect               *m_pEffect          = nullptr;
	ID3DX11EffectTechnique      *m_pEffectTechnique = nullptr;
	ID3DX11EffectMatrixVariable *m_pWorld           = nullptr;
	ID3DX11EffectMatrixVariable *m_pView            = nullptr;
	ID3DX11EffectMatrixVariable *m_pProj            = nullptr;

	std::vector<Vertex> m_VertexData;
	std::vector<UINT> m_IndexData;

	int m_VertexCount;
	int m_IndexCount;
};

}
#endif