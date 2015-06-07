#include "d3dApp.h"
#include "GeometryGenerator.h"
#include "MathHelper.h"
#include "LightHelper.h"
#include "Waves.h"

struct Vertex
{
	XMFLOAT3 Pos;
	XMFLOAT3 Normal;
};

class WavesApp :public D3DApp
{
	public:
		WavesApp(HINSTANCE hInstance);
		~WavesApp();

		float GetHeight(float x, float z)const;
		XMFLOAT3 GetNormal(float x, float z) const;

		bool Init();
		void Resize();
		void UpdateScene(float dt);
		void Render();

		void OnMouseDown(WPARAM btnState, int x, int y);
		void OnMouseUp(WPARAM btnState, int x, int y);
		void OnMouseMove(WPARAM btnState, int x, int y);

	private:
		void init_buffer();
		void init_fx();
		void init_layout();

	private:
		ID3D11Buffer                * pLandVB;
		ID3D11Buffer                * pLandIB;
		ID3D11Buffer                * pWavesVB;
		ID3D11Buffer                * pWavesIB;

		ID3DX11Effect               *pFX;
		ID3DX11EffectTechnique      *pTech;
		ID3D11InputLayout           *pInputLayout;
		ID3DX11EffectMatrixVariable *pFxWorldViewProj;
		ID3DX11EffectMatrixVariable * mfxWorld;
		ID3DX11EffectMatrixVariable * mfxWorldInvTranspose;
		ID3DX11EffectVectorVariable * mfxEyePosW;
		ID3DX11EffectVariable       * mfxDirLight;
		ID3DX11EffectVariable       * mfxPointLight;
		ID3DX11EffectVariable       * mfxSpotLight;
		ID3DX11EffectVariable       * mfxMaterial;
		ID3D11RasterizerState       * pWireframeRS;

		//Light 
		DirectionalLight mDirLight;
		PointLight mPointLight;
		SpotLight mSpotLight;
		Material  mLandMat;
		Material mWavesMat;
		XMFLOAT3 mEyePosW;

		XMFLOAT4X4 mGridWorld;
		XMFLOAT4X4 mWavesWorld;
		UINT mGridIndexCount;
		Waves mWaves;
		XMFLOAT4X4 mView;
		XMFLOAT4X4 mProj;

		float Theta;
		float Phi;
		float Radius;
		POINT LastMousePos;

};

int WINAPI wWinMain( _In_ HINSTANCE hInstance, _In_opt_ HINSTANCE hPrevInstance, 
					_In_ LPWSTR lpCmdLine, _In_ int nShowCmd )
	{
	// Enable run-time memory check for debug builds.
#if defined(DEBUG) | defined(_DEBUG)
	_CrtSetDbgFlag( _CRTDBG_ALLOC_MEM_DF | _CRTDBG_LEAK_CHECK_DF );
#endif

	//init the window
	WavesApp app(hInstance);
	if ( !app.Init() )
		return 0;

	return  app.Run();
}


