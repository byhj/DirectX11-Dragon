/*//////////////////////////////////////////////////////////////////////////////////////

   This is a framework class, D3DApp class provide some interface, which can be called 
with different inherit class. You can rewirte the interface for different function.

   The main function is call by define macro, you can call by this format:
   CALL_MAIN(Inherit class), then the window will execute the render loop. 
	                                                                
	                                                                 -----byhj  15/6/26
*//////////////////////////////////////////////////////////////////////////////////////


#ifndef D3DAPP_H
#define D3DAPP_H

#pragma warning(disable: 4005)

#include <string>

#if defined(DEBUG) || defined(_DEBUG)
#define _CRTDBG_MAP_ALLOC
#include <crtdbg.h>
#endif

#include <windows.h>
#include <WindowsX.h>

#include <dxgi.h>
#include <d3d11.h>
#include <d3dcommon.h>
#include <DirectXMath.h>

#include <ctime>
#include <algorithm>
#include <string>
#include <sstream>
#include <fstream>
#include <vector>

#include "d3dDebug.h"

class D3DApp
{
public:
	D3DApp():m_AppName(L"Framework"), m_WndClassName(L"D3DWindow") 
	{

	}
	virtual ~D3DApp() {}

	int Run();
	LRESULT CALLBACK MessageHandler(HWND hWnd, UINT uMsg, WPARAM wParam, LPARAM lParam);

	//We make these function to dynamic call
	virtual void v_Init()  ;  
	virtual	void v_InitD3D()  {}
	virtual void v_Reshape()  {}
	virtual void v_Shutdown() {}
	virtual void v_Render()   {}
	virtual void v_Update()   {}

	// Convenience overrides for handling mouse input.
	virtual void v_OnMouseDown(WPARAM btnState, int x, int y){ }
	virtual void v_OnMouseUp(WPARAM btnState, int x, int y)  { }
	virtual void v_OnMouseMove(WPARAM btnState, int x, int y){ }
	virtual void v_OnMouseWheel(WPARAM btnState, int x, int y) { }

	float GetAspect()
	{
		return static_cast<float>(m_ScreenWidth) / static_cast<float>(m_ScreenHeight);
	}
	//We make public inherit can access these data 
protected:

	int       m_ScreenWidth;      //Window information
	int       m_ScreenHeight;
	float     m_ScreenFar;
	float     m_ScreenNear;
	int       m_PosX;
	int       m_PosY;

	LPCTSTR   m_AppName;
	LPCTSTR   m_WndClassName;
	HINSTANCE m_hInstance;
	HWND      m_hWnd;

private:

	bool init_window();
};

static LRESULT CALLBACK WndProc(HWND, UINT, WPARAM, LPARAM);
static D3DApp *AppHandle = 0;

int D3DApp::Run()
{	
	v_Init();

	//Render loop
	MSG msg;
	ZeroMemory(&msg, sizeof(MSG));
	while (true)
	{
		if (PeekMessage(&msg, NULL, 0, 0, PM_REMOVE) )
		{
			if(msg.message == WM_QUIT)
				break;
			TranslateMessage(&msg);
			DispatchMessage(&msg);
		}
		else 
		{
		   v_Render();
		}

	}

	v_Shutdown();

	return (int)msg.wParam;
}

void D3DApp::v_Init()
{
	init_window();

	v_InitD3D();
}

