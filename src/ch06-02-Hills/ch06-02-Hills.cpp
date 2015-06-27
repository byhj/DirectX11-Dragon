#pragma warning(disable: 4005)

#ifdef _DEBUG  //If in debug mode, we show a console to output debug message
#pragma comment( linker, "/subsystem:\"console\" /entry:\"WinMainCRTStartup\"")
#endif

#ifdef _WIN32
#define _XM_NO_INTRINSICS_
#endif 

#include "d3d/d3dApp.h"
#include "d3d/d3dCamera.h"
#include "d3d/d3dTimer.h"
#include "d3d/d3dFont.h"

#include "hill.h"

class D3DRenderSystem: public D3DApp
{
public:
	D3DRenderSystem() 
	{
		m_AppName = L"Cube";

		m_pSwapChain             = NULL;
		m_pD3D11Device           = NULL;
		m_pD3D11DeviceContext    = NULL;
		m_pRenderTargetView      = NULL;
		m_pDepthStencilView      = NULL;
		m_pDepthStencilBuffer    = NULL;
		m_pRasterState           = NULL;
		fps = 0.0f;
	}
	~D3DRenderSystem(){}

public:
	void v_InitD3D()
	{
		init_device();
		init_object();
		init_camera();
	}

	void v_Render()
	{
		static float bgColor[4] = {0.2f, 0.4f, 0.5f, 1.0f};
		m_pD3D11DeviceContext->OMSetRenderTargets( 1, &m_pRenderTargetView, m_pDepthStencilView);
		m_pD3D11DeviceContext->ClearRenderTargetView(m_pRenderTargetView, bgColor);
	    m_pD3D11DeviceContext->ClearDepthStencilView(m_pDepthStencilView, D3D11_CLEAR_DEPTH|D3D11_CLEAR_STENCIL, 1.0f, 0);
		m_pD3D11DeviceContext->IASetPrimitiveTopology(D3D11_PRIMITIVE_TOPOLOGY_TRIANGLELIST);
		
		m_camera.update();
		Model = XMMatrixIdentity();
		View  = m_camera.GetViewMatrix();
		Proj  = m_camera.GetProjMatrix();
		m_hill.Render(m_pD3D11DeviceContext, Model, View, Proj);

		DrawMessage();

		m_pSwapChain->Present(0, 0);
	}

	void v_Shutdown()
	{
		ReleaseCOM(m_pSwapChain);
		ReleaseCOM(m_pD3D11Device);
		ReleaseCOM(m_pD3D11DeviceContext);
		ReleaseCOM(m_pRenderTargetView);
		ReleaseCOM(m_pDepthStencilView  )
		ReleaseCOM(m_pDepthStencilBuffer)
		ReleaseCOM(m_pRasterState);
	}
	void v_OnMouseDown(WPARAM btnState, int x, int y);
	void v_OnMouseMove(WPARAM btnState, int x, int y);
	void v_OnMouseUp(WPARAM btnState, int x, int y);
	void v_OnMouseWheel(WPARAM btnState, int x, int y);

private:
	void init_device();
	void init_object();
	void init_camera();
	void DrawMessage();

private:

	D3DCamera m_camera;
	D3DFont   m_font;
	D3DTimer  m_timer;
	Hill      m_hill;

	IDXGISwapChain         *m_pSwapChain;
	ID3D11Device           *m_pD3D11Device;
	ID3D11DeviceContext    *m_pD3D11DeviceContext;
	ID3D11RenderTargetView *m_pRenderTargetView;
	ID3D11DepthStencilView *m_pDepthStencilView;
	ID3D11Texture2D        *m_pDepthStencilBuffer;
	ID3D11RasterizerState  *m_pRasterState;
	XMMATRIX Model;
	XMMATRIX View;
	XMMATRIX Proj;

	int m_videoCardMemory;
	WCHAR m_videoCardInfo[255];
	float fps;
};

CALL_MAIN(D3DRenderSystem)