WavesApp::WavesApp(HINSTANCE hInstance)
	:D3DApp(hInstance),
	pLandVB(0), 
	pLandIB(0),
	pWavesVB(0), 
	pWavesIB(0), 
	pFX(0),
	pTech(0),
	pInputLayout(0),
	pFxWorldViewProj(0),
	mfxWorld(0), mfxWorldInvTranspose(0), mfxEyePosW(0), 
	mfxDirLight(0), mfxPointLight(0), mfxSpotLight(0), mfxMaterial(0),
	mEyePosW(0.0f, 0.0f, 0.0f),
	pWireframeRS(0),
	Theta(1.5 * MathHelper::Pi), 
	Phi(0.25f * MathHelper::Pi), 
	Radius(200.0f)
{
	WindowTitle = L"Box Demo";
	LastMousePos.x = 0;
	LastMousePos.y = 0;
	XMMATRIX I = XMMatrixIdentity();
	XMMATRIX T = XMMatrixTranslation(0.0f, -2.0f, 0.0f);
	XMStoreFloat4x4(&mGridWorld, I);
	XMStoreFloat4x4(&mWavesWorld, I);
	XMStoreFloat4x4(&mView, I);
	XMStoreFloat4x4(&mProj, I);

	XMMATRIX wavesOffset = XMMatrixTranslation(0.0f, -3.0f, 0.0f);
	XMStoreFloat4x4(&mWavesWorld, wavesOffset);

	// Directional light.
	mDirLight.Ambient  = XMFLOAT4(0.2f, 0.2f, 0.2f, 1.0f);
	mDirLight.Diffuse  = XMFLOAT4(0.5f, 0.5f, 0.5f, 1.0f);
	mDirLight.Specular = XMFLOAT4(0.5f, 0.5f, 0.5f, 1.0f);
	mDirLight.Direction = XMFLOAT3(0.57735f, -0.57735f, 0.57735f);

	// Point light--position is changed every frame to animate in UpdateScene function.
	mPointLight.Ambient  = XMFLOAT4(0.3f, 0.3f, 0.3f, 1.0f);
	mPointLight.Diffuse  = XMFLOAT4(0.7f, 0.7f, 0.7f, 1.0f);
	mPointLight.Specular = XMFLOAT4(0.7f, 0.7f, 0.7f, 1.0f);
	mPointLight.Att      = XMFLOAT3(0.0f, 0.1f, 0.0f);
	mPointLight.Range    = 25.0f;

	// Spot light--position and direction changed every frame to animate in UpdateScene function.
	mSpotLight.Ambient  = XMFLOAT4(0.0f, 0.0f, 0.0f, 1.0f);
	mSpotLight.Diffuse  = XMFLOAT4(1.0f, 1.0f, 0.0f, 1.0f);
	mSpotLight.Specular = XMFLOAT4(1.0f, 1.0f, 1.0f, 1.0f);
	mSpotLight.Att      = XMFLOAT3(1.0f, 0.0f, 0.0f);
	mSpotLight.Spot     = 96.0f;
	mSpotLight.Range    = 10000.0f;

	mLandMat.Ambient  = XMFLOAT4(0.48f, 0.77f, 0.46f, 1.0f);
	mLandMat.Diffuse  = XMFLOAT4(0.48f, 0.77f, 0.46f, 1.0f);
	mLandMat.Specular = XMFLOAT4(0.2f, 0.2f, 0.2f, 16.0f);

	mWavesMat.Ambient  = XMFLOAT4(0.137f, 0.42f, 0.556f, 1.0f);
	mWavesMat.Diffuse  = XMFLOAT4(0.137f, 0.42f, 0.556f, 1.0f);
	mWavesMat.Specular = XMFLOAT4(0.8f, 0.8f, 0.8f, 96.0f);
}


WavesApp::~WavesApp()
{
	ReleaseCOM(pLandVB);
	ReleaseCOM(pLandIB);
	ReleaseCOM(pWavesVB);
	ReleaseCOM(pWavesIB);
	ReleaseCOM(pFX);
	ReleaseCOM(pInputLayout);
	ReleaseCOM(pWireframeRS);
}

bool WavesApp::Init()
{
	if ( !D3DApp::Init() )
		return false;
	mWaves.Init(160, 160, 1.0f, 0.03f, 3.25f, 0.4f);

	init_buffer();
	init_fx();
	init_layout();

	return true;
}

void WavesApp::Resize()
{
	D3DApp::Resize();

	XMMATRIX P = XMMatrixPerspectiveFovLH(0.25f*MathHelper::Pi, AspectRatio(), 1.0f, 1000.0f);
	XMStoreFloat4x4(&mProj, P);
}

