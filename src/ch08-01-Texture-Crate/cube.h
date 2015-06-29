#ifndef CUBE_H
#define CUBE_H

#include <vector>

#include <d3d11.h>
#include <xnamath.h>
#include <D3DX11.h>

#include "d3d/d3dDebug.h"
#include "d3d/d3dShader.h"
#include "d3d/d3dGeometry.h"
#include "d3d/d3dLight.h"
#include "d3d/d3dCamera.h"

struct  Vertex
{
	XMFLOAT3 Pos;
	XMFLOAT3 Normal;
	XMFLOAT2 Tex;
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
   }
   ~Geometry() {}

   void Render(ID3D11DeviceContext *pD3D11DeviceContext, XMMATRIX &model,  
	           XMMATRIX &view, XMMATRIX &proj, D3DCamera *camera)
   {

	   // Set vertex buffer stride and offset
	   unsigned int stride;
	   unsigned int offset;
	   stride = sizeof(Vertex); 
	   offset = 0;
	   pD3D11DeviceContext->IASetVertexBuffers(0, 1, &m_pCubeVB, &stride, &offset);
	   pD3D11DeviceContext->IASetIndexBuffer(m_pCubeIB, DXGI_FORMAT_R32_UINT, 0);

	   /////Matrix
	   cbMatrix.model = XMMatrixTranspose(model);	
	   cbMatrix.view  = XMMatrixTranspose(view);	
	   cbMatrix.proj  = XMMatrixTranspose(proj);
	   pD3D11DeviceContext->UpdateSubresource(m_pMVPBuffer, 0, NULL, &cbMatrix, 0, 0 );
	   pD3D11DeviceContext->VSSetConstantBuffers( 0, 1, &m_pMVPBuffer);

	   ////Light
	   cbLight.g_DirLights[0]   = m_DirLights[0];
	   cbLight.g_DirLights[1]   = m_DirLights[1];
	   cbLight.g_EyePos     = camera->GetPos();
	   pD3D11DeviceContext->UpdateSubresource(m_pLightBuffer, 0, NULL, &cbLight, 0, 0 );
	   pD3D11DeviceContext->PSSetConstantBuffers( 0, 1, &m_pLightBuffer);

	   CubeShader.use(pD3D11DeviceContext);

	   //Material
	   cbMaterial = m_CubeMat;
	   pD3D11DeviceContext->UpdateSubresource(m_pMaterialBuffer, 0, NULL, &cbMaterial, 0, 0 );
	   pD3D11DeviceContext->PSSetConstantBuffers(1, 1, &m_pMaterialBuffer);
	   pD3D11DeviceContext->PSSetShaderResources(0, 1, &m_pDiffuseTexSRV);
	   pD3D11DeviceContext->DrawIndexed(m_IndexCount, 0, 0);
   }

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
	struct MatrixBuffer
	{
		XMMATRIX  model;
		XMMATRIX  view;
		XMMATRIX  proj;
	};
	MatrixBuffer cbMatrix;

	struct LightBuffer
	{
		DirectionLight g_DirLights[2];
		XMFLOAT3       g_EyePos;
		float          pad;
	};
	LightBuffer cbLight;

	Material cbMaterial;

	D3DShader CubeShader;

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

#endif