#include "cube.h"
#include "d3d/d3dUtil.h"


void Geometry::init_light()
{
	m_DirLights[0].Ambient  = XMFLOAT4(0.3f, 0.3f, 0.3f, 1.0f);
	m_DirLights[0].Diffuse  = XMFLOAT4(0.8f, 0.8f, 0.8f, 1.0f);
	m_DirLights[0].Specular = XMFLOAT4(0.6f, 0.6f, 0.6f, 16.0f);
	m_DirLights[0].Direction = XMFLOAT3(0.707f, -0.707f, 0.0f);

	m_DirLights[1].Ambient  = XMFLOAT4(0.2f, 0.2f, 0.2f, 1.0f);
	m_DirLights[1].Diffuse  = XMFLOAT4(1.4f, 1.4f, 1.4f, 1.0f);
	m_DirLights[1].Specular = XMFLOAT4(0.3f, 0.3f, 0.3f, 16.0f);
	m_DirLights[1].Direction = XMFLOAT3(-0.707f, 0.0f, 0.707f);

	m_CubeMat.Ambient  = XMFLOAT4(0.5f, 0.5f, 0.5f, 1.0f);
	m_CubeMat.Diffuse  = XMFLOAT4(1.0f, 1.0f, 1.0f, 1.0f);
	m_CubeMat.Specular = XMFLOAT4(0.6f, 0.6f, 0.6f, 16.0f);
}


void Geometry::init_buffer(ID3D11Device *pD3D11Device, ID3D11DeviceContext *pD3D11DeviceContext)
{	
	init_light();

	HRESULT hr;
	D3DGeometry::MeshData box;
	D3DGeometry geoGen;
	geoGen.CreateCube(1.0f, 1.0f, 1.0f, box);

	m_IndexCount  = box.IndexData.size();
	m_VertexCount = box.VertexData.size();
    std::vector<Vertex> VertexData(m_VertexCount);

	UINT k = 0;
	for(size_t i = 0; i < box.VertexData.size(); ++i, ++k)
	{
		VertexData[k].Pos    = box.VertexData[i].Pos;
		VertexData[k].Normal = box.VertexData[i].Normal;
		VertexData[k].Tex    = box.VertexData[i].Tex;
	}

	D3D11_BUFFER_DESC cubeVBDesc;
	cubeVBDesc.Usage               = D3D11_USAGE_IMMUTABLE;
	cubeVBDesc.ByteWidth           = sizeof(Vertex) * m_VertexCount;
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
	cubeIBDesc.ByteWidth           = sizeof(UINT) * m_IndexCount;
	cubeIBDesc.BindFlags           = D3D11_BIND_INDEX_BUFFER;
	cubeIBDesc.CPUAccessFlags      = 0;
	cubeIBDesc.MiscFlags           = 0;
	cubeIBDesc.StructureByteStride = 0;

	D3D11_SUBRESOURCE_DATA shapeIBO;
	shapeIBO.pSysMem = &box.IndexData[0];
	hr = pD3D11Device->CreateBuffer(&cubeIBDesc, &shapeIBO, &m_pCubeIB);
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


	D3D11_BUFFER_DESC lightBufferDesc;	
	ZeroMemory(&lightBufferDesc, sizeof(D3D11_BUFFER_DESC));
	lightBufferDesc.Usage          = D3D11_USAGE_DEFAULT;
	lightBufferDesc.ByteWidth      = sizeof(LightBuffer);
	lightBufferDesc.BindFlags      = D3D11_BIND_CONSTANT_BUFFER;
	lightBufferDesc.CPUAccessFlags = 0;
	lightBufferDesc.MiscFlags      = 0;
	hr = pD3D11Device->CreateBuffer(&lightBufferDesc, NULL, &m_pLightBuffer);
	DebugHR(hr);

	D3D11_BUFFER_DESC matBufferDesc;	
	ZeroMemory(&matBufferDesc, sizeof(D3D11_BUFFER_DESC));
	matBufferDesc.Usage          = D3D11_USAGE_DEFAULT;
	matBufferDesc.ByteWidth      = sizeof(Material);
	matBufferDesc.BindFlags      = D3D11_BIND_CONSTANT_BUFFER;
	matBufferDesc.CPUAccessFlags = 0;
	matBufferDesc.MiscFlags      = 0;
	hr = pD3D11Device->CreateBuffer(&matBufferDesc, NULL, &m_pMaterialBuffer);
	DebugHR(hr);
}

void Geometry::init_texture(ID3D11Device *pD3D11Device)
{
	HRESULT hr;
	hr = D3DX11CreateShaderResourceViewFromFile(pD3D11Device, L"../../media/textures/WoodCrate01.dds", 0, 0, &m_pDiffuseTexSRV, 0 );
	DebugHR(hr);
}

void Geometry::init_shader(ID3D11Device *pD3D11Device, HWND hWnd)
{
	D3D11_INPUT_ELEMENT_DESC pInputLayoutDesc[3];
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

	pInputLayoutDesc[2].SemanticName         = "TEXCOORD";
	pInputLayoutDesc[2].SemanticIndex        = 0;
	pInputLayoutDesc[2].Format               = DXGI_FORMAT_R32G32B32_FLOAT;
	pInputLayoutDesc[2].InputSlot            = 0;
	pInputLayoutDesc[2].AlignedByteOffset    = 24;
	pInputLayoutDesc[2].InputSlotClass       = D3D11_INPUT_PER_VERTEX_DATA;
	pInputLayoutDesc[2].InstanceDataStepRate = 0;

	unsigned numElements = ARRAYSIZE(pInputLayoutDesc);

	CubeShader.init(pD3D11Device, hWnd);
	CubeShader.attachVS(L"model.vsh", pInputLayoutDesc, numElements);
	CubeShader.attachPS(L"model.psh");
	CubeShader.end();
}