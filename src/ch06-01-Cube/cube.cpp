#include "cube.h"
#include "d3d/d3dUtil.h"
#include <D3DX11.h>


namespace byhj
{

Cube::Cube()
{

}

Cube::~Cube() 
{

}


void Cube::Init(ID3D11Device *pD3D11Device, ID3D11DeviceContext *pD3D11DeviceContext, HWND hWnd)
{
	init_buffer(pD3D11Device, pD3D11DeviceContext);
	init_shader(pD3D11Device, hWnd);
}



void Cube::Render(ID3D11DeviceContext *pD3D11DeviceContext, byhj::MatrixBuffer matrix)
{

	pD3D11DeviceContext->IASetInputLayout(m_pInputLayout);
	pD3D11DeviceContext->IASetPrimitiveTopology(D3D11_PRIMITIVE_TOPOLOGY_TRIANGLELIST);

	// Set vertex buffer stride and offset
	unsigned int stride;
	unsigned int offset;
	stride = sizeof(Vertex);
	offset = 0;
	pD3D11DeviceContext->IASetVertexBuffers(0, 1, &m_pVertexBuffer, &stride, &offset);
	pD3D11DeviceContext->IASetIndexBuffer(m_pIndexBuffer, DXGI_FORMAT_R32_UINT, 0);

	m_pWorld->SetMatrix(reinterpret_cast< float* >( &matrix.model ));
	m_pView->SetMatrix(reinterpret_cast< float* >( &matrix.view ));
	m_pProj->SetMatrix(reinterpret_cast< float* >( &matrix.proj ));

	D3DX11_TECHNIQUE_DESC techDesc;
	m_pEffectTechnique->GetDesc(&techDesc);
	for ( UINT p = 0; p<techDesc.Passes; ++p )
	{
		m_pEffectTechnique->GetPassByIndex(p)->Apply(0, pD3D11DeviceContext);
		pD3D11DeviceContext->DrawIndexed(m_IndexCount, 0, 0);
	}



}

void Cube::Shutdown()
{
	ReleaseCOM(m_pVertexBuffer)
	ReleaseCOM(m_pIndexBuffer)
	ReleaseCOM(m_pInputLayout)
}

void Cube::init_buffer(ID3D11Device *pD3D11Device, ID3D11DeviceContext *pD3D11DeviceContext)
{
	HRESULT hr;

	/////////////////////////////Vertex Buffer//////////////////////////////
	Vertex VertexData[] =
	{
		{XMFLOAT3(-1.0f, -1.0f, -1.0f), Colors::White},
		{XMFLOAT3(-1.0f, +1.0f, -1.0f), Colors::Black},
		{XMFLOAT3(+1.0f, +1.0f, -1.0f), Colors::Red},
		{XMFLOAT3(+1.0f, -1.0f, -1.0f), Colors::Green},
		{XMFLOAT3(-1.0f, -1.0f, +1.0f), Colors::Blue},
		{XMFLOAT3(-1.0f, +1.0f, +1.0f), Colors::Yellow},
		{XMFLOAT3(+1.0f, +1.0f, +1.0f), Colors::Cyan},
		{XMFLOAT3(+1.0f, -1.0f, +1.0f), Colors::Magenta}
	};
	m_VertexCount = ARRAYSIZE(VertexData);

	D3D11_BUFFER_DESC vertexBufferDesc;
	vertexBufferDesc.Usage               = D3D11_USAGE_IMMUTABLE;
	vertexBufferDesc.ByteWidth           = sizeof(Vertex) * m_VertexCount;
	vertexBufferDesc.BindFlags           = D3D11_BIND_VERTEX_BUFFER;
	vertexBufferDesc.CPUAccessFlags      = 0;
	vertexBufferDesc.MiscFlags           = 0;
	vertexBufferDesc.StructureByteStride = 0;

	D3D11_SUBRESOURCE_DATA VBO;
	VBO.pSysMem = VertexData;
	hr = pD3D11Device->CreateBuffer(&vertexBufferDesc, &VBO, &m_pVertexBuffer);
	DebugHR(hr);

	/////////////////////////////Index Buffer//////////////////////////////
	UINT IndexData[] = {
		// front face
		0, 1, 2,
		0, 2, 3,

		// back face
		4, 6, 5,
		4, 7, 6,

		// left face
		4, 5, 1,
		4, 1, 0,

		// right face
		3, 2, 6,
		3, 6, 7,

		// top face
		1, 5, 6,
		1, 6, 2,

		// bottom face
		4, 0, 3, 
		4, 3, 7
	};
	m_IndexCount = ARRAYSIZE(IndexData);

	D3D11_BUFFER_DESC indexBufferDesc;
	indexBufferDesc.Usage               = D3D11_USAGE_IMMUTABLE;
	indexBufferDesc.ByteWidth           = sizeof(UINT) * m_IndexCount;
	indexBufferDesc.BindFlags           = D3D11_BIND_INDEX_BUFFER;
	indexBufferDesc.CPUAccessFlags      = 0;
	indexBufferDesc.MiscFlags           = 0;
	indexBufferDesc.StructureByteStride = 0;

	D3D11_SUBRESOURCE_DATA IBO;
	IBO.pSysMem = IndexData;
	hr = pD3D11Device->CreateBuffer(&indexBufferDesc, &IBO, &m_pIndexBuffer);
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
	HRESULT hr = D3DX11CompileFromFile(L"color.fx", 0, 0, 0, "fx_5_0", shaderFlags,
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

	m_pEffectTechnique = m_pEffect->GetTechniqueByName("ColorTech");


	m_pWorld = m_pEffect->GetVariableByName("World")->AsMatrix();
	m_pView  = m_pEffect->GetVariableByName("View")->AsMatrix();
	m_pProj  = m_pEffect->GetVariableByName("Proj")->AsMatrix();

	// Done with compiled shader.
	ReleaseCOM(compiledShader);

	D3D11_INPUT_ELEMENT_DESC vertexDesc[] =
	{
		{"POSITION", 0, DXGI_FORMAT_R32G32B32_FLOAT, 0, 0, D3D11_INPUT_PER_VERTEX_DATA, 0},
		{"COLOR",    0, DXGI_FORMAT_R32G32B32A32_FLOAT, 0, 12, D3D11_INPUT_PER_VERTEX_DATA, 0}
	};
	
	D3DX11_PASS_DESC passDesc;
	m_pEffectTechnique->GetPassByIndex(0)->GetDesc(&passDesc);

	pD3D11Device->CreateInputLayout(vertexDesc, 2, passDesc.pIAInputSignature,
		                            passDesc.IAInputSignatureSize, &m_pInputLayout);
}

}