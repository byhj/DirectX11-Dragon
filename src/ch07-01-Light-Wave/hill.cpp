#include "Hill.h"
#include <D3DX11.h>

namespace byhj
{


float Hill::GetHeight(float x, float z) const
{
	return 0.3f * ( z*sinf(0.1f * x) + x*cosf(0.1f * z) );
}
XMFLOAT3 Hill::GetHillNormal(float x, float z) const
{
	// n = (-df/dx, 1, -df/dz)
	XMFLOAT3 n(
		-0.03f*z*cosf(0.1f*x)-0.3f*cosf(0.1f*z),
		1.0f,
		-0.3f*sinf(0.1f*x)+0.03f*x*sinf(0.1f*z));

	XMVECTOR unitNormal = XMVector3Normalize(XMLoadFloat3(&n));
	XMStoreFloat3(&n, unitNormal);

	return n;
}


void Hill::Init(ID3D11Device *pD3D11Device, ID3D11DeviceContext *pD3D11DeviceContext, HWND hWnd)
{
	init_buffer(pD3D11Device, pD3D11DeviceContext);
	init_shader(pD3D11Device, hWnd);
}


// Returns random float in [0, 1).
static float RandF()
{
	return (float)(rand()) / (float)RAND_MAX;
}
// Returns random float in [a, b).
static float RandF(float a, float b)
{
	return a + RandF() * (b - a);
}

void Hill::Render(ID3D11DeviceContext *pD3D11DeviceContext, const d3d::MatrixBuffer &matrix, d3d::Timer *pTimer, d3d::Camera *pCam)
{

	//
	// Animate the lights.
	//
	// Circle light over the land surface.
	m_PointLight.Position.x = 70.0f*cosf(0.2f * pTimer->GetTotalTime());
	m_PointLight.Position.z = 70.0f*sinf(0.2f * pTimer->GetTotalTime());
	m_PointLight.Position.y = max(GetHeight(m_PointLight.Position.x, m_PointLight.Position.z), -3.0f)+10.0f;


	// The spotlight takes on the camera position and is aimed in the
	// same direction the camera is looking.  In this way, it looks
	// like we are holding a flashlight.
	XMFLOAT4 camPos = pCam->GetCamPos();
	m_EyePos =  XMFLOAT3(camPos.x, camPos.y, camPos.z);
	XMVECTOR pos    = XMVectorSet(camPos.x, camPos.y, camPos.z, 0.0f);
	XMVECTOR target = XMVectorZero();
	XMVECTOR up     = XMVectorSet(0.0f, 1.0f, 0.0f, 0.0f);
	m_SpotLight.Position = m_EyePos;
	XMStoreFloat3(&m_SpotLight.Direction, XMVector3Normalize(target-pos));


	// Every quarter second, generate a random wave.

	static float t_base = 0.0f;
	if ((pTimer->GetTotalTime() - t_base) >= 0.25f)
	{
		t_base += 0.25f;

		DWORD i = 5 + rand() % 190;
		DWORD j = 5 + rand() % 190;

		float r = RandF(1.0f, 2.0f);
		m_Wave.Disturb(i, j, r);
	}

	m_Wave.Update(pTimer->GetDeltaTime());

	// Update the wave vertex buffer with the new solution.

	D3D11_MAPPED_SUBRESOURCE mappedData;
	pD3D11DeviceContext->Map(m_pWaveVB, 0, D3D11_MAP_WRITE_DISCARD, 0, &mappedData);

	Vertex* v = reinterpret_cast<Vertex*>(mappedData.pData);
	for (UINT i = 0; i < m_Wave.VertexCount(); ++i)
	{
		v[i].Pos = m_Wave[i];
	}

	pD3D11DeviceContext->Unmap(m_pWaveVB, 0);

	cbMatrix.model = matrix.model;
	cbMatrix.view  = matrix.view;
	cbMatrix.proj  = matrix.proj;

	// Set vertex buffer stride and offset
	unsigned int stride;
	unsigned int offset;
	stride = sizeof(Vertex);
	offset = 0;
	pD3D11DeviceContext->IASetPrimitiveTopology(D3D11_PRIMITIVE_TOPOLOGY_TRIANGLELIST);
	pD3D11DeviceContext->IASetInputLayout(m_pInputLayout);

	m_pFxDirLight->SetRawValue(&m_DirLight, 0, sizeof( m_DirLight ));
	m_pFxPointLight->SetRawValue(&m_PointLight, 0, sizeof( m_PointLight ));
	m_pFxSpotLight->SetRawValue(&m_SpotLight, 0, sizeof( m_SpotLight ));
	m_pFxEyePos->SetRawValue(&m_EyePos, 0, sizeof( m_EyePos ));
	m_pFxWorld->SetMatrix(reinterpret_cast< float* >( &cbMatrix.model ));
	m_pFxView->SetMatrix(reinterpret_cast< float* >( &cbMatrix.view ));
	m_pFxProj->SetMatrix(reinterpret_cast< float* >( &cbMatrix.proj ));

	D3DX11_TECHNIQUE_DESC techDesc;
	m_pEffectTechnique->GetDesc(&techDesc);
	for (UINT p = 0; p < techDesc.Passes; ++p)
	{

		m_pFxMaterial->SetRawValue(&m_LandMat, 0, sizeof( m_LandMat ));

		pD3D11DeviceContext->IASetVertexBuffers(0, 1, &m_pLandVB, &stride, &offset);
		pD3D11DeviceContext->IASetIndexBuffer(m_pLandIB, DXGI_FORMAT_R32_UINT, 0);
		m_pEffectTechnique->GetPassByIndex(p)->Apply(0, pD3D11DeviceContext);
		pD3D11DeviceContext->DrawIndexed(m_IndexCount, 0, 0);

//////////////////////////////////////////////////////////////////////////////////////////////

		m_pFxMaterial->SetRawValue(&m_WavesMat, 0, sizeof( m_WavesMat ));

		pD3D11DeviceContext->IASetVertexBuffers(0, 1, &m_pWaveVB, &stride, &offset);
		pD3D11DeviceContext->IASetIndexBuffer(m_pWaveIB, DXGI_FORMAT_R32_UINT, 0);
		m_pEffectTechnique->GetPassByIndex(p)->Apply(0, pD3D11DeviceContext);
		pD3D11DeviceContext->DrawIndexed(3 * m_Wave.TriangleCount(), 0, 0);
	}


}

void Hill::Shutdown()
{

	ReleaseCOM(m_pInputLayout)
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

	m_Wave.Init(200, 200, 0.8f, 0.03f, 3.25f, 0.4f);
	D3D11_BUFFER_DESC waveVBDesce;
	waveVBDesce.Usage               = D3D11_USAGE_DYNAMIC;
	waveVBDesce.ByteWidth           = sizeof(Vertex) * m_Wave.VertexCount();
	waveVBDesce.BindFlags           = D3D11_BIND_VERTEX_BUFFER;
	waveVBDesce.CPUAccessFlags      = D3D11_CPU_ACCESS_WRITE;
	waveVBDesce.MiscFlags           = 0;
	waveVBDesce.StructureByteStride = 0;

	hr = pD3D11Device->CreateBuffer(&waveVBDesce, NULL, &m_pWaveVB);
	DebugHR(hr);

	/////////////////////////////Index Buffer//////////////////////////////
	std::vector<UINT> indices(3 * m_Wave.TriangleCount() ); // 3 indices per face

	// Iterate over each quad.
	UINT m = m_Wave.RowCount();
	UINT n = m_Wave.ColumnCount();
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

	// Directional light.
	m_DirLight.Ambient  = XMFLOAT4(0.2f, 0.2f, 0.2f, 1.0f);
	m_DirLight.Diffuse  = XMFLOAT4(0.5f, 0.5f, 0.5f, 1.0f);
	m_DirLight.Specular = XMFLOAT4(0.5f, 0.5f, 0.5f, 1.0f);
	m_DirLight.Direction = XMFLOAT3(0.57735f, -0.57735f, 0.57735f);
	
	//Point light--position is changed every frame to animate in UpdateScene function.
	m_PointLight.Ambient  = XMFLOAT4(0.3f, 0.3f, 0.3f, 1.0f);
	m_PointLight.Diffuse  = XMFLOAT4(0.7f, 0.7f, 0.7f, 1.0f);
	m_PointLight.Specular = XMFLOAT4(0.7f, 0.7f, 0.7f, 1.0f);
	m_PointLight.Att      = XMFLOAT3(0.0f, 0.1f, 0.0f);
	m_PointLight.Range    = 25.0f;
	
	//Spot light--position and direction changed every frame to animate in UpdateScene function.
	m_SpotLight.Ambient  = XMFLOAT4(0.0f, 0.0f, 0.0f, 1.0f);
	m_SpotLight.Diffuse  = XMFLOAT4(1.0f, 1.0f, 0.0f, 1.0f);
	m_SpotLight.Specular = XMFLOAT4(1.0f, 1.0f, 1.0f, 1.0f);
	m_SpotLight.Att      = XMFLOAT3(1.0f, 0.0f, 0.0f);
	m_SpotLight.Spot     = 96.0f;
	m_SpotLight.Range    = 10000.0f;
	
	m_LandMat.Ambient  = XMFLOAT4(0.48f, 0.77f, 0.46f, 1.0f);
	m_LandMat.Diffuse  = XMFLOAT4(0.48f, 0.77f, 0.46f, 1.0f);
	m_LandMat.Specular = XMFLOAT4(0.2f, 0.2f, 0.2f, 16.0f);
	
	m_WavesMat.Ambient  = XMFLOAT4(0.137f, 0.42f, 0.556f, 1.0f);
	m_WavesMat.Diffuse  = XMFLOAT4(0.137f, 0.42f, 0.556f, 1.0f);
	m_WavesMat.Specular = XMFLOAT4(0.8f, 0.8f, 0.8f, 96.0f);
}

void Hill::init_shader(ID3D11Device *pD3D11Device, HWND hWnd)
{
	DWORD shaderFlags = 0;
#if defined( DEBUG ) || defined( _DEBUG )
	shaderFlags |= D3D10_SHADER_DEBUG;
	shaderFlags |= D3D10_SHADER_SKIP_OPTIMIZATION;
#endif

	ID3D10Blob* compiledShader = 0;
	ID3D10Blob* compilationMsgs = 0;
	HRESULT hr = D3DX11CompileFromFile(L"wave.fx", 0, 0, 0, "fx_5_0", shaderFlags,
		0, 0, &compiledShader, &compilationMsgs, 0);

	// compilationMsgs can store errors or warnings.
	if (compilationMsgs != 0)
	{
		MessageBoxA(0, (char*)compilationMsgs->GetBufferPointer(), 0, 0);
		ReleaseCOM(compilationMsgs);
	}

	// Even if there are no compilationMsgs, check to make sure there were no other errors.
	if (FAILED(hr))
	{
		DXTrace(__FILE__, (DWORD)__LINE__, hr, L"D3DX11CompileFromFile", true);
	}

	D3DX11CreateEffectFromMemory(compiledShader->GetBufferPointer(), compiledShader->GetBufferSize(),
		0, pD3D11Device, &m_pEffect);

	m_pEffectTechnique = m_pEffect->GetTechniqueByName("ModelTech");


	m_pFxWorld      = m_pEffect->GetVariableByName("g_World")->AsMatrix();
	m_pFxView       = m_pEffect->GetVariableByName("g_View")->AsMatrix();
	m_pFxProj       = m_pEffect->GetVariableByName("g_Proj")->AsMatrix();
	m_pFxEyePos     = m_pEffect->GetVariableByName("g_EyePos")->AsVector();
	m_pFxDirLight   = m_pEffect->GetVariableByName("g_DirLight");
	m_pFxPointLight = m_pEffect->GetVariableByName("g_PointLight");
	m_pFxSpotLight  = m_pEffect->GetVariableByName("g_SpotLight");
	m_pFxMaterial   = m_pEffect->GetVariableByName("g_Mat");

	// Done with compiled shader.
	ReleaseCOM(compiledShader);

	D3D11_INPUT_ELEMENT_DESC vertexDesc[] =
	{
		{ "POSITION", 0, DXGI_FORMAT_R32G32B32_FLOAT, 0, 0, D3D11_INPUT_PER_VERTEX_DATA, 0 },
		{ "NORMAL",   0, DXGI_FORMAT_R32G32B32_FLOAT, 0, 12, D3D11_INPUT_PER_VERTEX_DATA, 0 }
	};

	D3DX11_PASS_DESC passDesc;
	m_pEffectTechnique->GetPassByIndex(0)->GetDesc(&passDesc);

	pD3D11Device->CreateInputLayout(vertexDesc, 2, passDesc.pIAInputSignature,
		passDesc.IAInputSignatureSize, &m_pInputLayout);


}

}
