#ifndef CUBE_H
#define CUBE_H

#include <d3d11.h>

#include <DirectXMath.h>
#include "d3dx11effect.h"
#include "d3d/d3dDebug.h"
#include "d3d/Utility.h"

using namespace DirectX;

namespace byhj
{


class Cube 
{
public:
	Cube();
	~Cube();

   void Init(ID3D11Device *pD3D11Device, ID3D11DeviceContext *pD3D11DeviceContext, HWND hWnd);
   void Render(ID3D11DeviceContext *pD3D11DeviceContext, d3d::MatrixBuffer matrix);
   void Shutdown();

private:
	void init_buffer(ID3D11Device *pD3D11Device, ID3D11DeviceContext *pD3D11DeviceContext);
	void init_shader(ID3D11Device *pD3D11Device, HWND hWnd);

	d3d::MatrixBuffer cbMatrix;

	struct  Vertex
	{
		XMFLOAT3 Pos;
		XMFLOAT4 Color;
	};

	ID3DX11Effect               *m_pEffect          = nullptr;
	ID3DX11EffectTechnique      *m_pEffectTechnique = nullptr;
	ID3DX11EffectMatrixVariable *m_pWorld           = nullptr;
	ID3DX11EffectMatrixVariable *m_pView            = nullptr;
	ID3DX11EffectMatrixVariable *m_pProj            = nullptr;

	ID3D11Buffer                *m_pVertexBuffer = nullptr;
	ID3D11Buffer                *m_pIndexBuffer  = nullptr;
	ID3D11InputLayout           *m_pInputLayout  = nullptr;

	int m_VertexCount = 0;
	int m_IndexCount  = 0;
};

}
#endif