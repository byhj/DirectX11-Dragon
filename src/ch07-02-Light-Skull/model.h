#ifndef MODEL_H
#define MODEL_H

#include <vector>

#include <d3d11.h>
#include <DirectXMath.h> using namespace DirectX;

#include "d3dx11effect.h"
#include "d3d/Geometry.h"
#include "d3d/Light.h"
#include "d3d/Camera.h"
#include "d3d/Utility.h"
#include "EffectHelper.h"

namespace byhj
{

class Model
{
public:
   Model()   {}
   ~Model() {}
   void Init(ID3D11Device *pD3D11Device, ID3D11DeviceContext *pD3D11DeviceContext, HWND hWnd);
   void Render(ID3D11DeviceContext *pD3D11DeviceContext, const d3d::MatrixBuffer &matrix, d3d::Camera *pCamera);

   void Shutdown()
   {
   }	

    void init_buffer (ID3D11Device *pD3D11Device, ID3D11DeviceContext *pD3D11DeviceContext);
    void init_shader (ID3D11Device *pD3D11Device, HWND hWnd);
	void init_light();

private:
	struct  Vertex
	{
		XMFLOAT3 Pos;
		XMFLOAT3 Normal;
	};
	d3d::MatrixBuffer cbMatrix;

	struct LightBuffer
	{
		d3d::DirectionLight g_DirLights[3];
		XMFLOAT4       g_EyePos;
	};
	LightBuffer cbLight;

	ID3D11Buffer        *m_pShapesVB;
	ID3D11Buffer        *m_pShapesIB;     
	ID3D11Buffer        *m_pSkullVB;
	ID3D11Buffer        *m_pSkullIB;

	d3d::DirectionLight m_DirLights[3];
	d3d::Material m_GridMat;
	d3d::Material m_BoxMat;
	d3d::Material m_CylinderMat;
	d3d::Material m_SphereMat;
	d3d::Material m_SkullMat;

	EffectHelper m_EffectHelper;

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
	UINT mLightCount = 0;
};

}
#endif