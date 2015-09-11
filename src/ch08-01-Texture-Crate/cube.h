#ifndef CUBE_H
#define CUBE_H

#include <vector>
#include <d3d11.h>
#include <xnamath.h>

#include "d3dx11effect.h"
#include "d3d/Shader.h"
#include "d3d/Geometry.h"
#include "d3d/Light.h"
#include "d3d/Camera.h"
#include "d3d/Utility.h"

namespace byhj
{

class Cube
{
public:
   Cube()  {}
   ~Cube() {}
   void Init(ID3D11Device *pD3D11Device, ID3D11DeviceContext *pD3D11DeviceContext, HWND hWnd);
   void Render(ID3D11DeviceContext *pD3D11DeviceContext, const d3d::MatrixBuffer &matrix, d3d::Camera *camera);

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
		d3d::DirectionLight g_DirLights[2];
		XMFLOAT3       g_EyePos;
		float          pad;
	};
	LightBuffer cbLight;

	d3d::Material 	m_EffectHelper.SetMaterial(m_GridMat); cbMaterial;

	d3d::Shader CubeShader;

	ID3D11Buffer             *m_pMVPBuffer;
	ID3D11InputLayout        *m_pInputLayout;
	ID3D11Buffer             *m_pCubeVB;
	ID3D11Buffer             *m_pCubeIB;     
	ID3D11ShaderResourceView *m_pDiffuseTexSRV;

	d3d::DirectionLight m_DirLights[2];
	d3d::Material m_CubeMat;

	int m_VertexCount;
	int m_IndexCount;
};


}


#endif