bool D3DApp::init_window()
{
	//Set the window in the middle of screen
	m_ScreenWidth = GetSystemMetrics(SM_CXSCREEN) * 0.75;
	m_ScreenHeight = GetSystemMetrics(SM_CYSCREEN) * 0.75;
	m_PosX = (GetSystemMetrics(SM_CXSCREEN) - m_ScreenWidth)  / 2;
	m_PosY = (GetSystemMetrics(SM_CYSCREEN) - m_ScreenHeight) / 2;
	m_ScreenNear = 0.1f;
	m_ScreenFar  = 1000.0f;

	AppHandle = this;
	m_hInstance = GetModuleHandle(NULL);

	//Register Class to window

	WNDCLASSEX wc;	
	wc.cbSize = sizeof(WNDCLASSEX);	
	wc.style = CS_HREDRAW | CS_VREDRAW;
	wc.lpfnWndProc = WndProc;	
	wc.cbClsExtra = NULL;	
	wc.cbWndExtra = NULL;	
	wc.hInstance = m_hInstance;
	wc.hIcon = LoadIcon(NULL, IDI_WINLOGO);	
	wc.hCursor = LoadCursor(NULL, IDC_ARROW);	
	wc.hbrBackground = (HBRUSH)(COLOR_WINDOW + 2);
	wc.lpszMenuName = NULL;	
	wc.lpszClassName = m_WndClassName;	
	wc.hIconSm = LoadIcon(NULL, IDI_WINLOGO); 

	if (!RegisterClassEx(&wc))
	{
		MessageBox(NULL, L"Registering Class Failded",	L"Error", MB_OK | MB_ICONERROR);
		return 1;
	}

	//Create the window and show

	m_hWnd = CreateWindowEx(	
		NULL,	           
		m_WndClassName,
		m_AppName,
		WS_OVERLAPPEDWINDOW,	
		m_PosX, m_PosY,
		m_ScreenWidth,	
		m_ScreenHeight,	
		NULL,
		NULL,
		m_hInstance,	
		NULL
		);

	if (!m_hWnd )	
	{
		MessageBox(NULL, L"Creating Window Failed", L"Error", MB_OK | MB_ICONERROR);
		return 1;
	}

	ShowWindow(m_hWnd, SW_SHOW);
	UpdateWindow(m_hWnd);	

	return true;
}


LRESULT CALLBACK D3DApp::MessageHandler(HWND hWnd, UINT uMsg, WPARAM wParam, LPARAM lParam)
{
	switch (uMsg)
	{
	    case WM_KEYDOWN:
	    	{
	    		if(wParam == VK_ESCAPE)
	    			PostMessage(m_hWnd, WM_DESTROY, 0, 0);
	    	}

	    case WM_LBUTTONDOWN:
	    case WM_MBUTTONDOWN:
	    case WM_RBUTTONDOWN:
	    	v_OnMouseDown(wParam, GET_X_LPARAM(lParam), GET_Y_LPARAM(lParam));
	    	return 0;

	    case WM_LBUTTONUP:
	    case WM_MBUTTONUP:
	    case WM_RBUTTONUP:
	    	v_OnMouseUp(wParam, GET_X_LPARAM(lParam), GET_Y_LPARAM(lParam));
	    	return 0;

	    case WM_MOUSEMOVE:
	    	v_OnMouseMove(wParam, GET_X_LPARAM(lParam), GET_Y_LPARAM(lParam));
	    	return 0;

	    case WM_MOUSEWHEEL:
	    	v_OnMouseWheel(wParam, GET_WHEEL_DELTA_WPARAM(wParam), GET_Y_LPARAM(lParam));

	    default:
		    return DefWindowProc(hWnd, uMsg, wParam, lParam);
	} 
}

LRESULT CALLBACK WndProc(HWND hwnd, UINT umessage, WPARAM wparam, LPARAM lparam)
{
	switch(umessage)
	{
	    case WM_DESTROY:
	    {
			PostQuitMessage(0);
			return 0;
	    }
	    case WM_CLOSE:
	    {
	    	PostQuitMessage(0);		
	    	return 0;
	    }
	     // All other messages pass to the message handler in the system class.
	    default:
	    {
	    	return AppHandle->MessageHandler(hwnd, umessage, wparam, lparam);
	    }
	}
}

//D3DApp * D3DApp::app; 
#define CALL_MAIN(a)                                    \
int WINAPI WinMain(HINSTANCE hInstance, HINSTANCE hPrevInstance, PSTR pScmdline, int iCmdshow) \
{                                                       \
	a *app = new a;                                     \
	app->Run();                                         \
	delete app;                                         \
	return 0;                                           \
}


#endif