void D3DRenderSystem::init_device()
{
	HRESULT hr;
	DXGI_MODE_DESC bufferDesc;
	ZeroMemory(&bufferDesc, sizeof(DXGI_MODE_DESC) );
	bufferDesc.Width                   = m_ScreenWidth;
	bufferDesc.Height                  = m_ScreenHeight;
	bufferDesc.RefreshRate.Numerator   = 60;
	bufferDesc.RefreshRate.Denominator = 1;
	bufferDesc.Format                  = DXGI_FORMAT_R8G8B8A8_UNORM;
	bufferDesc.ScanlineOrdering        = DXGI_MODE_SCANLINE_ORDER_UNSPECIFIED;
	bufferDesc.Scaling                 = DXGI_MODE_SCALING_UNSPECIFIED;

	DXGI_SWAP_CHAIN_DESC swapChainDesc;
	ZeroMemory(&swapChainDesc, sizeof(DXGI_SWAP_CHAIN_DESC));
	swapChainDesc.BufferDesc         = bufferDesc;
	swapChainDesc.SampleDesc.Count   = 1;
	swapChainDesc.SampleDesc.Quality = 0;
	swapChainDesc.BufferUsage        = DXGI_USAGE_RENDER_TARGET_OUTPUT;
	swapChainDesc.BufferCount        = 1;
	swapChainDesc.OutputWindow       = m_hWnd;
	swapChainDesc.Windowed           = TRUE;
	swapChainDesc.SwapEffect         = DXGI_SWAP_EFFECT_DISCARD;

	//Create the double buffer chain
	hr = D3D11CreateDeviceAndSwapChain(NULL, D3D_DRIVER_TYPE_HARDWARE,
		NULL, NULL, NULL, NULL, D3D11_SDK_VERSION, 
		&swapChainDesc, &m_pSwapChain, &m_pD3D11Device,
		NULL, &m_pD3D11DeviceContext);
	DebugHR(hr);

	//Create backbuffer, buffer also is a texture
	ID3D11Texture2D *pBackBuffer;
	hr = m_pSwapChain->GetBuffer(0, __uuidof(ID3D11Texture2D), (void**)&pBackBuffer);
	hr = m_pD3D11Device->CreateRenderTargetView(pBackBuffer, NULL, &m_pRenderTargetView);
	DebugHR(hr)
	pBackBuffer->Release();

	/////////////////////Describe our Depth/Stencil Buffer///////////////////////////
	D3D11_TEXTURE2D_DESC depthStencilDesc;

	depthStencilDesc.Width              = m_ScreenWidth;
	depthStencilDesc.Height             = m_ScreenHeight;
	depthStencilDesc.MipLevels          = 1;
	depthStencilDesc.ArraySize          = 1;
	depthStencilDesc.Format             = DXGI_FORMAT_D24_UNORM_S8_UINT;
	depthStencilDesc.SampleDesc.Count   = 1;
	depthStencilDesc.SampleDesc.Quality = 0;
	depthStencilDesc.Usage              = D3D11_USAGE_DEFAULT;
	depthStencilDesc.BindFlags          = D3D11_BIND_DEPTH_STENCIL;
	depthStencilDesc.CPUAccessFlags     = 0; 
	depthStencilDesc.MiscFlags          = 0;

	m_pD3D11Device->CreateTexture2D(&depthStencilDesc, NULL, &m_pDepthStencilBuffer);
	m_pD3D11Device->CreateDepthStencilView(m_pDepthStencilBuffer, NULL, &m_pDepthStencilView);

	//////////////////////Raterizer State/////////////////////////////
	D3D11_RASTERIZER_DESC rasterDesc;
	ZeroMemory(&rasterDesc, sizeof(D3D11_RASTERIZER_DESC));
	rasterDesc.FillMode = D3D11_FILL_SOLID;
	rasterDesc.CullMode = D3D11_CULL_BACK;
	rasterDesc.FrontCounterClockwise = false;
	hr = m_pD3D11Device->CreateRasterizerState(&rasterDesc, &m_pRasterState);
	m_pD3D11DeviceContext->RSSetState(m_pRasterState);

	///////////////////////Get Device Information/////////////////////////////////
	unsigned int numModes, i, numerator, denominator, stringLength;
	IDXGIFactory* factory;
	IDXGIAdapter* adapter;
	IDXGISurface *surface;
	DXGI_ADAPTER_DESC adapterDesc;

	// Create a DirectX graphics interface factory.
	CreateDXGIFactory(__uuidof(IDXGIFactory), (void**)&factory);
	// Use the factory to create an adapter for the primary graphics interface (video card).
	factory->EnumAdapters(0, &adapter);
	adapter->GetDesc(&adapterDesc);
	m_videoCardMemory = (int)(adapterDesc.DedicatedVideoMemory / 1024 / 1024);

	// Convert the name of the video card to a character array and store it.
	swprintf(m_videoCardInfo,L"Video Card  : %ls", adapterDesc.Description);
}

