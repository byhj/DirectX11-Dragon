#ifndef D3DCAMERA_H
#define D3DCAMERA_H

#ifdef _WIN32
#define _XM_NO_INTRINSICS_
#endif 

#include <d3d11.h>
#include <iostream>
#include <xnamath.h>

#include "d3d/d3dUtil.h"

class D3DCamera
{
public:
	D3DCamera():m_Theta(1.5f * MathHelper::Pi), m_Phi(0.25f * MathHelper::Pi), m_Radius(5.0f)
	{
		m_LastMousePos.x = 0;
		m_LastMousePos.y = 0;
		XMMATRIX I = XMMatrixIdentity();
		m_World = I;
		m_View  = I;
		m_Proj  = I;
		aspect  = 1.0f;
		zoom = 45.0f;
	}

	void update();
	XMMATRIX GetViewMatrix() const 
	{
		return m_View;
	}
	XMMATRIX GetProjMatrix() const
	{
		return m_Proj;
	}
	XMFLOAT3 GetPos()
	{
		return pos;
	}
	void Init(float r, float asp)
	{
		m_Radius = r;
		aspect  =  asp;
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

	XMMATRIX m_World;
	XMMATRIX m_View;
	XMMATRIX m_Proj;
	XMFLOAT3 pos;
	XMFLOAT3 target;
	float aspect;
	float zoom;
};

#endif