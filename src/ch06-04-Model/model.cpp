#include <fstream>
#include <iostream>
#include <string>

#include <d3dcompiler.h>

#include "model.h"


namespace byhj
{

	void Model::Init(ID3D11Device *pD3D11Device, ID3D11DeviceContext *pD3D11DeviceContext, HWND hWnd)
	{
		init_buffer(pD3D11Device, pD3D11DeviceContext);
		init_shader(pD3D11Device, hWnd);
	}

	void Model::Render(ID3D11DeviceContext *pD3D11DeviceContext, const d3d::MatrixBuffer &matrix)
	{
		cbMatrix.model = matrix.model;
		cbMatrix.view = matrix.view;
		cbMatrix.proj = matrix.proj;

		// Set vertex buffer stride and offset
		unsigned int stride;
		unsigned int offset;
		stride = sizeof(Vertex);
		offset = 0;
		pD3D11DeviceContext->IASetVertexBuffers(0, 1, &m_pVertexBuffer, &stride, &offset);
		pD3D11DeviceContext->IASetIndexBuffer(m_pIndexBuffer, DXGI_FORMAT_R32_UINT, 0);
		pD3D11DeviceContext->IASetInputLayout(m_pInputLayout);
		pD3D11DeviceContext->IASetPrimitiveTopology(D3D11_PRIMITIVE_TOPOLOGY_TRIANGLELIST);

		m_pWorld->SetMatrix(reinterpret_cast<float*>(&cbMatrix.model));
		m_pView->SetMatrix(reinterpret_cast<float*>(&cbMatrix.view));
		m_pProj->SetMatrix(reinterpret_cast<float*>(&cbMatrix.proj));

		D3DX11_TECHNIQUE_DESC techDesc;
		m_pEffectTechnique->GetDesc(&techDesc);
		for (UINT p = 0; p < techDesc.Passes; ++p)
		{
			m_pEffectTechnique->GetPassByIndex(p)->Apply(0, pD3D11DeviceContext);
			pD3D11DeviceContext->DrawIndexed(m_IndexCount, 0, 0);
		}

	}

	void Model::Shutdown()
	{
		ReleaseCOM(m_pVertexBuffer)
		ReleaseCOM(m_pIndexBuffer)
		ReleaseCOM(m_pInputLayout)
	}

	void Model::init_buffer(ID3D11Device *pD3D11Device, ID3D11DeviceContext *pD3D11DeviceContext)
	{

		HRESULT hr;

		//Load the model file
		std::ifstream fin("../../media/objects/skull.txt");
		if ( !fin )
		{
			MessageBox(NULL, L"Can not open the model file!", 0, 0);
			return;
		}
		std::string ignore;
		int tCount;

		fin >> ignore >> m_VertexCount;
		fin >> ignore >> tCount;
		fin >> ignore >> ignore>>ignore>>ignore;

		m_VertexData.resize(m_VertexCount);
		for ( UINT i = 0; i!=m_VertexCount; ++i )
		{
			fin>>m_VertexData[i].Pos.x
				>>m_VertexData[i].Pos.y
				>>m_VertexData[i].Pos.z;

			fin>>m_VertexData[i].Normal.x
				>>m_VertexData[i].Normal.y
				>>m_VertexData[i].Normal.z;
		}
		fin>>ignore>>ignore>>ignore;
		m_IndexCount = 3 * tCount;
		m_IndexData.resize(m_IndexCount);
		for (UINT i = 0; i < tCount; ++i)
		{
			fin >> m_IndexData[i * 3 + 0] >> m_IndexData[i * 3 + 1] >> m_IndexData[i * 3 + 2];
		}


		D3D11_BUFFER_DESC vertexBufferDesc;
		vertexBufferDesc.Usage               = D3D11_USAGE_IMMUTABLE;
		vertexBufferDesc.ByteWidth           = sizeof( Vertex )* m_VertexCount;
		vertexBufferDesc.BindFlags           = D3D11_BIND_VERTEX_BUFFER;
		vertexBufferDesc.CPUAccessFlags      = 0;
		vertexBufferDesc.MiscFlags           = 0;
		vertexBufferDesc.StructureByteStride = 0;

		D3D11_SUBRESOURCE_DATA VBO;
		VBO.pSysMem = &m_VertexData[0];
		hr = pD3D11Device->CreateBuffer(&vertexBufferDesc, &VBO, &m_pVertexBuffer);
		//DebugHR(hr);


		/////////////////////////////Index Buffer//////////////////////////////

		D3D11_BUFFER_DESC indexBufferDesc;
		indexBufferDesc.Usage               = D3D11_USAGE_IMMUTABLE;
		indexBufferDesc.ByteWidth           = sizeof( UINT )* m_IndexCount;
		indexBufferDesc.BindFlags           = D3D11_BIND_INDEX_BUFFER;
		indexBufferDesc.CPUAccessFlags      = 0;
		indexBufferDesc.MiscFlags           = 0;
		indexBufferDesc.StructureByteStride = 0;

		D3D11_SUBRESOURCE_DATA IBO;
		IBO.pSysMem = &m_IndexData[0];
		hr = pD3D11Device->CreateBuffer(&indexBufferDesc, &IBO, &m_pIndexBuffer);
		//DebugHR(hr);


	}

