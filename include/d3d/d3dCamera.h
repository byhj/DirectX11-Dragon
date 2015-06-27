#ifndef D3DCAMERA_H
#define D3DCAMERA_H

#include <d3d11.h>
#include <DirectXmath.h>

using namespace DirectX;

const float Pi = 3.1415926535f;

float Clamp(const float& x, const float& low, const float& high)
{
	return x < low ? low : (x > high ? high : x); 
}

class D3DCamera
{
public:
	D3DCamera():m_Theta(1.5f * Pi), m_Phi(0.25f * Pi), m_Radius(5.0f)
	{
		m_LastMousePos.x = 0;
		m_LastMousePos.y = 0;
		XMMATRIX I = XMMatrixIdentity();
		m_World = I;
		m_View  = I;
		m_Proj  = I;
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

	XMMATRIX m_World;
	XMMATRIX m_View;
	XMMATRIX m_Proj;
	XMFLOAT3 pos;
	XMFLOAT3 target;
};

void D3DCamera::update()
{
	// Convert Spherical to Cartesian coordinates.
	float x = m_Radius * sinf(m_Phi) * cosf(m_Theta);
	float z = m_Radius * sinf(m_Phi) * sinf(m_Theta);
	float y = m_Radius * cosf(m_Phi);

	// Build the view matrix.
	XMVECTOR pos    = XMVectorSet(x, y, z, 1.0f);
	XMVECTOR target = XMVectorZero();
	XMVECTOR up     = XMVectorSet(0.0f, 1.0f, 0.0f, 0.0f);

	XMStoreFloat3(&this->pos, pos);
	XMStoreFloat3(&this->target, target);

	m_View = XMMatrixLookAtLH(pos, target, up);
	//m_Proj  = XMMatrixPerspectiveFovLH( D3DXToRadian(45.0f), aspect, 1.0f, 1000.0f);
}

void D3DCamera::OnMouseWheel(WPARAM btnState, int x, int y, float aspect)
{
	static float zoom = 45.0f;
	zoom += x * 0.01f;
	m_Proj  = XMMatrixPerspectiveFovLH(XMConvertToRadians(45.0f), aspect, 1.0f, 1000.0f);
}

void D3DCamera::OnMouseDown(WPARAM btnState, int x, int y, HWND hWnd)
{
	m_LastMousePos.x = x;
	m_LastMousePos.y = y;

	SetCapture(hWnd );
}

void D3DCamera::OnMouseUp(WPARAM btnState, int x, int y)
{
	ReleaseCapture();
}

void D3DCamera::OnMouseMove(WPARAM btnState, int x, int y)
{
	if( (btnState & MK_LBUTTON) != 0 )
	{
		// Make each pixel correspond to a quarter of a degree.
		float dx = XMConvertToRadians(0.25f*static_cast<float>(x - m_LastMousePos.x));
		float dy = XMConvertToRadians(0.25f*static_cast<float>(y - m_LastMousePos.y));

		// Update angles based on input to orbit camera around box.
		m_Theta += dx;
		m_Phi   += dy;

		// Restrict the angle mPhi.
		m_Phi = Clamp(m_Phi, 0.1f, Pi-0.1f);
	}

	else if( (btnState & MK_RBUTTON) != 0 )
	{
		// Make each pixel correspond to 0.005 unit in the scene.
		float dx = 0.005f*static_cast<float>(x - m_LastMousePos.x);
		float dy = 0.005f*static_cast<float>(y - m_LastMousePos.y);

		// Update the camera radius based on input.
		m_Radius += dx - dy;

		// Restrict the radius.
		m_Radius = Clamp(m_Radius, 3.0f, 15.0f);
	}

	m_LastMousePos.x = x;
	m_LastMousePos.y = y;
}


#endif