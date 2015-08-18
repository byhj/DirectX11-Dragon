#include "model.h"
#include "d3d/d3dUtil.h"
#include <fstream>

namespace byhj
{

void Model::Init(ID3D11Device *pD3D11Device, ID3D11DeviceContext *pD3D11DeviceContext, HWND hWnd)
{
	init_buffer(pD3D11Device, pD3D11DeviceContext);
	init_shader(pD3D11Device, hWnd);
}

void Model::init_buffer(ID3D11Device *pD3D11Device, ID3D11DeviceContext *pD3D11DeviceContext)
{
	HRESULT hr;

	//Load the model file
	std::ifstream fin("../../media/objects/skull.txt");
	if (!fin)
	{
		MessageBox(NULL, L"Can not open the model file!", 0, 0);
		return; 
	}
	std::string ignore;
	fin >> ignore >> m_VertexCount;
	fin >> ignore >> m_IndexCount;
	fin >> ignore >> ignore >> ignore >> ignore;

	m_VertexData.resize(m_VertexCount);
	for (UINT i = 0; i != m_VertexCount; ++i)
	{
		fin >> m_VertexData[i].Pos.x 
			>> m_VertexData[i].Pos.y
			>> m_VertexData[i].Pos.z;

		fin >> m_VertexData[i].Normal.x 
			>> m_VertexData[i].Normal.y
			>> m_VertexData[i].Normal.z;
	}
	fin >> ignore >> ignore >> ignore;
	/////////////////////////////Vertex Buffer//////////////////////////////

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
	DebugHR(hr);

	/////////////////////////////Index Buffer//////////////////////////////

	m_IndexCount *= 3;
	m_IndexData.resize(m_IndexCount);
	for (UINT i = 0; i != m_IndexCount; ++i)
		fin >> m_IndexData[i];
	fin.close();

	D3D11_BUFFER_DESC indexBufferDesc;
	indexBufferDesc.Usage               = D3D11_USAGE_IMMUTABLE;
	indexBufferDesc.ByteWidth           = sizeof(UINT) * m_IndexCount;
	indexBufferDesc.BindFlags           = D3D11_BIND_INDEX_BUFFER;
	indexBufferDesc.CPUAccessFlags      = 0;
	indexBufferDesc.MiscFlags           = 0;
	indexBufferDesc.StructureByteStride = 0;

	D3D11_SUBRESOURCE_DATA IBO;
	IBO.pSysMem = &m_IndexData[0];
	hr = pD3D11Device->CreateBuffer(&indexBufferDesc, &IBO, &m_pIndexBuffer);
	DebugHR(hr);

	////////////////////////////////Const Buffer//////////////////////////////////////

	D3D11_BUFFER_DESC mvpDesc;	
	ZeroMemory(&mvpDesc, sizeof(D3D11_BUFFER_DESC));
	mvpDesc.Usage          = D3D11_USAGE_DEFAULT;
	mvpDesc.ByteWidth      = sizeof(MatrixBuffer);
	mvpDesc.BindFlags      = D3D11_BIND_CONSTANT_BUFFER;
	mvpDesc.CPUAccessFlags = 0;
	mvpDesc.MiscFlags      = 0;
	hr = pD3D11Device->CreateBuffer(&mvpDesc, NULL, &m_pMVPBuffer);
	DebugHR(hr);
}

void Model::init_shader(ID3D11Device *pD3D11Device, HWND hWnd)
{
	D3D11_INPUT_ELEMENT_DESC pInputLayoutDesc[2];
	pInputLayoutDesc[0].SemanticName         = "POSITION";
	pInputLayoutDesc[0].SemanticIndex        = 0;
	pInputLayoutDesc[0].Format               = DXGI_FORMAT_R32G32B32_FLOAT;
	pInputLayoutDesc[0].InputSlot            = 0;
	pInputLayoutDesc[0].AlignedByteOffset    = 0;
	pInputLayoutDesc[0].InputSlotClass       = D3D11_INPUT_PER_VERTEX_DATA;
	pInputLayoutDesc[0].InstanceDataStepRate = 0;

	pInputLayoutDesc[1].SemanticName         = "NORMAL";
	pInputLayoutDesc[1].SemanticIndex        = 0;
	pInputLayoutDesc[1].Format               = DXGI_FORMAT_R32G32B32_FLOAT;
	pInputLayoutDesc[1].InputSlot            = 0;
	pInputLayoutDesc[1].AlignedByteOffset    = 12;
	pInputLayoutDesc[1].InputSlotClass       = D3D11_INPUT_PER_VERTEX_DATA;
	pInputLayoutDesc[1].InstanceDataStepRate = 0;

	unsigned numElements = ARRAYSIZE(pInputLayoutDesc);

	CubeShader.init(pD3D11Device, hWnd);
	CubeShader.attachVS(L"Model.vsh", pInputLayoutDesc, numElements);
	CubeShader.attachPS(L"Model.psh");
	CubeShader.end();
}


}