#include "d3dApp.h"
#include "d3dx11effect.h"
#include "MathHelper.h"

struct Vertex
{
	XMFLOAT3 Pos;
	XMFLOAT4 Color;
};

class SkullApp :public D3DApp
{
	public:
		SkullApp(HINSTANCE hInstance);
		~SkullApp();

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
		ID3D11Buffer *pVB;
		ID3D11Buffer *pIB;
		ID3DX11Effect *pFX;
		ID3DX11EffectTechnique *pTech;
		ID3D11InputLayout *pInputLayout;
		ID3DX11EffectMatrixVariable *pFxWorldViewProj;

		//mvp matrix
		XMFLOAT4X4 world;
		XMFLOAT4X4 view;
		XMFLOAT4X4 proj;

		float Theta;
		float Phi;
		float Radius;
		UINT SkullIndexCount;
		POINT LastMousePos;
		ID3D11RasterizerState* pWireframeRS;
};

int WINAPI wWinMain( _In_ HINSTANCE hInstance, _In_opt_ HINSTANCE hPrevInstance, 
					_In_ LPWSTR lpCmdLine, _In_ int nShowCmd )
{
	// Enable run-time memory check for debug builds.
#if defined(DEBUG) | defined(_DEBUG)
	_CrtSetDbgFlag( _CRTDBG_ALLOC_MEM_DF | _CRTDBG_LEAK_CHECK_DF );
#endif

	//init the window
	SkullApp app(hInstance);
	if ( !app.Init() )
		return 0;

	return  app.Run();
}


SkullApp::SkullApp(HINSTANCE hInstance)
	:D3DApp(hInstance), pVB(0), pIB(0), pFX(0), pTech(0),
	pInputLayout(0), pFxWorldViewProj(0), pWireframeRS(0),
	Theta(1.5 * MathHelper::Pi), Phi(0.25f * MathHelper::Pi), Radius(20.0f)
{
	WindowTitle = L"Box Demo";
	LastMousePos.x = 0;
	LastMousePos.y = 0;
	XMMATRIX I = XMMatrixIdentity();
	XMMATRIX T = XMMatrixTranslation(0.0f, -2.0f, 0.0f);
	XMStoreFloat4x4(&world, T);
	XMStoreFloat4x4(&view, I);
	XMStoreFloat4x4(&proj, I);

}


SkullApp::~SkullApp()
{
	ReleaseCOM(pVB);
	ReleaseCOM(pIB);
	ReleaseCOM(pFX);
	ReleaseCOM(pInputLayout);
}

bool SkullApp::Init()
{
	if ( !D3DApp::Init() )
		return false;

	init_buffer();
	init_fx();
	init_layout();
	D3D11_RASTERIZER_DESC wireframeDesc;
	ZeroMemory(&wireframeDesc, sizeof(D3D11_RASTERIZER_DESC));
	wireframeDesc.FillMode = D3D11_FILL_WIREFRAME;
	wireframeDesc.CullMode = D3D11_CULL_BACK;
	wireframeDesc.FrontCounterClockwise = false;
	wireframeDesc.DepthClipEnable = true;

	HR(pDevice->CreateRasterizerState(&wireframeDesc, &pWireframeRS));
	return true;
}

void SkullApp::Resize()
{
	D3DApp::Resize();

	XMMATRIX P = XMMatrixPerspectiveFovLH(0.25f * MathHelper::Pi, AspectRatio(), 1.0f, 1000.0f);
	XMStoreFloat4x4(&proj, P);
}

void SkullApp::UpdateScene(float dt)
{
	//convert spherical to cartesian coordinates
	float x = Radius * sinf(Phi) * cosf(Theta);
	float z = Radius * sinf(Phi) * sinf(Theta);
	float y = Radius * cosf(Phi);

	//set the view matrix, we change camera pos when the mouse move
	XMVECTOR pos = XMVectorSet(x, y, z, 1.0f);
	XMVECTOR target = XMVectorZero();
	XMVECTOR up = XMVectorSet(0.0f, 1.0f, 0.0f, 0.0f);
	XMMATRIX v = XMMatrixLookAtLH(pos, target, up);
	XMStoreFloat4x4(&view, v);

}

void SkullApp::Render()
{
	assert(pDeviceContext);
	assert(pSwapChain);

	pDeviceContext->ClearRenderTargetView(pRenderTargetView, reinterpret_cast<const float*>(&Colors::Silver));
	pDeviceContext->ClearDepthStencilView(pDepthStencilView, D3D11_CLEAR_DEPTH|D3D11_CLEAR_STENCIL, 1.0f, 0);
	pDeviceContext->IASetInputLayout(pInputLayout);
	pDeviceContext->IASetPrimitiveTopology(D3D11_PRIMITIVE_TOPOLOGY_TRIANGLELIST);

	UINT stride = sizeof(Vertex);
	UINT offset = 0;
	pDeviceContext->IASetVertexBuffers(0, 1, &pVB, &stride, &offset);
	pDeviceContext->IASetIndexBuffer(pIB, DXGI_FORMAT_R32_UINT, 0);
	pDeviceContext->RSSetState(pWireframeRS);
	// Set constants
	XMMATRIX w  = XMLoadFloat4x4(&world);
	XMMATRIX v  = XMLoadFloat4x4(&view);
	XMMATRIX p  = XMLoadFloat4x4(&proj);
	XMMATRIX worldViewProj = w * v * p;

	pFxWorldViewProj->SetMatrix(reinterpret_cast<float*>(&worldViewProj));

	D3DX11_TECHNIQUE_DESC techDesc;
	pTech->GetDesc( &techDesc );

	for(UINT p = 0; p < techDesc.Passes; ++p)
	{
		pTech->GetPassByIndex(p)->Apply(0, pDeviceContext);

		// 36 indices for the box.
		pDeviceContext->DrawIndexed(SkullIndexCount, 0, 0);
}

	HR(pSwapChain->Present(0, 0));
}


