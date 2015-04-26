
#include "d3dApp.h"
#include "d3dx11effect.h"
#include "MathHelper.h"

struct Vertex
{
   XMFLOAT3 Pos;
   XMFLOAT4 Color;
};

class BoxApp :public D3DApp
{
	public:
		BoxApp(HINSTANCE hInstance);
		~BoxApp();

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
	BoxApp app(hInstance);
	if ( !app.Init() )
		return 0;

	return  app.Run();
}


BoxApp::BoxApp(HINSTANCE hInstance)
	:D3DApp(hInstance), pVB(0), pIB(0), pFX(0), pTech(0),
	pInputLayout(0), pFxWorldViewProj(0),
	Theta(1.5 * MathHelper::Pi), Phi(0.25f * MathHelper::Pi), Radius(5.0f)
{
     WindowTitle = L"Box Demo";
	 LastMousePos.x = 0;
	 LastMousePos.y = 0;
	 XMMATRIX I = XMMatrixIdentity();
	 XMStoreFloat4x4(&world, I);
	 XMStoreFloat4x4(&view, I);
	 XMStoreFloat4x4(&proj, I);

}


BoxApp::~BoxApp()
{
   ReleaseCOM(pVB);
   ReleaseCOM(pIB);
   ReleaseCOM(pFX);
   ReleaseCOM(pInputLayout);
}

bool BoxApp::Init()
{
	if ( !D3DApp::Init() )
		return false;

	init_buffer();
	init_fx();
	init_layout();
	return true;
}

void BoxApp::Resize()
{
	D3DApp::Resize();

	XMMATRIX P = XMMatrixPerspectiveFovLH(0.25f * MathHelper::Pi, AspectRatio(), 1.0f, 1000.0f);
	XMStoreFloat4x4(&proj, P);
}

void BoxApp::UpdateScene(float dt)
{
     //convert spherical to cartesian coordinates
     float x = Radius * sinf(Phi) * cosf(Theta);
	 float z = Radius * sinf(Phi) * sinf(Theta);
	 float y = Radius * cosf(Phi);

	 //set the view  matrix
	 XMVECTOR pos = XMVectorSet(x, y, z, 1.0f);
	 XMVECTOR target = XMVectorZero();
	 XMVECTOR up = XMVectorSet(0.0f, 1.0f, 0.0f, 0.0f);
	 XMMATRIX v = XMMatrixLookAtLH(pos, target, up);
	 XMStoreFloat4x4(&view, v);

}

void BoxApp::Render()
{
	assert(pDeviceContext);
	assert(pSwapChain);

	pDeviceContext->ClearRenderTargetView(pRenderTargetView, reinterpret_cast<const float*>(&Colors::Blue));
	pDeviceContext->ClearDepthStencilView(pDepthStencilView, D3D11_CLEAR_DEPTH|D3D11_CLEAR_STENCIL, 1.0f, 0);
	pDeviceContext->IASetInputLayout(pInputLayout);
	pDeviceContext->IASetPrimitiveTopology(D3D11_PRIMITIVE_TOPOLOGY_TRIANGLELIST);

	UINT stride = sizeof(Vertex);
	UINT offset = 0;
	pDeviceContext->IASetVertexBuffers(0, 1, &pVB, &stride, &offset);
	pDeviceContext->IASetIndexBuffer(pIB, DXGI_FORMAT_R32_UINT, 0);
    
	// Set constants
	XMMATRIX w = XMLoadFloat4x4(&world);
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
		pDeviceContext->DrawIndexed(36, 0, 0);
	}

	HR(pSwapChain->Present(0, 0));
}


void BoxApp::OnMouseDown(WPARAM btnState, int x, int y)
{
    LastMousePos.x = x;
	LastMousePos.y = y;
	SetCapture(hWnd);
}

void BoxApp::OnMouseUp(WPARAM btnState, int x, int y)
{
    ReleaseCapture();
}

void BoxApp::OnMouseMove(WPARAM btnState, int x, int y)
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
	   Radius = MathHelper::Clamp(Radius, 3.0f, 15.0f);
	}
	LastMousePos.x = x;
	LastMousePos.y = y;
}

void BoxApp::init_buffer()
{
    Vertex VertexData[] =
	{
		{ XMFLOAT3(-1.0f, -1.0f, -1.0f), (const float*)&Colors::White   },
		{ XMFLOAT3(-1.0f, +1.0f, -1.0f), (const float*)&Colors::Black   },
		{ XMFLOAT3(+1.0f, +1.0f, -1.0f), (const float*)&Colors::Red     },
		{ XMFLOAT3(+1.0f, -1.0f, -1.0f), (const float*)&Colors::Green   },
		{ XMFLOAT3(-1.0f, -1.0f, +1.0f), (const float*)&Colors::Blue    },
		{ XMFLOAT3(-1.0f, +1.0f, +1.0f), (const float*)&Colors::Yellow  },
		{ XMFLOAT3(+1.0f, +1.0f, +1.0f), (const float*)&Colors::Cyan    },
		{ XMFLOAT3(+1.0f, -1.0f, +1.0f), (const float*)&Colors::Magenta }
	};

     D3D11_BUFFER_DESC vbDesc;
	 vbDesc.Usage = D3D11_USAGE_IMMUTABLE;
	 vbDesc.ByteWidth = sizeof(Vertex) * 8;
	 vbDesc.BindFlags = D3D11_BIND_VERTEX_BUFFER;
	 vbDesc.CPUAccessFlags = 0;
	 vbDesc.MiscFlags = 0;
	 vbDesc.StructureByteStride = 0;
	 D3D11_SUBRESOURCE_DATA vinitData;
	 vinitData.pSysMem = VertexData;
	 HR( pDevice->CreateBuffer(&vbDesc, &vinitData, &pVB) );
	 // Create the index buffer

	 UINT indices[] = 
	{
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

	 D3D11_BUFFER_DESC ibd;
	 ibd.Usage = D3D11_USAGE_IMMUTABLE;
	 ibd.ByteWidth = sizeof(UINT) * 36;
	 ibd.BindFlags = D3D11_BIND_INDEX_BUFFER;
	 ibd.CPUAccessFlags = 0;
	 ibd.MiscFlags = 0;
	 ibd.StructureByteStride = 0;
	 D3D11_SUBRESOURCE_DATA iinitData;
	 iinitData.pSysMem = indices;
	 HR(pDevice->CreateBuffer(&ibd, &iinitData, &pIB));
} 

void BoxApp::init_fx()
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

void BoxApp::init_layout()
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