#ifndef CUBE_H
#define CUBE_H

#include <vector>

#include <d3d11.h>

#include "d3d/d3dDebug.h"
#include "d3d/d3dShader.h"
#include "d3d/d3dGeometry.h"
#include "d3d/d3dWave.h"
#include "d3d/d3dTimer.h"
#include "d3d/d3dUtil.h"
#include "d3d/d3dLight.h"
#include "d3d/d3dCamera.h"
#include "D3DX11.h"

struct  Vertex
{
	XMFLOAT3 Pos;
	XMFLOAT3 Normal;
	XMFLOAT2 Tex;
};

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

   void Render(ID3D11DeviceContext *pD3D11DeviceContext, XMMATRIX &model,  
	           XMMATRIX &view, XMMATRIX &proj, D3DTimer *timer, D3DCamera *camera)
   {

	   // Every quarter second, generate a random wave.

	   static float t_base = 0.0f;
	   if( (timer->GetTotalTime() - t_base) >= 0.25f )
	   {
		   t_base += 0.25f;

		   DWORD i = 5 + rand() % (wave.RowCount()-10);
		   DWORD j = 5 + rand() % (wave.ColumnCount()-10);

		   float r = MathHelper::RandF(1.0f, 2.0f);
		   wave.Disturb(i, j, r);
	   }
	   wave.Update(timer->GetDeltaTime());

	   D3D11_MAPPED_SUBRESOURCE mappedData;
	   pD3D11DeviceContext->Map(m_pWaveVB, 0, D3D11_MAP_WRITE_DISCARD, 0, &mappedData);

	   Vertex* v = reinterpret_cast<Vertex*>(mappedData.pData);
	   for(UINT i = 0; i < wave.VertexCount(); ++i)
	   {
		   v[i].Pos = wave[i];
		   v[i].Normal = wave.Normal(i);

		   // Derive tex-coords in [0,1] from position.
		   v[i].Tex.x  = 0.5f + wave[i].x / wave.Width();
		   v[i].Tex.y  = 0.5f - wave[i].z / wave.Depth();
	   }
	   pD3D11DeviceContext->Unmap(m_pWaveVB, 0);

/////////////////////////////Update Light and Matrix//////////////////////////////////
	   XMFLOAT3 camPos = camera->GetPos();

	   // Circle light over the land surface.
	   m_PointLight.Position.x = 70.0f*cosf( 0.2f*timer->GetTotalTime() );
	   m_PointLight.Position.z = 70.0f*sinf( 0.2f*timer->GetTotalTime() );
	   m_PointLight.Position.y = MathHelper::Max(GetHillHeight(m_PointLight.Position.x, 
	                                             m_PointLight.Position.z), -3.0f) + 10.0f;
	   m_SpotLight.Position = camPos;
	   XMVECTOR pos = XMVectorSet(camPos.x, camPos.y, camPos.z, 1.0f);
	   XMStoreFloat3(&m_SpotLight.Direction, XMVector3Normalize(-pos));


	   cbLight.g_DirLight   = m_DirLight;
	   cbLight.g_PointLight = m_PointLight;
	   cbLight.g_SpotLight  = m_SpotLight;
	   cbLight.g_EyePos     = camPos;
	   pD3D11DeviceContext->UpdateSubresource(m_pLightBuffer, 0, NULL, &cbLight, 0, 0 );
	   pD3D11DeviceContext->PSSetConstantBuffers( 0, 1, &m_pLightBuffer);

	   cbMatrix.model = XMMatrixTranspose(model);	
	   cbMatrix.view  = XMMatrixTranspose(view);	
	   cbMatrix.proj  = XMMatrixTranspose(proj);

	   XMVECTOR matInvDeter;
	   XMMATRIX modelInv = XMMatrixInverse(&matInvDeter, model);
	   XMMATRIX modelInvTranspose = XMMatrixTranspose(modelInv);
	   
	   // Set vertex buffer stride and offset
	   unsigned int stride;
	   unsigned int offset;
	   stride = sizeof(Vertex); 
	   offset = 0;

	   ////////////////////////// Land  //////////////////////////////

	   XMMATRIX grassTexScale = XMMatrixScaling(1.0f, 1.0f, 1.0f);
	   cbMatrix.texTrans = XMMatrixTranspose(grassTexScale);
	   pD3D11DeviceContext->UpdateSubresource(m_pMVPBuffer, 0, NULL, &cbMatrix, 0, 0 );
	   pD3D11DeviceContext->VSSetConstantBuffers( 0, 1, &m_pMVPBuffer);

	   cbMaterial  = m_LandMat;
	   pD3D11DeviceContext->UpdateSubresource(m_pMaterialBuffer, 0, NULL, &cbMaterial, 0, 0 );
	   pD3D11DeviceContext->PSSetConstantBuffers(1, 1, &m_pMaterialBuffer);
	   pD3D11DeviceContext->IASetVertexBuffers(0, 1, &m_pLandVB, &stride, &offset);
	   pD3D11DeviceContext->IASetIndexBuffer(m_pLandIB, DXGI_FORMAT_R32_UINT, 0);
	   pD3D11DeviceContext->PSSetShaderResources(0, 1, &m_pHillTexSRV);
	   CubeShader.use(pD3D11DeviceContext);
	   pD3D11DeviceContext->DrawIndexed(m_IndexCount, 0, 0);


	   ////////////////////////// Wave //////////////////////////////
	   // Animate water texture coordinates.
	   // Tile water texture.
	   static XMFLOAT2 m_WaterTexOffset = XMFLOAT2(0.0f, 0.0f);
	   // Translate texture over time.
	   m_WaterTexOffset.y += 0.05f * timer->GetDeltaTime();
	   m_WaterTexOffset.x += 0.1f * timer->GetDeltaTime();	
	   XMMATRIX wavesOffset = XMMatrixTranslation(sinf(m_WaterTexOffset.x), sinf(m_WaterTexOffset.y), 0.0f);
	   cbMatrix.texTrans = XMMatrixTranspose(wavesOffset);
	   pD3D11DeviceContext->UpdateSubresource(m_pMVPBuffer, 0, NULL, &cbMatrix, 0, 0 );
	   pD3D11DeviceContext->VSSetConstantBuffers( 0, 1, &m_pMVPBuffer);

	   cbMaterial = m_WavesMat;
	   pD3D11DeviceContext->UpdateSubresource(m_pMaterialBuffer, 0, NULL, &cbMaterial, 0, 0 );
	   pD3D11DeviceContext->PSSetConstantBuffers( 1, 1, &m_pMaterialBuffer);
	   pD3D11DeviceContext->IASetVertexBuffers(0, 1, &m_pWaveVB, &stride, &offset);
	   pD3D11DeviceContext->IASetIndexBuffer(m_pWaveIB, DXGI_FORMAT_R32_UINT, 0);
	   pD3D11DeviceContext->PSSetShaderResources(0, 1, &m_pWaveTexSRV);
	   CubeShader.use(pD3D11DeviceContext);
	   pD3D11DeviceContext->DrawIndexed(3 * wave.TriangleCount(), 0, 0);
   }

   void shutdown()
   {
	   ReleaseCOM(m_pMVPBuffer   )
	   ReleaseCOM(m_pInputLayout )
   }	

    void init_buffer (ID3D11Device *pD3D11Device, ID3D11DeviceContext *pD3D11DeviceContext);
    void init_shader (ID3D11Device *pD3D11Device, HWND hWnd);
	void init_light();
	void init_texture(ID3D11Device *pD3D11Device);

	XMFLOAT3 GetHillNormal(float x, float z) const;
	float    GetHillHeight(float x, float z) const;