void WavesApp::UpdateScene(float dt)
{
	//convert spherical to cartesian coordinates
	float x = Radius * sinf(Phi) * cosf(Theta);
	float z = Radius * sinf(Phi) * sinf(Theta);
	float y = Radius * cosf(Phi);
    mEyePosW = XMFLOAT3(x, y, z);

	// Build the view matrix.
	XMVECTOR pos    = XMVectorSet(x, y, z, 1.0f);
	XMVECTOR target = XMVectorZero();
	XMVECTOR up     = XMVectorSet(0.0f, 1.0f, 0.0f, 0.0f);
	XMMATRIX V = XMMatrixLookAtLH(pos, target, up);
	XMStoreFloat4x4(&mView, V);

	// Every quarter second, generate a random wave.
	static float t_base = 0.0f;
	if( (mTimer.TotalTime() - t_base) >= 0.25f )
	{
		t_base += 0.25f;

		DWORD i = 5 + rand() % (mWaves.RowCount()-10);
		DWORD j = 5 + rand() % (mWaves.ColumnCount()-10);

		float r = MathHelper::RandF(1.0f, 2.0f);

		mWaves.Disturb(i, j, r);
	}

	mWaves.Update(dt);

	// Update the wave vertex buffer with the new solution.
	D3D11_MAPPED_SUBRESOURCE mappedData;
	HR(pDeviceContext->Map(pWavesVB, 0, D3D11_MAP_WRITE_DISCARD, 0, &mappedData));

	Vertex* v = reinterpret_cast<Vertex*>(mappedData.pData);
	for(UINT i = 0; i < mWaves.VertexCount(); ++i)
	{
		v[i].Pos    = mWaves[i];
		v[i].Normal = mWaves.Normal(i);
	}

	pDeviceContext->Unmap(pWavesVB, 0);

	// Circle light over the land surface.
	mPointLight.Position.x = 70.0f*cosf( 0.2f*mTimer.TotalTime() );
	mPointLight.Position.z = 70.0f*sinf( 0.2f*mTimer.TotalTime() );
	mPointLight.Position.y = MathHelper::Max(GetHeight(mPointLight.Position.x, 
		                                               mPointLight.Position.z), -3.0f) + 10.0f;


	// The spotlight takes on the camera position and is aimed in the
	// same direction the camera is looking.  In this way, it looks
	// like we are holding a flashlight.
	mSpotLight.Position = mEyePosW;
	XMStoreFloat3(&mSpotLight.Direction, XMVector3Normalize(target - pos));
}

void WavesApp::Render()
{
	assert(pDeviceContext);
	assert(pSwapChain);

	pDeviceContext->ClearRenderTargetView(pRenderTargetView, reinterpret_cast<const float*>(&Colors::Silver));
	pDeviceContext->ClearDepthStencilView(pDepthStencilView, D3D11_CLEAR_DEPTH|D3D11_CLEAR_STENCIL, 1.0f, 0);
	pDeviceContext->IASetInputLayout(pInputLayout);
	pDeviceContext->IASetPrimitiveTopology(D3D11_PRIMITIVE_TOPOLOGY_TRIANGLELIST);

	UINT stride = sizeof(Vertex);
	UINT offset = 0;
	XMMATRIX view  = XMLoadFloat4x4(&mView);
	XMMATRIX proj  = XMLoadFloat4x4(&mProj);
	XMMATRIX viewProj = view*proj;

	// Set per frame constants.
	mfxDirLight->SetRawValue(&mDirLight, 0, sizeof(mDirLight));
	mfxPointLight->SetRawValue(&mPointLight, 0, sizeof(mPointLight));
	mfxSpotLight->SetRawValue(&mSpotLight, 0, sizeof(mSpotLight));
	mfxEyePosW->SetRawValue(&mEyePosW, 0, sizeof(mEyePosW));

	D3DX11_TECHNIQUE_DESC techDesc;
	pTech->GetDesc( &techDesc );

	for(UINT p = 0; p < techDesc.Passes; ++p)
	{
		// Draw the land.
		pDeviceContext->IASetVertexBuffers(0, 1, &pLandVB, &stride, &offset);
		pDeviceContext->IASetIndexBuffer(pLandIB, DXGI_FORMAT_R32_UINT, 0);

		XMMATRIX world = XMLoadFloat4x4(&mGridWorld);
		XMMATRIX worldInvTranspose = MathHelper::InverseTranspose(world);
		XMMATRIX worldViewProj = world*view*proj;

		mfxWorld->SetMatrix(reinterpret_cast<float*>(&world));
		mfxWorldInvTranspose->SetMatrix(reinterpret_cast<float*>(&worldInvTranspose));
		mfxMaterial->SetRawValue(&mLandMat, 0, sizeof(mLandMat));
		pFxWorldViewProj->SetMatrix(reinterpret_cast<float*>(&worldViewProj));
		
		pTech->GetPassByIndex(p)->Apply(0, pDeviceContext);
		pDeviceContext->DrawIndexed(mGridIndexCount, 0, 0);

		// Draw the waves
		pDeviceContext->RSSetState(pWireframeRS);

		pDeviceContext->IASetVertexBuffers(0, 1, &pWavesVB, &stride, &offset);
		pDeviceContext->IASetIndexBuffer(pWavesIB, DXGI_FORMAT_R32_UINT, 0);

		// Set per object constants.
		world = XMLoadFloat4x4(&mWavesWorld);
		worldInvTranspose = MathHelper::InverseTranspose(world);
		worldViewProj = world*view*proj;

		mfxWorld->SetMatrix(reinterpret_cast<float*>(&world));
		mfxWorldInvTranspose->SetMatrix(reinterpret_cast<float*>(&worldInvTranspose));
		pFxWorldViewProj->SetMatrix(reinterpret_cast<float*>(&worldViewProj));
		mfxMaterial->SetRawValue(&mWavesMat, 0, sizeof(mWavesMat));

		pTech->GetPassByIndex(p)->Apply(0, pDeviceContext);
		pDeviceContext->DrawIndexed(3*mWaves.TriangleCount(), 0, 0);
	}

	HR(pSwapChain->Present(0, 0));
}


