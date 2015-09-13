#include "cube.h"
#include <D3DX11.h>
#include "d3d/Geometry.h"

namespace byhj
{

	Cube::Cube(){}

	Cube::~Cube(){}


	void Cube::Init(ID3D11Device *pD3D11Device, ID3D11DeviceContext *pD3D11DeviceContext, HWND hWnd)
	{
		init_buffer(pD3D11Device, pD3D11DeviceContext);
		init_shader(pD3D11Device, hWnd);
		init_texture(pD3D11Device);
	}

	void Cube::Render(ID3D11DeviceContext *pD3D11DeviceContext, d3d::MatrixBuffer matrix)
	{

		// Set vertex buffer stride and offset
		unsigned int stride;
		unsigned int offset;
		stride = sizeof( Vertex );
		offset = 0;
		pD3D11DeviceContext->IASetVertexBuffers(0, 1, &m_pCubeVB, &stride, &offset);
		pD3D11DeviceContext->IASetIndexBuffer(m_pCubeIB, DXGI_FORMAT_R32_UINT, 0);
		pD3D11DeviceContext->IASetInputLayout(m_pInputLayout);
		pD3D11DeviceContext->IASetPrimitiveTopology(D3D11_PRIMITIVE_TOPOLOGY_TRIANGLELIST);

		cbMatrix.model = matrix.model;
		cbMatrix.view = matrix.view;
		cbMatrix.proj = matrix.proj;


		D3DX11_TECHNIQUE_DESC techDesc;
		m_pEffectTechnique->GetDesc(&techDesc);
		for ( UINT p = 0; p<techDesc.Passes; ++p )
		{
			m_pFxDiffuseMap->SetResource(m_pDiffuseTexSRV);
			m_pFxWorld->SetMatrix(reinterpret_cast< float* >( &cbMatrix.model ));
			m_pFxView->SetMatrix(reinterpret_cast< float* >( &cbMatrix.view ));
			m_pFxProj->SetMatrix(reinterpret_cast< float* >( &cbMatrix.proj ));
			m_pEffectTechnique->GetPassByIndex(p)->Apply(0, pD3D11DeviceContext);
			pD3D11DeviceContext->DrawIndexed(m_IndexCount, 0, 0);
		}

	}

	void Cube::Shutdown()
	{
		ReleaseCOM(m_pCubeVB)
		ReleaseCOM(m_pCubeIB)
		ReleaseCOM(m_pInputLayout)
	}

	void Cube::init_light()
	{

	}