private:
	struct MatrixBuffer
	{
		XMMATRIX  model;
		XMMATRIX  view;
		XMMATRIX  proj;
		XMMATRIX  texTrans;
	};
	MatrixBuffer cbMatrix;

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

	D3DShader CubeShader;
	ID3D11Buffer        *m_pLandVB;
	ID3D11Buffer        *m_pLandIB;
	ID3D11Buffer        *m_pWaveVB;
	ID3D11Buffer        *m_pWaveIB;
	ID3D11Buffer        *m_pMVPBuffer;
	ID3D11Buffer        *m_pLightBuffer;
	ID3D11Buffer        *m_pMaterialBuffer;
	ID3D11InputLayout   *m_pInputLayout;

	ID3D11ShaderResourceView *m_pHillTexSRV;
	ID3D11ShaderResourceView *m_pWaveTexSRV;

	XMFLOAT4X4 m_GrassTexTransform;
	XMFLOAT4X4 m_WaterTexTransform;

	std::vector<Vertex>  m_VertexData;
	std::vector<UINT>    m_IndexData;

	int m_VertexCount;
	int m_IndexCount;

	D3DGeometry geometry;
	D3DWave     wave;

	//Light and Material
	DirectionLight m_DirLight;
	PointLight     m_PointLight;
	SpotLight      m_SpotLight;
	Material       m_LandMat;
	Material       m_WavesMat;
};

#endif