void SkullApp::OnMouseDown(WPARAM btnState, int x, int y)
{
	LastMousePos.x = x;
	LastMousePos.y = y;
	SetCapture(hWnd);
}

void SkullApp::OnMouseUp(WPARAM btnState, int x, int y)
{
	ReleaseCapture();
}

void SkullApp::OnMouseMove(WPARAM btnState, int x, int y)
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
		Radius = MathHelper::Clamp(Radius, 5.0f, 50.0f);
}
	LastMousePos.x = x;
	LastMousePos.y = y;
}

void SkullApp::init_buffer()
{
	std::ifstream fin("../common/media/skull.txt");

	if(!fin)
    {
		MessageBox(0, L"Models/skull.txt not found.", 0, 0);
		return;
    }

	UINT vcount = 0;
	UINT tcount = 0;
	std::string ignore;

	fin >> ignore >> vcount;
	fin >> ignore >> tcount;
	fin >> ignore >> ignore >> ignore >> ignore;

	float nx, ny, nz;
	XMFLOAT4 black(0.0f, 0.0f, 0.0f, 1.0f);

	std::vector<Vertex> vertices(vcount);
	for(UINT i = 0; i < vcount; ++i)
    {
		fin >> vertices[i].Pos.x >> vertices[i].Pos.y >> vertices[i].Pos.z;
		vertices[i].Color = black;
		// Normal not used in this demo.
		fin >> nx >> ny >> nz;
    }

	fin >> ignore;
	fin >> ignore;
	fin >> ignore;

	SkullIndexCount = 3 * tcount;
	std::vector<UINT> indices(SkullIndexCount);
	for(UINT i = 0; i < tcount; ++i)
    {
		fin >> indices[i*3+0] >> indices[i*3+1] >> indices[i*3+2];
    }

	fin.close();

	D3D11_BUFFER_DESC vbd;
	vbd.Usage = D3D11_USAGE_IMMUTABLE;
	vbd.ByteWidth = sizeof(Vertex) * vcount;
	vbd.BindFlags = D3D11_BIND_VERTEX_BUFFER;
	vbd.CPUAccessFlags = 0;
	vbd.MiscFlags = 0;
	D3D11_SUBRESOURCE_DATA vinitData;
	vinitData.pSysMem = &vertices[0];
	HR(pDevice->CreateBuffer(&vbd, &vinitData, &pVB));

	//
	// Pack the indices of all the meshes into one index buffer.
	//

	D3D11_BUFFER_DESC ibd;
	ibd.Usage = D3D11_USAGE_IMMUTABLE;
	ibd.ByteWidth = sizeof(UINT) * SkullIndexCount;
	ibd.BindFlags = D3D11_BIND_INDEX_BUFFER;
	ibd.CPUAccessFlags = 0;
	ibd.MiscFlags = 0;
	D3D11_SUBRESOURCE_DATA iinitData;
	iinitData.pSysMem = &indices[0];
	HR(pDevice->CreateBuffer(&ibd, &iinitData, &pIB));

} 

void SkullApp::init_fx()
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
	if( compilationMsgs != 0 )
	{
		MessageBoxA(0, (char*)compilationMsgs->GetBufferPointer(), 0, 0);
		ReleaseCOM(compilationMsgs);
    }

	// Even if there are no compilationMsgs, check to make sure there were no other errors.
	if(FAILED(hr))
	{
		DXTrace(__FILE__, (DWORD)__LINE__, hr, L"D3DX11CompileFromFile", true);
    }

	HR(D3DX11CreateEffectFromMemory(compiledShader->GetBufferPointer(), 
		compiledShader->GetBufferSize(), 
		0, pDevice, &pFX));
	// Done with compiled shader.
	ReleaseCOM(compiledShader);
	pTech    = pFX->GetTechniqueByName("ColorTech");
	pFxWorldViewProj = pFX->GetVariableByName("gWorldViewProj")->AsMatrix();
}

void SkullApp::init_layout()
{	// Create the vertex input layout.
	D3D11_INPUT_ELEMENT_DESC vertexDesc[] =
	{
		{"POSITION", 0, DXGI_FORMAT_R32G32B32_FLOAT, 0, 0, D3D11_INPUT_PER_VERTEX_DATA, 0},
		{"COLOR",    0, DXGI_FORMAT_R32G32B32A32_FLOAT, 0, 12, D3D11_INPUT_PER_VERTEX_DATA, 0}
    };

	// Create the input layout
	D3DX11_PASS_DESC passDesc;
	pTech->GetPassByIndex(0)->GetDesc(&passDesc);
	HR(pDevice->CreateInputLayout(vertexDesc, 2, passDesc.pIAInputSignature, 
		passDesc.IAInputSignatureSize, &pInputLayout));

}