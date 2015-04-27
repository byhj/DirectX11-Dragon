#include "d3dApp.h"
#include "d3dx11effect.h"
#include "GeometryGenerator.h"
#include "MathHelper.h"

struct Vertex
{
   XMFLOAT3 Pos;
   XMFLOAT3 Color;
};

class ShapesApp: public D3DApp
{
public:
	ShapesApp(HINSTANCE hInstance);
	~ShapesApp();

	bool Init();
	void Resize();
	void UpdateScene(float dt);
	void Render();

	void OnMouseDown(WPARAM btnState, int x, int y);
	void OnMouseMove(WPARAM btnState, int x, int y);
	void OnMouseUp(WPARAM btnState, int x, int y);

private:
	void init_buffer();
	void init_fx();
	void init_layout();

private:
	ID3D11Buffer *pVB;
	ID3D11Buffer *pIB;

	ID3DX11Effect *pFX;
	ID3DX11EffectTechnique *pTech;
	ID3DX11EffectMatrixVariable *pFxWorldViewProj;
	ID3D11InputLayout *pInputLayout;
	ID3D11RasterizerState *pWireFrameRS;

    XMFLOAT4X4 SphereWorld[10];
	XMFLOAT4X4 CylWorld[10];
	XMFLOAT4X4 BoxWorld;
	XMFLOAT4X4 GridWorld;
	XMFLOAT4X4 CenterSphere;

	XMFLOAT4X4 View;
	XMFLOAT4X4 Proj;

	int BoxVertexOffset;
	int GridVertexOffset;
	int SphereVertexOffset;
	int CylinderVertexOffset;

	UINT BoxIndexOffset;
	UINT GridIndexOffset;
	UINT SphereIndexOffset;
	UINT CylinderIndexOffset;

	UINT BoxIndexCount;
	UINT GridIndexCount;
	UINT SphereIndexCount;
	UINT CylinderIndexCount;

	float Theta;
	float Phi;
	float Radius;

	POINT LastMousePos;
};

int WINAPI WinMain(HINSTANCE hInstance, HINSTANCE prevInstance,
				   PSTR cmdLine, int showCmd)
{
	// Enable run-time memory check for debug builds.
#if defined(DEBUG) | defined(_DEBUG)
	_CrtSetDbgFlag( _CRTDBG_ALLOC_MEM_DF | _CRTDBG_LEAK_CHECK_DF );
#endif

	ShapesApp app(hInstance);

	if( !app.Init() )
		return 0;

	return app.Run();
}


ShapesApp::ShapesApp(HINSTANCE hInstance)
	:D3DApp(hInstance), pVB(0), pIB(0), pFX(0), pTech(0),
	pInputLayout(0), pFxWorldViewProj(0), pWireFrameRS(0),
	Theta(1.5 * MathHelper::Pi), Phi(0.25f * MathHelper::Pi), Radius(15.0f)
{
	WindowTitle = L"Shapes Demo";
	LastMousePos.x = 0;
	LastMousePos.y = 0;
	XMMATRIX I = XMMatrixIdentity();

	XMStoreFloat4x4(&GridWorld, I);
	XMStoreFloat4x4(&View, I);
	XMStoreFloat4x4(&Proj, I);

	XMMATRIX boxScale = XMMatrixScaling(2.0f, 1.0f, 2.0f);
	XMMATRIX boxOffset = XMMatrixTranslation(0.0f, 0.5f, 0.0f);
	XMStoreFloat4x4(&BoxWorld, XMMatrixMultiply(boxScale, boxOffset));

	XMMATRIX centerSphereScale = XMMatrixScaling(2.0f, 2.0f, 2.0f);
	XMMATRIX centerSphereOffset = XMMatrixTranslation(0.0f, 2.0f, 0.0f);
	XMStoreFloat4x4(&CenterSphere, XMMatrixMultiply(centerSphereScale, centerSphereOffset));

	for(int i = 0; i < 5; ++i)
	{
		XMStoreFloat4x4(&CylWorld[i*2+0], XMMatrixTranslation(-5.0f, 1.5f, -10.0f + i*5.0f));
		XMStoreFloat4x4(&CylWorld[i*2+1], XMMatrixTranslation(+5.0f, 1.5f, -10.0f + i*5.0f));

		XMStoreFloat4x4(&SphereWorld[i*2+0], XMMatrixTranslation(-5.0f, 3.5f, -10.0f + i*5.0f));
		XMStoreFloat4x4(&SphereWorld[i*2+1], XMMatrixTranslation(+5.0f, 3.5f, -10.0f + i*5.0f));
	}

}


ShapesApp::~ShapesApp()
{
	ReleaseCOM(pVB);
	ReleaseCOM(pIB);
	ReleaseCOM(pFX);
	ReleaseCOM(pInputLayout);
	ReleaseCOM(pWireFrameRS);
}

