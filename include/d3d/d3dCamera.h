#ifndef D3DCAMERA_H
#define D3DCAMERA_H

#ifdef _WIN32
#define _XM_NO_INTRINSICS_
#endif 

#include <d3d11.h>
#include <xnamath.h>
#include <D3DX10math.h>
#include <vector>

namespace byhj
{

const float Pi = 3.1415926535f;

class D3DCamera
{
public:
    D3DCamera():m_Theta(1.5f * Pi), m_Phi(0.25f * Pi), m_Radius(15.0f)
	{
		m_LastMousePos.x = 0;
		m_LastMousePos.y = 0;
		XMMATRIX I = XMMatrixIdentity();
		XMStoreFloat4x4(&m_World,  I);
		XMStoreFloat4x4(&m_View ,  I);
		XMStoreFloat4x4(&m_Proj ,  I);
	}

	void update();
	XMFLOAT4X4 GetViewMatrix() const 
	{
		return m_View;
	}
    XMFLOAT4X4 GetProjMatrix() const
	{
		return m_Proj;
	}
	XMFLOAT3 GetPos()
	{
		return pos;
	}
	void SetRadius(float r)
	{
		m_Radius = r;
	}
	void OnMouseDown(WPARAM btnState, int x, int y, HWND hWnd);
	void OnMouseMove(WPARAM btnState, int x, int y);
	void OnMouseUp(WPARAM btnState, int x, int y);
	void OnMouseWheel(WPARAM btnState, int x, int y, float aspect);

private:

	float m_Theta;
	float m_Phi;
	float m_Radius;
	POINT m_LastMousePos;

	XMFLOAT4X4 m_World;
	XMFLOAT4X4 m_View;
	XMFLOAT4X4 m_Proj;
	XMFLOAT3 pos;
	XMFLOAT3 target;
	float m_aspect;
};


}

#endif