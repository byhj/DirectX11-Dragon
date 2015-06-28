#include "d3dCamera.h"


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
	m_Proj  = XMMatrixPerspectiveFovLH(XMConvertToRadians(zoom), aspect, 1.0f, 1000.0f);
}

void D3DCamera::OnMouseWheel(WPARAM btnState, int x, int y, float aspect)
{
	zoom += x * 0.01f;
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
		m_Phi = MathHelper::Clamp(m_Phi, 0.1f, MathHelper::Pi-0.1f);
	}
	/*
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
	*/
	m_LastMousePos.x = x;
	m_LastMousePos.y = y;
}

