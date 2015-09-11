#ifndef CUBE_H
#define CUBE_H

#include <vector>

#include <d3d11.h>
#include <xnamath.h>

#include "d3d/Geometry.h"
#include "d3d/Light.h"
#include "d3d/Camera.h"
#include "d3d/Utility.h"

namespace byhj
{

class Geometry
{
public:
   Geometry()   {}
   ~Geometry() {}
   void Init(ID3D11Device *pD3D11Device, ID3D11DeviceContext *pD3D11DeviceContext, HWND hWnd);
   void Render(ID3D11DeviceContext *pD3D11DeviceContext, const d3d::MatrixBuffer &matrix, D3DCamera *camera);

   void shutdown()
   {
	   ReleaseCOM(m_pMVPBuffer         )
	   ReleaseCOM(m_pInputLayout       )
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
		d3d::DirectionLight g_DirLight;
		XMFLOAT3       g_EyePos;
		float          pad;
	};
	LightBuffer cbLight;

	Material cbMaterial;

	ID3D11Buffer        *m_pMVPBuffer;
	ID3D11InputLayout   *m_pInputLayout;
	ID3D11Buffer        *m_pShapesVB;
	ID3D11Buffer        *m_pShapesIB;     
	ID3D11Buffer        *m_pSkullVB;
	ID3D11Buffer        *m_pSkullIB;
	ID3D11Buffer        *m_pLightBuffer;
	ID3D11Buffer        *m_pMaterialBuffer;

	d3d::DirectionLight m_DirLights[3];
	d3d::Material m_GridMat;
	d3d::Material m_BoxMat;
	d3d::Material m_CylinderMat;
	d3d::Material m_SphereMat;
	d3d::Material m_SkullMat;
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

}
#endif