#ifndef CUBE_H
#define CUBE_H

#include <vector>
#include <d3d11.h>
#include <xnamath.h>

#include "d3dx11effect.h"
#include "d3d/Geometry.h"
#include "d3d/Wave.h"
#include "d3d/Timer.h"
#include "d3d/Utility.h"
#include "d3d/Light.h"

namespace byhj
{


class Hill 
{
public:
   Hill()  {}
   ~Hill() {}

   void Init(ID3D11Device *pD3D11Device, ID3D11DeviceContext *pD3D11DeviceContext, HWND hWnd);
   void Render(ID3D11DeviceContext *pD3D11DeviceContext, const d3d::MatrixBuffer &matrix, d3d::Timer *timer);
   void Shutdown();


    void init_buffer (ID3D11Device *pD3D11Device, ID3D11DeviceContext *pD3D11DeviceContext);
    void init_shader (ID3D11Device *pD3D11Device, HWND hWnd);
	float GetHeight(float x, float z) const;

private:
	struct  Vertex
	{
		XMFLOAT3 Pos;
		XMFLOAT4 Color;
	};

	d3d::MatrixBuffer cbMatrix;

	ID3DX11Effect               *m_pEffect          = nullptr;
	ID3DX11EffectTechnique      *m_pEffectTechnique = nullptr;
	ID3DX11EffectMatrixVariable *m_pFxWorld           = nullptr;
	ID3DX11EffectMatrixVariable *m_pFxView            = nullptr;
	ID3DX11EffectMatrixVariable *m_pFxProj            = nullptr;
	ID3DX11EffectVariable       *m_pFxDirLight;
	ID3DX11EffectVariable       *m_pFxPointLight;
	ID3DX11EffectVariable       *m_pFxSpotLight;
	ID3DX11EffectVariable       *m_pFxMaterial;

	ID3D11Buffer        *m_pLandVB      = nullptr;
	ID3D11Buffer        *m_pLandIB      = nullptr;
	ID3D11Buffer        *m_pWaveVB      = nullptr;
	ID3D11Buffer        *m_pWaveIB      = nullptr;
	ID3D11InputLayout   *m_pInputLayout = nullptr;

	d3d::DirectionLight m_DirLight;
	d3d::PointLight     m_PointLight;
	d3d::SpotLight      m_SpotLight;
	d3d::Material       m_LandMat;
	d3d::Material       m_WavesMat;

	std::vector<Vertex>  m_VertexData;
	std::vector<UINT>    m_IndexData;

	int m_VertexCount;
	int m_IndexCount;

	d3d::Geometry m_Geometry;
	d3d::Wave     m_Wave;
};

}
#endif