void WavesApp::OnMouseDown(WPARAM btnState, int x, int y)
{
	LastMousePos.x = x;
	LastMousePos.y = y;
	SetCapture(hWnd);
}

void WavesApp::OnMouseUp(WPARAM btnState, int x, int y)
{
	ReleaseCapture();
}

void WavesApp::OnMouseMove(WPARAM btnState, int x, int y)
{
	if ( (btnState & MK_LBUTTON) != 0)
	{
		//make each pixel correspond to a quarter of a degree;
		float dx = XMConvertToRadians(0.25f * static_cast<float>(x - LastMousePos.x));
		float dy = XMConvertToRadians(0.25f * static_cast<float>(y - LastMousePos.y));

		//update angles based on input to orbit camera around box
		Theta += dx;
		Phi += dy;

		//restrict the angle phi
		Phi = MathHelper::Clamp(Phi, 0.1f, MathHelper::Pi-0.1f);
	}
	else if ( (btnState * MK_RBUTTON) != 0)
	{
		float dx = 0.005f * static_cast<float>(x - LastMousePos.x);
		float dy = 0.005f * static_cast<float>(y - LastMousePos.y);

		Radius += dx - dy;
		Radius = MathHelper::Clamp(Radius, 100.0f, 300.0f);
	}
	LastMousePos.x = x;
	LastMousePos.y = y;
}

float WavesApp::GetHeight(float x, float z) const
{
	return 0.3f*( z*sinf(0.1f*x) + x*cosf(0.1f*z) );
}

XMFLOAT3 WavesApp::GetNormal(float x, float z)const
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

