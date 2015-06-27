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

#include "cube.h"

class D3DRenderSystem: public D3DApp
{
public:
	D3DRenderSystem() 
	{
		m_AppName = L"InitD3D";

		m_pSwapChain             = NULL;
		m_pD3D11Device           = NULL;
		m_pD3D11DeviceContext    = NULL;
		m_pD3D11RenderTargetView = NULL;
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
		m_pD3D11DeviceContext->ClearRenderTargetView(m_pD3D11RenderTargetView, bgColor);
		m_pD3D11DeviceContext->IASetPrimitiveTopology(D3D11_PRIMITIVE_TOPOLOGY_TRIANGLELIST);
		Model = XMMatrixIdentity();

		m_cube.Render(m_pD3D11DeviceContext, Model, View, Proj);

		m_pSwapChain->Present(0, 0);
	}

	void v_Shutdown()
	{
		ReleaseCOM(m_pSwapChain);
		ReleaseCOM(m_pD3D11Device);
		ReleaseCOM(m_pD3D11DeviceContext);
		ReleaseCOM(m_pD3D11RenderTargetView);
	}

private:
	void init_device();
	void init_object();
	void init_camera();

private:

	D3DCamera m_camera;
	D3DFont   m_font;
	D3DTimer  m_timer;
	Cube      m_cube;

	IDXGISwapChain         *m_pSwapChain;
	ID3D11Device           *m_pD3D11Device;
	ID3D11DeviceContext    *m_pD3D11DeviceContext;
	ID3D11RenderTargetView *m_pD3D11RenderTargetView;

	XMMATRIX Model;
	XMMATRIX View;
	XMMATRIX Proj;
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
	hr = m_pD3D11Device->CreateRenderTargetView(pBackBuffer, NULL, &m_pD3D11RenderTargetView);
	pBackBuffer->Release();

}

void D3DRenderSystem::init_object()
{
	m_camera.SetRadius(5.0f);

	m_timer.Reset();

	m_cube.init_buffer(m_pD3D11Device, m_pD3D11DeviceContext);
	m_cube.init_shader(m_pD3D11Device, m_hWnd);
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