bool ShapesApp::Init()
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
	HR(pDevice->CreateRasterizerState(&wireframeDesc, &pWireFrameRS));

	return true;
}

void ShapesApp::Resize()
{
	D3DApp::Resize();

	XMMATRIX P = XMMatrixPerspectiveFovLH(0.25f * MathHelper::Pi, AspectRatio(), 1.0f, 1000.0f);
	XMStoreFloat4x4(&Proj, P);
}

void ShapesApp::UpdateScene(float dt)
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
	XMStoreFloat4x4(&View, v);

}

void ShapesApp::Render()
{
	assert(pDeviceContext);
	assert(pSwapChain);

	pDeviceContext->ClearRenderTargetView(pRenderTargetView, reinterpret_cast<const float*>(&Colors::Blue));
	pDeviceContext->ClearDepthStencilView(pDepthStencilView, D3D11_CLEAR_DEPTH|D3D11_CLEAR_STENCIL, 1.0f, 0);
	pDeviceContext->IASetInputLayout(pInputLayout);
	pDeviceContext->IASetPrimitiveTopology(D3D11_PRIMITIVE_TOPOLOGY_TRIANGLELIST);
	pDeviceContext->RSSetState(pWireFrameRS);

	UINT stride = sizeof(Vertex);
	UINT offset = 0;
	pDeviceContext->IASetVertexBuffers(0, 1, &pVB, &stride, &offset);
	pDeviceContext->IASetIndexBuffer(pIB, DXGI_FORMAT_R32_UINT, 0);

	XMMATRIX view  = XMLoadFloat4x4(&View);
	XMMATRIX proj  = XMLoadFloat4x4(&Proj);
	XMMATRIX viewProj = view*proj;

	D3DX11_TECHNIQUE_DESC techDesc;
	pTech->GetDesc( &techDesc );
	for(UINT p = 0; p < techDesc.Passes; ++p)
	{
		// Draw the grid.
		XMMATRIX world = XMLoadFloat4x4(&GridWorld);
		pFxWorldViewProj->SetMatrix(reinterpret_cast<float*>(&(world*viewProj)));
		pTech->GetPassByIndex(p)->Apply(0, pDeviceContext);
		pDeviceContext->DrawIndexed(GridIndexCount, GridIndexOffset, GridVertexOffset);

		// Draw the box.
		world = XMLoadFloat4x4(&BoxWorld);
		pFxWorldViewProj->SetMatrix(reinterpret_cast<float*>(&(world*viewProj)));
		pTech->GetPassByIndex(p)->Apply(0, pDeviceContext);
		pDeviceContext->DrawIndexed(BoxIndexCount, BoxIndexOffset, BoxVertexOffset);

		// Draw center sphere.
		world = XMLoadFloat4x4(&CenterSphere);
		pFxWorldViewProj->SetMatrix(reinterpret_cast<float*>(&(world*viewProj)));
		pTech->GetPassByIndex(p)->Apply(0, pDeviceContext);
		pDeviceContext->DrawIndexed(SphereIndexCount, SphereIndexOffset, SphereVertexOffset);

		// Draw the cylinders.
		for(int i = 0; i < 10; ++i)
		{
			world = XMLoadFloat4x4(&CylWorld[i]);
			pFxWorldViewProj->SetMatrix(reinterpret_cast<float*>(&(world*viewProj)));
			pTech->GetPassByIndex(p)->Apply(0, pDeviceContext);
			pDeviceContext->DrawIndexed(CylinderIndexCount, CylinderIndexOffset, CylinderVertexOffset);
		}

		// Draw the spheres.
		for(int i = 0; i < 10; ++i)
		{
			world = XMLoadFloat4x4(&SphereWorld[i]);
			pFxWorldViewProj->SetMatrix(reinterpret_cast<float*>(&(world*viewProj)));
			pTech->GetPassByIndex(p)->Apply(0, pDeviceContext);
			pDeviceContext->DrawIndexed(SphereIndexCount, SphereIndexOffset, SphereVertexOffset);
		}
	}

	HR(pSwapChain->Present(0, 0));
}


void ShapesApp::OnMouseDown(WPARAM btnState, int x, int y)
{
	LastMousePos.x = x;
	LastMousePos.y = y;
	SetCapture(hWnd);
}

void ShapesApp::OnMouseUp(WPARAM btnState, int x, int y)
{
	ReleaseCapture();
}

void ShapesApp::OnMouseMove(WPARAM btnState, int x, int y)
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
		float dx = 0.2f * static_cast<float>(x - LastMousePos.x);
		float dy = 0.2f * static_cast<float>(y - LastMousePos.y);

		Radius += dx - dy;
		Radius = MathHelper::Clamp(Radius, 3.0f, 200.0f);
	}
	LastMousePos.x = x;
	LastMousePos.y = y;
}