void D3DRenderSystem::init_object()
{
	m_camera.Init(100.0f, GetAspect());

	m_timer.Reset();

	m_font.init(m_pD3D11Device, L"Arial");

	m_hill.init_buffer(m_pD3D11Device, m_pD3D11DeviceContext);
	m_hill.init_shader(m_pD3D11Device, m_hWnd);
}

void D3DRenderSystem::init_camera()
{
	//Viewport Infomation
	D3D11_VIEWPORT vp;
	ZeroMemory(&vp, sizeof(D3D11_VIEWPORT));
	vp.TopLeftX = 0;
	vp.TopLeftY = 0;
	vp.Width    = static_cast<FLOAT>(m_ScreenWidth);
	vp.Height   = static_cast<FLOAT>(m_ScreenHeight);
	m_pD3D11DeviceContext->RSSetViewports(1, &vp);

	//MVP Matrix
	XMVECTOR camPos    = XMVectorSet( 0.0f, 0.0f, -3.0f, 0.0f );
	XMVECTOR camTarget = XMVectorSet( 0.0f, 0.0f, 0.0f, 0.0f );
	XMVECTOR camUp     = XMVectorSet( 0.0f, 1.0f, 0.0f, 0.0f );
	View      = XMMatrixLookAtLH( camPos, camTarget, camUp );
	Proj      = XMMatrixPerspectiveFovLH( 0.4f*3.14f, GetAspect(), 1.0f, 1000.0f);
}


void D3DRenderSystem::DrawMessage()
{
	static bool flag = true;
	if (flag)
	{
		m_timer.Start();
		flag = false;
	}
	m_timer.Count();
	static int frameCnt = 0;
	static float timeElapsed = 0.0f;

	frameCnt++;
	if(m_timer.GetTotalTime() - timeElapsed >= 1.0f)
	{
		fps = frameCnt;
		frameCnt = 0;
		timeElapsed += 1.0f;
	}	
	static WCHAR frameStr[255];
	wsprintfW(frameStr, L"FPS: %u", (UINT)fps);

	m_font.drawText(m_pD3D11DeviceContext, frameStr, 22.0f, 10.0f, 10.0f, 0xff0099ff);

	WCHAR WinInfo[255];
	swprintf(WinInfo, L"Window Size: %d x %d", m_ScreenWidth, m_ScreenHeight);
	m_font.drawText(m_pD3D11DeviceContext, WinInfo, 22.0f, 10.0f, 40.0f, 0xff0099ff);
	m_font.drawText(m_pD3D11DeviceContext, m_videoCardInfo, 22.0f, 10.0f, 70.0f, 0xff0099ff);

}
void  D3DRenderSystem::v_OnMouseDown(WPARAM btnState, int x, int y)
{
	m_camera.OnMouseDown(btnState, x, y, m_hWnd);
}

void  D3DRenderSystem::v_OnMouseMove(WPARAM btnState, int x, int y)
{
	m_camera.OnMouseMove(btnState, x, y);
}

void  D3DRenderSystem::v_OnMouseUp(WPARAM btnState, int x, int y)
{
	m_camera.OnMouseUp(btnState, x, y);
}

void  D3DRenderSystem::v_OnMouseWheel(WPARAM btnState, int x, int y)
{
	m_camera.OnMouseWheel(btnState, x, y, GetAspect());
}