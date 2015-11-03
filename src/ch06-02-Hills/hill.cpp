#include "Hill.h"
#include <d3dcompiler.h>

namespace byhj
{
void Hill::Init(ID3D11Device *pD3D11Device, ID3D11DeviceContext *pD3D11DeviceContext, HWND hWnd)
{
	init_buffer(pD3D11Device, pD3D11DeviceContext);
	init_shader(pD3D11Device, hWnd);
}


void Hill::Render(ID3D11DeviceContext *pD3D11DeviceContext, d3d::MatrixBuffer matrix)
{

	// Set vertex buffer stride and offset
	unsigned int stride;
	unsigned int offset;
	stride = sizeof( Vertex );
	offset = 0;
	pD3D11DeviceContext->IASetVertexBuffers(0, 1, &m_pVertexBuffer, &stride, &offset);
	pD3D11DeviceContext->IASetIndexBuffer(m_pIndexBuffer, DXGI_FORMAT_R32_UINT, 0);
	pD3D11DeviceContext->IASetPrimitiveTopology(D3D11_PRIMITIVE_TOPOLOGY_TRIANGLELIST); 
	pD3D11DeviceContext->IASetInputLayout(m_pInputLayout);

	m_pWorld->SetMatrix(reinterpret_cast< float* >( &matrix.model ));
	m_pView->SetMatrix( reinterpret_cast< float* >( &matrix.view ));
	m_pProj->SetMatrix( reinterpret_cast< float* >( &matrix.proj ));

	D3DX11_TECHNIQUE_DESC techDesc;
	m_pEffectTechnique->GetDesc(&techDesc);
	for ( UINT p = 0; p<techDesc.Passes; ++p )
	{
		m_pEffectTechnique->GetPassByIndex(p)->Apply(0, pD3D11DeviceContext);
		pD3D11DeviceContext->DrawIndexed(m_IndexCount, 0, 0);
	}


}

void Hill::Shutdown()
{
	ReleaseCOM(m_pVertexBuffer)
	ReleaseCOM(m_pIndexBuffer)
	ReleaseCOM(m_pInputLayout)
}

 float Hill::GetHeight(float x, float z) const
{
	return 0.3f * ( z*sinf(0.1f * x) + x*cosf(0.1f * z) );
}

void Hill::init_buffer(ID3D11Device *pD3D11Device, ID3D11DeviceContext *pD3D11DeviceContext)
{
	HRESULT hr;
	d3d::Geometry::MeshData gridMesh;
	m_Geometry.CreateGrid(160.0f, 160.0f, 50, 50, gridMesh);

	/////////////////////////////Vertex Buffer//////////////////////////////

	m_VertexCount = gridMesh.VertexData.size();
	m_VertexData.resize(m_VertexCount);
	for(size_t i = 0; i < m_VertexCount; ++i)
	{
		XMFLOAT3 p = gridMesh.VertexData[i].Pos;
		p.y = GetHeight(p.x, p.z);
		m_VertexData[i].Pos   = p;

		if( p.y < -10.0f )
			m_VertexData[i].Color = XMFLOAT4(1.0f, 0.96f, 0.62f, 1.0f);
		else if( p.y < 5.0f )
			m_VertexData[i].Color = XMFLOAT4(0.48f, 0.77f, 0.46f, 1.0f);
		else if( p.y < 12.0f )
			m_VertexData[i].Color = XMFLOAT4(0.1f, 0.48f, 0.19f, 1.0f);
		else if( p.y < 20.0f )
			m_VertexData[i].Color = XMFLOAT4(0.45f, 0.39f, 0.34f, 1.0f);
		else
			m_VertexData[i].Color = XMFLOAT4(1.0f, 1.0f, 1.0f, 1.0f);
	}

	D3D11_BUFFER_DESC vertexBufferDesc;
	vertexBufferDesc.Usage               = D3D11_USAGE_IMMUTABLE;
	vertexBufferDesc.ByteWidth           = sizeof(Vertex) * m_VertexCount;
	vertexBufferDesc.BindFlags           = D3D11_BIND_VERTEX_BUFFER;
	vertexBufferDesc.CPUAccessFlags      = 0;
	vertexBufferDesc.MiscFlags           = 0;
	vertexBufferDesc.StructureByteStride = 0;

	D3D11_SUBRESOURCE_DATA VBO;
	VBO.pSysMem = &m_VertexData[0];
	hr = pD3D11Device->CreateBuffer(&vertexBufferDesc, &VBO, &m_pVertexBuffer);
	//DebugHR(hr);

	/////////////////////////////Index Buffer//////////////////////////////
	m_IndexCount = gridMesh.IndexData.size();

	D3D11_BUFFER_DESC indexBufferDesc;
	indexBufferDesc.Usage               = D3D11_USAGE_IMMUTABLE;
	indexBufferDesc.ByteWidth           = sizeof(UINT) * m_IndexCount;
	indexBufferDesc.BindFlags           = D3D11_BIND_INDEX_BUFFER;
	indexBufferDesc.CPUAccessFlags      = 0;
	indexBufferDesc.MiscFlags           = 0;
	indexBufferDesc.StructureByteStride = 0;

	D3D11_SUBRESOURCE_DATA IBO;
	IBO.pSysMem = &gridMesh.IndexData[0];
	hr = pD3D11Device->CreateBuffer(&indexBufferDesc, &IBO, &m_pIndexBuffer);
	//DebugHR(hr);

}

void Hill::init_shader(ID3D11Device *pD3D11Device, HWND hWnd)
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
	D3DX11CompileEffectFromFile(L"hill.fx", nullptr, D3D_COMPILE_STANDARD_FILE_INCLUDE, dwShaderFlags, 0, pD3D11Device, &m_pEffect, nullptr);

#else

	ID3DBlob* pEffectBuffer = nullptr;
	V_RETURN(DXUTCompileFromFile(L"Tutorial11.fx", nullptr, "none", "fx_5_0", dwShaderFlags, 0, &pEffectBuffer));
	hr = D3DX11CreateEffectFromMemory(pEffectBuffer->GetBufferPointer(), pEffectBuffer->GetBufferSize(), 0, pd3dDevice, &m_pEffect);
	SAFE_RELEASE(pEffectBuffer);
	if (FAILED(hr))
		return hr;

#endif

	m_pEffectTechnique = m_pEffect->GetTechniqueByName("HillTech");


	m_pWorld = m_pEffect->GetVariableByName("g_World")->AsMatrix();
	m_pView  = m_pEffect->GetVariableByName("g_View")->AsMatrix();
	m_pProj  = m_pEffect->GetVariableByName("g_Proj")->AsMatrix();

	// Done with compiled shader.
	//

	D3D11_INPUT_ELEMENT_DESC vertexDesc[] =
	{
		{"POSITION", 0, DXGI_FORMAT_R32G32B32_FLOAT, 0, 0, D3D11_INPUT_PER_VERTEX_DATA, 0},
		{"COLOR", 0, DXGI_FORMAT_R32G32B32A32_FLOAT, 0, 12, D3D11_INPUT_PER_VERTEX_DATA, 0}
	};

	D3DX11_PASS_DESC passDesc;
	m_pEffectTechnique->GetPassByIndex(0)->GetDesc(&passDesc);

	pD3D11Device->CreateInputLayout(vertexDesc, 2, passDesc.pIAInputSignature,
		passDesc.IAInputSignatureSize, &m_pInputLayout);

   
}


}