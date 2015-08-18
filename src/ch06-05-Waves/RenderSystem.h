#ifndef RENDERSYSTEM_H
#define RENDERSYSTEM_H

#include "d3d/d3dApp.h"
#include "d3d/d3dFont.h"
#include "d3d/d3dTimer.h"
#include "d3d/d3dCamera.h"
#include "d3d/d3dUtility.h"

#include "Hill.h"

namespace byhj
{

	class RenderSystem : public D3DApp
{
public:
	RenderSystem();
	~RenderSystem();

	void v_Init();
	void v_Render();
	void v_Shutdown();

	void UpdateScene();
	void v_OnMouseDown(WPARAM btnState, int x, int y);
	void v_OnMouseMove(WPARAM btnState, int x, int y);
	void v_OnMouseUp(WPARAM btnState, int x, int y);
	void v_OnMouseWheel(WPARAM btnState, int x, int y);

private:
	void init_device();
	void init_camera();
	void init_object();
	void init_fbo();

	void BeginScene();
	void EndScene();
	void TurnZBufferOn();
	void TurnZBufferOff();
	void DrawFps();
	void DrawInfo(); 

	byhj::Hill m_Hill;
	byhj::D3DFont m_Font;
	byhj::D3DTimer m_Timer;
	byhj::D3DCamera m_Camera;
	byhj::MatrixBuffer m_Matrix;

	float fps = 0.0f;
	int m_videoCardMemory;
	std::wstring m_videoCardInfo;

	IDXGISwapChain           *m_pSwapChain                  = nullptr;
	ID3D11Device             *m_pD3D11Device                = nullptr;
	ID3D11DeviceContext      *m_pD3D11DeviceContext         = nullptr;
	ID3D11RenderTargetView   *m_pRenderTargetView           = nullptr;
	ID3D11DepthStencilView   *m_pDepthStencilView           = nullptr;
	ID3D11Texture2D          *m_pDepthStencilBuffer         = nullptr;
	ID3D11DepthStencilState  *m_pDepthStencilState          = nullptr;
	ID3D11DepthStencilState  *m_pDepthDisabledStencilState  = nullptr;
	ID3D11RasterizerState    *m_pRasterState                = nullptr;

};
}
#endif