void ShapesApp::init_buffer()
{
	GeometryGenerator::MeshData box;
	GeometryGenerator::MeshData grid;
	GeometryGenerator::MeshData sphere;
	GeometryGenerator::MeshData cylinder;

	GeometryGenerator geoGen;
	geoGen.CreateBox(1.0f, 1.0f, 1.0f, box);
	geoGen.CreateGrid(20.0f, 30.0f, 60, 40, grid);
	geoGen.CreateSphere(0.5f, 20, 20, sphere);
	//geoGen.CreateGeosphere(0.5f, 2, sphere);
	geoGen.CreateCylinder(0.5f, 0.3f, 3.0f, 20, 20, cylinder);

	// Cache the vertex offsets to each object in the concatenated vertex buffer.
	BoxVertexOffset      = 0;
	GridVertexOffset     = box.Vertices.size();
	SphereVertexOffset   = GridVertexOffset + grid.Vertices.size();
	CylinderVertexOffset = SphereVertexOffset + sphere.Vertices.size();

	// Cache the index count of each object.
	BoxIndexCount      = box.Indices.size();
	GridIndexCount     = grid.Indices.size();
	SphereIndexCount   = sphere.Indices.size();
	CylinderIndexCount = cylinder.Indices.size();

	// Cache the starting index for each object in the concatenated index buffer.
	BoxIndexOffset      = 0;
	GridIndexOffset     = BoxIndexCount;
	SphereIndexOffset   = GridIndexOffset + GridIndexCount;
	CylinderIndexOffset = SphereIndexOffset + SphereIndexCount;

	UINT totalVertexCount = 
		box.Vertices.size() + 
		grid.Vertices.size() + 
		sphere.Vertices.size() +
		cylinder.Vertices.size();

	UINT totalIndexCount = 
		BoxIndexCount + 
		GridIndexCount + 
		SphereIndexCount +
		CylinderIndexCount;

	//
	// Extract the vertex elements we are interested in and pack the
	// vertices of all the meshes into one vertex buffer.
	//

	std::vector<Vertex> vertices(totalVertexCount);

	XMFLOAT3 black(0.0f, 0.0f, 0.0f);

	UINT k = 0;
	for(size_t i = 0; i < box.Vertices.size(); ++i, ++k)
	{
		vertices[k].Pos   = box.Vertices[i].Position;
		vertices[k].Color = black;
	}

	for(size_t i = 0; i < grid.Vertices.size(); ++i, ++k)
	{
		vertices[k].Pos   = grid.Vertices[i].Position;
		vertices[k].Color = black;
	}

	for(size_t i = 0; i < sphere.Vertices.size(); ++i, ++k)
	{
		vertices[k].Pos   = sphere.Vertices[i].Position;
		vertices[k].Color = black;
	}

	for(size_t i = 0; i < cylinder.Vertices.size(); ++i, ++k)
	{
		vertices[k].Pos   = cylinder.Vertices[i].Position;
		vertices[k].Color = black;
	}

	D3D11_BUFFER_DESC vbd;
	vbd.Usage = D3D11_USAGE_IMMUTABLE;
	vbd.ByteWidth = sizeof(Vertex) * totalVertexCount;
	vbd.BindFlags = D3D11_BIND_VERTEX_BUFFER;
	vbd.CPUAccessFlags = 0;
	vbd.MiscFlags = 0;
	D3D11_SUBRESOURCE_DATA vinitData;
	vinitData.pSysMem = &vertices[0];
	HR(pDevice->CreateBuffer(&vbd, &vinitData, &pVB));

	//
	// Pack the indices of all the meshes into one index buffer.
	//

	std::vector<UINT> indices;
	indices.insert(indices.end(), box.Indices.begin(), box.Indices.end());
	indices.insert(indices.end(), grid.Indices.begin(), grid.Indices.end());
	indices.insert(indices.end(), sphere.Indices.begin(), sphere.Indices.end());
	indices.insert(indices.end(), cylinder.Indices.begin(), cylinder.Indices.end());

	D3D11_BUFFER_DESC ibd;
	ibd.Usage = D3D11_USAGE_IMMUTABLE;
	ibd.ByteWidth = sizeof(UINT) * totalIndexCount;
	ibd.BindFlags = D3D11_BIND_INDEX_BUFFER;
	ibd.CPUAccessFlags = 0;
	ibd.MiscFlags = 0;
	D3D11_SUBRESOURCE_DATA iinitData;
	iinitData.pSysMem = &indices[0];
	HR(pDevice->CreateBuffer(&ibd, &iinitData, &pIB));
} 

void ShapesApp::init_fx()
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

void ShapesApp::init_layout()
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