void WavesApp::init_buffer()
{
	GeometryGenerator::MeshData grid;
	GeometryGenerator geoGen;

	geoGen.CreateGrid(160.0f, 160.0f, 50, 50, grid);

	mGridIndexCount = grid.Indices.size();

	//
	// Extract the vertex elements we are interested and apply the height function to
	// each vertex.  In addition, color the vertices based on their height so we have
	// sandy looking beaches, grassy low hills, and snow mountain peaks.
	//

	std::vector<Vertex> vertices(grid.Vertices.size());
	for(size_t i = 0; i < grid.Vertices.size(); ++i)
	{
		XMFLOAT3 p = grid.Vertices[i].Position;

		p.y = GetHeight(p.x, p.z);
		vertices[i].Pos   = p;
		vertices[i].Pos    = p;
	    vertices[i].Normal = GetNormal(p.x, p.z);
	 }

	D3D11_BUFFER_DESC vbd;
	vbd.Usage = D3D11_USAGE_IMMUTABLE;
	vbd.ByteWidth = sizeof(Vertex) * grid.Vertices.size();
	vbd.BindFlags = D3D11_BIND_VERTEX_BUFFER;
	vbd.CPUAccessFlags = 0;
	vbd.MiscFlags = 0;
	D3D11_SUBRESOURCE_DATA vinitData;
	vinitData.pSysMem = &vertices[0];
	HR(pDevice->CreateBuffer(&vbd, &vinitData, &pLandVB));

	// Pack the indices of all the meshes into one index buffer.
	D3D11_BUFFER_DESC ibd;
	ibd.Usage = D3D11_USAGE_IMMUTABLE;
	ibd.ByteWidth = sizeof(UINT) * mGridIndexCount;
	ibd.BindFlags = D3D11_BIND_INDEX_BUFFER;
	ibd.CPUAccessFlags = 0;
	ibd.MiscFlags = 0;
	D3D11_SUBRESOURCE_DATA iinitData;
	iinitData.pSysMem = &grid.Indices[0];
	HR(pDevice->CreateBuffer(&ibd, &iinitData, &pLandIB));

	// Create the vertex buffer.  Note that we allocate space only, as
	// we will be updating the data every time step of the simulation.

	//D3D11_BUFFER_DESC vbd;
	vbd.Usage = D3D11_USAGE_DYNAMIC;
	vbd.ByteWidth = sizeof(Vertex) * mWaves.VertexCount();
	vbd.BindFlags = D3D11_BIND_VERTEX_BUFFER;
	vbd.CPUAccessFlags = D3D11_CPU_ACCESS_WRITE;
	vbd.MiscFlags = 0;
	HR(pDevice->CreateBuffer(&vbd, 0, &pWavesVB));


	// Create the index buffer.  The index buffer is fixed, so we only 
	// need to create and set once.

	std::vector<UINT> indices(3*mWaves.TriangleCount()); // 3 indices per face

	// Iterate over each quad.
	UINT m = mWaves.RowCount();
	UINT n = mWaves.ColumnCount();
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

	//D3D11_BUFFER_DESC ibd;
	ibd.Usage = D3D11_USAGE_IMMUTABLE;
	ibd.ByteWidth = sizeof(UINT) * indices.size();
	ibd.BindFlags = D3D11_BIND_INDEX_BUFFER;
	ibd.CPUAccessFlags = 0;
	ibd.MiscFlags = 0;
	//D3D11_SUBRESOURCE_DATA iinitData;
	iinitData.pSysMem = &indices[0];
	HR(pDevice->CreateBuffer(&ibd, &iinitData, &pWavesIB));

} 

void WavesApp::init_fx()
{

	std::ifstream fin("Lighting.fxo", std::ios::binary);

	fin.seekg(0, std::ios_base::end);
	int size = (int)fin.tellg();
	fin.seekg(0, std::ios_base::beg);
	std::vector<char> compiledShader(size);

	fin.read(&compiledShader[0], size);
	fin.close();

	HR(D3DX11CreateEffectFromMemory(&compiledShader[0], size, 
		0, pDevice, &pFX));

	pTech                = pFX->GetTechniqueByName("LightTech");
	pFxWorldViewProj     = pFX->GetVariableByName("gWorldViewProj")->AsMatrix();
	mfxWorld             = pFX->GetVariableByName("gWorld")->AsMatrix();
	mfxWorldInvTranspose = pFX->GetVariableByName("gWorldInvTranspose")->AsMatrix();
	mfxEyePosW           = pFX->GetVariableByName("gEyePosW")->AsVector();
	mfxDirLight          = pFX->GetVariableByName("gDirLight");
	mfxPointLight        = pFX->GetVariableByName("gPointLight");
	mfxSpotLight         = pFX->GetVariableByName("gSpotLight");
	mfxMaterial          = pFX->GetVariableByName("gMaterial");
}

void WavesApp::init_layout()
{	// Create the vertex input layout.
	D3D11_INPUT_ELEMENT_DESC vertexDesc[] =
	{
		{"POSITION", 0, DXGI_FORMAT_R32G32B32_FLOAT, 0, 0, D3D11_INPUT_PER_VERTEX_DATA, 0},
		{"NORMAL",    0, DXGI_FORMAT_R32G32B32_FLOAT, 0, 12, D3D11_INPUT_PER_VERTEX_DATA, 0}
	};

	// Create the input layout
	D3DX11_PASS_DESC passDesc;
	pTech->GetPassByIndex(0)->GetDesc(&passDesc);
	HR(pDevice->CreateInputLayout(vertexDesc, 2, passDesc.pIAInputSignature, 
		passDesc.IAInputSignatureSize, &pInputLayout));
}