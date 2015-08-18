#ifndef CUBE_H
#define CUBE_H

#include <d3d11.h>
#include <xnamath.h>

#include "d3d/d3dDebug.h"
#include "d3d/d3dShader.h"

namespace byhj
{


class Cube 
{
public:
	Cube();
	~Cube();

   void Init(ID3D11Device *pD3D11Device, ID3D11DeviceContext *pD3D11DeviceContext, HWND hWnd);
   void Render(ID3D11DeviceContext *pD3D11DeviceContext, XMMATRIX &model,
	   XMMATRIX &view, XMMATRIX &proj);

    void Shutdown();



private:
	void init_buffer(ID3D11Device *pD3D11Device, ID3D11DeviceContext *pD3D11DeviceContext);
	void init_shader(ID3D11Device *pD3D11Device, HWND hWnd);

	struct MatrixBuffer
	{
		XMMATRIX  model;
		XMMATRIX  view;
		XMMATRIX  proj;

	};
	MatrixBuffer cbMatrix;

	struct  Vertex
	{
		XMFLOAT3 Pos;
		XMFLOAT4 Color;
	};

	byhj::Shader CubeShader;
	ID3D11Buffer        *m_pVertexBuffer = nullptr;
	ID3D11Buffer        *m_pIndexBuffer  = nullptr;
	ID3D11Buffer        *m_pMVPBuffer    = nullptr;
	ID3D11InputLayout   *m_pInputLayout  = nullptr;

	int m_VertexCount = 0;
	int m_IndexCount  = 0;
};

}
#endif