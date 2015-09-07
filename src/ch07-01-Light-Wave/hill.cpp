#include "Hill.h"


namespace byhj
{

void Hill::Init(ID3D11Device *pD3D11Device, ID3D11DeviceContext *pD3D11DeviceContext, HWND hWnd)
{
	init_buffer(pD3D11Device, pD3D11DeviceContext);
	init_shader(pD3D11Device, hWnd);
}

void Hill::Render(ID3D11DeviceContext *pD3D11DeviceContext, const d3d::MatrixBuffer &matrix, D3DTimer *timer, D3DCamera *camera)
{
		// Every quarter second, generate a random wave.
		static float t_base = 0.0f;
		if ((timer->GetTotalTime() - t_base) >= 0.25f)
		{
			t_base += 0.25f;

			DWORD i = 5 + rand() % 190;
			DWORD j = 5 + rand() % 190;

			float r = MathHelper::RandF(1.0f, 2.0f);
			wave.Disturb(i, j, r);
		}
		wave.Update(timer->GetDeltaTime());

		D3D11_MAPPED_SUBRESOURCE mappedData;
		pD3D11DeviceContext->Map(m_pWaveVB, 0, D3D11_MAP_WRITE_DISCARD, 0, &mappedData);

		Vertex* v = reinterpret_cast<Vertex*>(mappedData.pData);
		for (UINT i = 0; i < wave.VertexCount(); ++i)
		{
			v[i].Pos = wave[i];
			v[i].Normal = wave.Normal(i);
		}
		pD3D11DeviceContext->Unmap(m_pWaveVB, 0);

		/////////////////////////////Update Light and Matrix//////////////////////////////////
		XMFLOAT3 camPos = camera->GetPos();

		// Circle light over the land surface.
		m_PointLight.Position.x = 70.0f*cosf(0.2f*timer->GetTotalTime());
		m_PointLight.Position.z = 70.0f*sinf(0.2f*timer->GetTotalTime());
		m_PointLight.Position.y = MathHelper::Max(GetHillHeight(m_PointLight.Position.x,
			m_PointLight.Position.z), -3.0f) + 10.0f;
		m_SpotLight.Position = camPos;
		XMVECTOR pos = XMVectorSet(camPos.x, camPos.y, camPos.z, 1.0f);
		XMStoreFloat3(&m_SpotLight.Direction, XMVector3Normalize(-pos));


		cbLight.g_DirLight   = m_DirLight;
		cbLight.g_PointLight = m_PointLight;
		cbLight.g_SpotLight  = m_SpotLight;
		cbLight.g_EyePos     = camPos;
		pD3D11DeviceContext->UpdateSubresource(m_pLightBuffer, 0, NULL, &cbLight, 0, 0);
		pD3D11DeviceContext->PSSetConstantBuffers(0, 1, &m_pLightBuffer);

		cbMatrix.model = matrix.model;
		cbMatrix.view  = matrix.view;
		cbMatrix.proj  = matrix.proj;
		pD3D11DeviceContext->UpdateSubresource(m_pMVPBuffer, 0, NULL, &cbMatrix, 0, 0);
		pD3D11DeviceContext->VSSetConstantBuffers(0, 1, &m_pMVPBuffer);

		// Set vertex buffer stride and offset
		unsigned int stride;
		unsigned int offset;
		stride = sizeof(Vertex);
		offset = 0;

		////////////////////////// Land  //////////////////////////////

		cbMaterial  = m_LandMat;
		pD3D11DeviceContext->UpdateSubresource(m_pMaterialBuffer, 0, NULL, &cbMaterial, 0, 0);
		pD3D11DeviceContext->PSSetConstantBuffers(1, 1, &m_pMaterialBuffer);
		pD3D11DeviceContext->IASetVertexBuffers(0, 1, &m_pLandVB, &stride, &offset);
		pD3D11DeviceContext->IASetIndexBuffer(m_pLandIB, DXGI_FORMAT_R32_UINT, 0);
		CubeShader.use(pD3D11DeviceContext);
		pD3D11DeviceContext->DrawIndexed(m_IndexCount, 0, 0);


		////////////////////////// Wave //////////////////////////////
		cbMaterial  = m_WavesMat;
		pD3D11DeviceContext->UpdateSubresource(m_pMaterialBuffer, 0, NULL, &cbMaterial, 0, 0);
		pD3D11DeviceContext->PSSetConstantBuffers(1, 1, &m_pMaterialBuffer);
		pD3D11DeviceContext->IASetVertexBuffers(0, 1, &m_pWaveVB, &stride, &offset);
		pD3D11DeviceContext->IASetIndexBuffer(m_pWaveIB, DXGI_FORMAT_R32_UINT, 0);
		CubeShader.use(pD3D11DeviceContext);
		pD3D11DeviceContext->DrawIndexed(3 * wave.TriangleCount(), 0, 0);
}


float Hill::GetHillHeight(float x, float z) const
{
	return 0.3f * ( z*sinf(0.1f * x) + x*cosf(0.1f * z) );
}

XMFLOAT3 Hill::GetHillNormal(float x, float z) const
{
	// n = (-df/dx, 1, -df/dz)
	XMFLOAT3 n(
		-0.03f*z*cosf(0.1f*x) - 0.3f*cosf(0.1f*z),
		1.0f,
		-0.3f*sinf(0.1f*x) + 0.03f*x*sinf(0.1f*z));

	XMVECTOR unitNormal = XMVector3Normalize(XMLoadFloat3(&n));
	XMStoreFloat3(&n, unitNormal);

	return n;
}

void Hill::init_light()
{
	// Directional light.
	m_DirLight.Ambient  = XMFLOAT4(0.2f, 0.2f, 0.2f, 1.0f);
	m_DirLight.Diffuse  = XMFLOAT4(0.5f, 0.5f, 0.5f, 1.0f);
	m_DirLight.Specular = XMFLOAT4(0.5f, 0.5f, 0.5f, 1.0f);
	m_DirLight.Direction = XMFLOAT3(0.57735f, -0.57735f, 0.57735f);
	m_DirLight.padding = 0.0f;

	// Point light--position is changed every frame to animate in UpdateScene function.
	m_PointLight.Ambient  = XMFLOAT4(0.3f, 0.3f, 0.3f, 1.0f);
	m_PointLight.Diffuse  = XMFLOAT4(0.7f, 0.7f, 0.7f, 1.0f);
	m_PointLight.Specular = XMFLOAT4(0.7f, 0.7f, 0.7f, 1.0f);
	m_PointLight.Att      = XMFLOAT3(0.0f, 0.1f, 0.0f);
	m_PointLight.Range    = 25.0f;
	m_PointLight.padding  = 0.0f;

	// Spot light--position and direction changed every frame to animate in UpdateScene function.
	m_SpotLight.Ambient  = XMFLOAT4(0.0f, 0.0f, 0.0f, 1.0f);
	m_SpotLight.Diffuse  = XMFLOAT4(1.0f, 1.0f, 0.0f, 1.0f);
	m_SpotLight.Specular = XMFLOAT4(1.0f, 1.0f, 1.0f, 1.0f);
	m_SpotLight.Att      = XMFLOAT3(1.0f, 0.0f, 0.0f);
	m_SpotLight.Spot     = 96.0f;
	m_SpotLight.Range    = 10000.0f;
	m_SpotLight.Pad      =  0.0f;
	//Material
	m_LandMat.Ambient  = XMFLOAT4(0.48f, 0.77f, 0.46f, 1.0f);
	m_LandMat.Diffuse  = XMFLOAT4(0.48f, 0.77f, 0.46f, 1.0f);
	m_LandMat.Specular = XMFLOAT4(0.2f, 0.2f, 0.2f, 16.0f);
	m_LandMat.Reflect  = XMFLOAT4(0.0f, 0.0f, 0.0f, 0.0f);

	m_WavesMat.Ambient  = XMFLOAT4(0.137f, 0.42f, 0.556f, 1.0f);
	m_WavesMat.Diffuse  = XMFLOAT4(0.137f, 0.42f, 0.556f, 1.0f);
	m_WavesMat.Specular = XMFLOAT4(0.8f, 0.8f, 0.8f, 96.0f);
	m_WavesMat.Reflect  = XMFLOAT4(0.0f, 0.0f, 0.0f, 0.0f);
}

void Hill::init_buffer(ID3D11Device *pD3D11Device, ID3D11DeviceContext *pD3D11DeviceContext)
{
	init_light();

	HRESULT hr;
	d3d::Geometry::MeshData gridMesh;
	m_Geometry.CreateGrid(160.0f, 160.0f, 50, 50, gridMesh);

	/////////////////////////////Vertex Buffer//////////////////////////////

	m_VertexCount = gridMesh.VertexData.size();
	m_VertexData.resize(m_VertexCount);
	for(size_t i = 0; i < m_VertexCount; ++i)
	{
		XMFLOAT3 p = gridMesh.VertexData[i].Pos;
		p.y = GetHillHeight(p.x, p.z);
		m_VertexData[i].Pos   = p;
        m_VertexData[i].Normal = GetHillNormal(p.x, p.z);
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

	pInputLayoutDesc[1].SemanticName         = "NORMAL";
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