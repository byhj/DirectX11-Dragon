#include "Hill.h"


namespace byhj
{


float Hill::GetHeight(float x, float z) const
{
	return 0.3f * ( z*sinf(0.1f * x) + x*cosf(0.1f * z) );
}

void Hill::Init(ID3D11Device *pD3D11Device, ID3D11DeviceContext *pD3D11DeviceContext, HWND hWnd)
{
	init_buffer(pD3D11Device, pD3D11DeviceContext);
	init_shader(pD3D11Device, hWnd);
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

	D3D11_BUFFER_DESC landVBDesc;
	landVBDesc.Usage               = D3D11_USAGE_IMMUTABLE;
	landVBDesc.ByteWidth           = sizeof(Vertex) * m_VertexCount;
	landVBDesc.BindFlags           = D3D11_BIND_VERTEX_BUFFER;
	landVBDesc.CPUAccessFlags      = 0;
	landVBDesc.MiscFlags           = 0;
	landVBDesc.StructureByteStride = 0;

	D3D11_SUBRESOURCE_DATA landVBO;
	landVBO.pSysMem = &m_VertexData[0];
	hr = pD3D11Device->CreateBuffer(&landVBDesc, &landVBO, &m_pLandVB);
	DebugHR(hr);

	/////////////////////////////Index Buffer//////////////////////////////
	m_IndexCount = gridMesh.IndexData.size();

	D3D11_BUFFER_DESC landIBDesc;
	landIBDesc.Usage               = D3D11_USAGE_IMMUTABLE;
	landIBDesc.ByteWidth           = sizeof(UINT) * m_IndexCount;
	landIBDesc.BindFlags           = D3D11_BIND_INDEX_BUFFER;
	landIBDesc.CPUAccessFlags      = 0;
	landIBDesc.MiscFlags           = 0;
	landIBDesc.StructureByteStride = 0;

	D3D11_SUBRESOURCE_DATA landIBO;
	landIBO.pSysMem = &gridMesh.IndexData[0];
	hr = pD3D11Device->CreateBuffer(&landIBDesc, &landIBO, &m_pLandIB);
	DebugHR(hr);

	////////////////////////////////Const Buffer//////////////////////////////////////

	wave.Init(200, 200, 0.8f, 0.03f, 3.25f, 0.4f);
	D3D11_BUFFER_DESC waveVBDesce;
	waveVBDesce.Usage               = D3D11_USAGE_DYNAMIC;
	waveVBDesce.ByteWidth           = sizeof(Vertex) * wave.VertexCount();
	waveVBDesce.BindFlags           = D3D11_BIND_VERTEX_BUFFER;
	waveVBDesce.CPUAccessFlags      = D3D11_CPU_ACCESS_WRITE;
	waveVBDesce.MiscFlags           = 0;
	waveVBDesce.StructureByteStride = 0;

	hr = pD3D11Device->CreateBuffer(&waveVBDesce, NULL, &m_pWaveVB);
	DebugHR(hr);

	/////////////////////////////Index Buffer//////////////////////////////
	std::vector<UINT> indices(3 * wave.TriangleCount() ); // 3 indices per face

	// Iterate over each quad.
	UINT m = wave.RowCount();
	UINT n = wave.ColumnCount();
	int k = 0;
	for(UINT i = 0; i < m-1; ++i)
	{
		for(DWORD j = 0; j < n-1; ++j)
		{
			indices[k]   = i*n+j;
			indices[k+1] = i*n+j+1;
			indices[k+2] = (i+1)*n+j;

			indices[k+3] = (i+1)*n+j;
			indices[k+4] = i*n+j+1;
			indices[k+5] = (i+1)*n+j+1;

			k += 6; // next quad
		}
	}
	D3D11_BUFFER_DESC waveIBDesc;
	waveIBDesc.Usage               = D3D11_USAGE_IMMUTABLE;
	waveIBDesc.ByteWidth           = sizeof(UINT) * indices.size();
	waveIBDesc.BindFlags           = D3D11_BIND_INDEX_BUFFER;
	waveIBDesc.CPUAccessFlags      = 0;
	waveIBDesc.MiscFlags           = 0;
	waveIBDesc.StructureByteStride = 0;

	D3D11_SUBRESOURCE_DATA waveIBO;
	waveIBO.pSysMem = &indices[0];
	hr = pD3D11Device->CreateBuffer(&waveIBDesc, &waveIBO, &m_pWaveIB);
	DebugHR(hr);


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

void Hill::init_shader(ID3D11Device *pD3D11Device, HWND hWnd)
{
	D3D11_INPUT_ELEMENT_DESC pInputLayoutDesc[2];
	pInputLayoutDesc[0].SemanticName         = "POSITION";
	pInputLayoutDesc[0].SemanticIndex        = 0;
	pInputLayoutDesc[0].Format               = DXGI_FORMAT_R32G32B32_FLOAT;
	pInputLayoutDesc[0].InputSlot            = 0;
	pInputLayoutDesc[0].AlignedByteOffset    = 0;
	pInputLayoutDesc[0].InputSlotClass       = D3D11_INPUT_PER_VERTEX_DATA;
	pInputLayoutDesc[0].InstanceDataStepRate = 0;

	pInputLayoutDesc[1].SemanticName         = "COLOR";
	pInputLayoutDesc[1].SemanticIndex        = 0;
	pInputLayoutDesc[1].Format               = DXGI_FORMAT_R32G32B32A32_FLOAT;
	pInputLayoutDesc[1].InputSlot            = 0;
	pInputLayoutDesc[1].AlignedByteOffset    = 12;
	pInputLayoutDesc[1].InputSlotClass       = D3D11_INPUT_PER_VERTEX_DATA;
	pInputLayoutDesc[1].InstanceDataStepRate = 0;

	unsigned numElements = ARRAYSIZE(pInputLayoutDesc);

	CubeShader.init(pD3D11Device, hWnd);
	CubeShader.attachVS(L"wave.vsh", pInputLayoutDesc, numElements);
	CubeShader.attachPS(L"wave.psh");
	CubeShader.end();
}

}