	void Cube::init_buffer(ID3D11Device *pD3D11Device, ID3D11DeviceContext *pD3D11DeviceContext)
	{

		init_light();

		HRESULT hr;
		d3d::Geometry::MeshData box;
		d3d::Geometry geoGen;
		geoGen.CreateCube(1.0f, 1.0f, 1.0f, box);

		m_IndexCount  = box.IndexData.size();
		m_VertexCount = box.VertexData.size();
		std::vector<Vertex> VertexData(m_VertexCount);

		UINT k = 0;
		for ( size_t i = 0; i<box.VertexData.size(); ++i, ++k )
		{
			VertexData[k].Pos    = box.VertexData[i].Pos;
			VertexData[k].Normal = box.VertexData[i].Normal;
			VertexData[k].Tex    = box.VertexData[i].Tex;
		}

		D3D11_BUFFER_DESC cubeVBDesc;
		cubeVBDesc.Usage               = D3D11_USAGE_IMMUTABLE;
		cubeVBDesc.ByteWidth           = sizeof( Vertex )* m_VertexCount;
		cubeVBDesc.BindFlags           = D3D11_BIND_VERTEX_BUFFER;
		cubeVBDesc.CPUAccessFlags      = 0;
		cubeVBDesc.MiscFlags           = 0;
		cubeVBDesc.StructureByteStride = 0;

		D3D11_SUBRESOURCE_DATA shapeVBO;
		shapeVBO.pSysMem = &VertexData[0];
		hr = pD3D11Device->CreateBuffer(&cubeVBDesc, &shapeVBO, &m_pCubeVB);
		DebugHR(hr);


		D3D11_BUFFER_DESC cubeIBDesc;
		cubeIBDesc.Usage               = D3D11_USAGE_IMMUTABLE;
		cubeIBDesc.ByteWidth           = sizeof( UINT )* m_IndexCount;
		cubeIBDesc.BindFlags           = D3D11_BIND_INDEX_BUFFER;
		cubeIBDesc.CPUAccessFlags      = 0;
		cubeIBDesc.MiscFlags           = 0;
		cubeIBDesc.StructureByteStride = 0;

		D3D11_SUBRESOURCE_DATA shapeIBO;
		shapeIBO.pSysMem = &box.IndexData[0];
		hr = pD3D11Device->CreateBuffer(&cubeIBDesc, &shapeIBO, &m_pCubeIB);
		DebugHR(hr);

	}

void Cube::init_texture(ID3D11Device *pD3D11Device)
{
	HRESULT hr;
	hr = D3DX11CreateShaderResourceViewFromFile(pD3D11Device, L"../../media/textures/WoodCrate01.dds", 0, 0, &m_pDiffuseTexSRV, 0 );
	DebugHR(hr);
}

void Cube::init_shader(ID3D11Device *pD3D11Device, HWND hWnd)
{
	DWORD shaderFlags = 0;
#if defined( DEBUG ) || defined( _DEBUG )
	shaderFlags |= D3D10_SHADER_DEBUG;
	shaderFlags |= D3D10_SHADER_SKIP_OPTIMIZATION;
#endif

	ID3D10Blob* compiledShader = 0;
	ID3D10Blob* compilationMsgs = 0;
	HRESULT hr = D3DX11CompileFromFile(L"texture.fx", 0, 0, 0, "fx_5_0", shaderFlags,
		0, 0, &compiledShader, &compilationMsgs, 0);

	// compilationMsgs can store errors or warnings.
	if ( compilationMsgs!=0 )
	{
		MessageBoxA(0, ( char* )compilationMsgs->GetBufferPointer(), 0, 0);
		ReleaseCOM(compilationMsgs);
	}

	// Even if there are no compilationMsgs, check to make sure there were no other errors.
	if ( FAILED(hr) )
	{
		DXTrace(__FILE__, ( DWORD )__LINE__, hr, L"D3DX11CompileFromFile", true);
	}

	D3DX11CreateEffectFromMemory(compiledShader->GetBufferPointer(), compiledShader->GetBufferSize(),
		0, pD3D11Device, &m_pEffect);

	m_pEffectTechnique = m_pEffect->GetTechniqueByName("TextureTech");


	m_pFxWorld = m_pEffect->GetVariableByName("g_World")->AsMatrix();
	m_pFxView  = m_pEffect->GetVariableByName("g_View")->AsMatrix();
	m_pFxProj  = m_pEffect->GetVariableByName("g_Proj")->AsMatrix();
	m_pFxDiffuseMap = m_pEffect->GetVariableByName("g_DiffuseMap")->AsShaderResource();

	// Done with compiled shader.
	ReleaseCOM(compiledShader);

	D3D11_INPUT_ELEMENT_DESC vertexDesc[] =
	{
		{"POSITION", 0, DXGI_FORMAT_R32G32B32_FLOAT, 0, 0, D3D11_INPUT_PER_VERTEX_DATA, 0},
		{"NORMAL",   0, DXGI_FORMAT_R32G32B32_FLOAT, 0, 12, D3D11_INPUT_PER_VERTEX_DATA, 0},
		{"TEXCOORD", 0, DXGI_FORMAT_R32G32_FLOAT, 0, 24, D3D11_INPUT_PER_VERTEX_DATA, 0}
	};

	D3DX11_PASS_DESC passDesc;
	m_pEffectTechnique->GetPassByIndex(0)->GetDesc(&passDesc);
	pD3D11Device->CreateInputLayout(vertexDesc, 3, passDesc.pIAInputSignature,
		                            passDesc.IAInputSignatureSize, &m_pInputLayout);

}


}