	void Model::init_shader(ID3D11Device *pD3D11Device, HWND hWnd)
	{
		DWORD dwShaderFlags = D3DCOMPILE_ENABLE_STRICTNESS;
#ifdef _DEBUG
		// Set the D3DCOMPILE_DEBUG flag to embed debug information in the shaders.
		// Setting this flag improves the shader debugging experience, but still allows 
		// the shaders to be optimized and to run exactly the way they will run in 
		// the release configuration of this program.
		dwShaderFlags |= D3DCOMPILE_DEBUG;

		// Disable optimizations to further improve shader debugging
		dwShaderFlags |= D3DCOMPILE_SKIP_OPTIMIZATION;
#endif

#if D3D_COMPILER_VERSION >= 46

		// Read the D3DX effect file
		HRESULT hr = S_OK;
		D3DX11CompileEffectFromFile(L"model.fx", nullptr, D3D_COMPILE_STANDARD_FILE_INCLUDE, dwShaderFlags, 0, pD3D11Device, &m_pEffect, nullptr);

#else

		ID3DBlob* pEffectBuffer = nullptr;
		V_RETURN(DXUTCompileFromFile(L"Tutorial11.fx", nullptr, "none", "fx_5_0", dwShaderFlags, 0, &pEffectBuffer));
		hr = D3DX11CreateEffectFromMemory(pEffectBuffer->GetBufferPointer(), pEffectBuffer->GetBufferSize(), 0, pd3dDevice, &m_pEffect);
		SAFE_RELEASE(pEffectBuffer);
		if (FAILED(hr))
			return hr;

#endif

		m_pEffectTechnique = m_pEffect->GetTechniqueByName("ModelTech");


		m_pWorld = m_pEffect->GetVariableByName("g_World")->AsMatrix();
		m_pView  = m_pEffect->GetVariableByName("g_View")->AsMatrix();
		m_pProj  = m_pEffect->GetVariableByName("g_Proj")->AsMatrix();

		// Done with compiled shader.


		D3D11_INPUT_ELEMENT_DESC vertexDesc[] =
		{
			{"POSITION", 0, DXGI_FORMAT_R32G32B32_FLOAT, 0, 0, D3D11_INPUT_PER_VERTEX_DATA, 0},
			{"NORMAL", 0, DXGI_FORMAT_R32G32B32_FLOAT, 0, 12, D3D11_INPUT_PER_VERTEX_DATA, 0}
		};

		D3DX11_PASS_DESC passDesc;
		m_pEffectTechnique->GetPassByIndex(0)->GetDesc(&passDesc);

		pD3D11Device->CreateInputLayout(vertexDesc, 2, passDesc.pIAInputSignature,
			passDesc.IAInputSignatureSize, &m_pInputLayout);


	}

}