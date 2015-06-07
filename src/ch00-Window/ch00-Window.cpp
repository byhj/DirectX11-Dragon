#include "d3dApp.h"

class D3DWindow :public D3DApp
{
public:
	D3DWindow(HINSTANCE hInstance);
	~D3DWindow();

	bool Init();
	void Resize();
	void UpdateScene(float dt);
	void Render();
};

int WINAPI wWinMain( _In_ HINSTANCE hInstance, _In_opt_ HINSTANCE hPrevInstance, 
					_In_ LPWSTR lpCmdLine, _In_ int nShowCmd )
{
// Enable run-time memory check for debug builds.
#if defined(DEBUG) | defined(_DEBUG)
      _CrtSetDbgFlag( _CRTDBG_ALLOC_MEM_DF | _CRTDBG_LEAK_CHECK_DF );
#endif

	  //init the window
	  D3DWindow app(hInstance);
	  if ( !app.Init() )
		  return 0;

	  return  app.Run();
}


D3DWindow::D3DWindow(HINSTANCE hInstance)
	:D3DApp(hInstance)
{
}

D3DWindow::~D3DWindow()
{
}

bool D3DWindow::Init()
{
   if ( !D3DApp::Init() )
	   return false;
   
   return true;
}

void D3DWindow::Resize()
{
    D3DApp::Resize();
}

void D3DWindow::UpdateScene(float dt)
{
}

void D3DWindow::Render()
{
	assert(pDeviceContext);
	assert(pSwapChain);
	pDeviceContext->ClearRenderTargetView(pRenderTargetView, reinterpret_cast<const float*>(&Colors::Silver));
	pDeviceContext->ClearDepthStencilView(pDepthStencilView, D3D11_CLEAR_DEPTH|D3D11_CLEAR_STENCIL, 1.0f, 0);

	HR(pSwapChain->Present(0, 0));
}