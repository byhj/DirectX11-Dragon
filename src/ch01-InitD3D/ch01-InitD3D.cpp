#ifdef _DEBUG  //If in debug mode, we show a console to output debug message
#pragma comment( linker, "/subsystem:\"console\" /entry:\"WinMainCRTStartup\"")
#endif

#include "d3d/d3dApp.h"

class D3DInitApp: public D3DApp
{
public:
	D3DInitApp() 
	{
		m_AppName = L"InitD3D";

		m_pSwapChain             = NULL;
		m_pD3D11Device           = NULL;
		m_pD3D11DeviceContext    = NULL;
		m_pD3D11RenderTargetView = NULL;
	}
	~D3DInitApp(){}

public:
	void v_InitD3D()
	{
		init_device();
	}

	void v_Render()
	{
		static float bgColor[4] = {0.2f, 0.4f, 0.5f, 1.0f};
		m_pD3D11DeviceContext->ClearRenderTargetView(m_pD3D11RenderTargetView, bgColor);
	
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

private:
	IDXGISwapChain         *m_pSwapChain;
	ID3D11Device           *m_pD3D11Device;
	ID3D11DeviceContext    *m_pD3D11DeviceContext;
	ID3D11RenderTargetView *m_pD3D11RenderTargetView;
};

CALL_MAIN(D3DInitApp)

void D3DInitApp::init_device()
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
