#ifndef CUBE_H
#define CUBE_H

#include <d3d11.h>
#include <xnamath.h>

#include "d3dx11effect.h"
#include "d3d/d3dDebug.h"
#include "d3d/Utility.h"
#include "d3d/Light.h"

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
		void init_light();
		void init_texture(ID3D11Device *pD3D11Device);

		d3d::MatrixBuffer cbMatrix;
		struct  Vertex
		{
			XMFLOAT3 Pos;
			XMFLOAT3 Normal;
			XMFLOAT2 Tex;
		};

		ID3DX11Effect               *m_pEffect          = nullptr;
		ID3DX11EffectTechnique      *m_pEffectTechnique = nullptr;
		ID3DX11EffectMatrixVariable *m_pFxWorld           = nullptr;
		ID3DX11EffectMatrixVariable *m_pFxView            = nullptr;
		ID3DX11EffectMatrixVariable *m_pFxProj            = nullptr;
		ID3DX11EffectShaderResourceVariable * m_pFxDiffuseMap = nullptr;

		ID3D11Buffer                *m_pCubeVB        = nullptr;
		ID3D11Buffer                *m_pCubeIB        = nullptr;
		ID3D11InputLayout           *m_pInputLayout   = nullptr;
		ID3D11ShaderResourceView    *m_pDiffuseTexSRV = nullptr;
		int m_VertexCount = 0;
		int m_IndexCount  = 0;
	};

}


#endif