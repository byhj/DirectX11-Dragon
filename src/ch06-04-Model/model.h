#ifndef CUBE_H
#define CUBE_H

#include <d3d11.h>
#include <DirectXMath.h> using namespace DirectX;
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
   void Render(ID3D11DeviceContext *pD3D11DeviceContext, const d3d::MatrixBuffer &matrix);
   void Shutdown();

   void init_buffer(ID3D11Device *pD3D11Device, ID3D11DeviceContext *pD3D11DeviceContext);
   void init_shader(ID3D11Device *pD3D11Device, HWND hWnd);

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