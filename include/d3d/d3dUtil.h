#ifndef D3DUTIL_H
#define D3DUTIL_H

#include <DirectXmath.h>

using namespace DirectX;

#define ARRAYSIZE(arr) sizeof(arr) / sizeof(arr[0])

namespace Colors
{
	const XMFLOAT4 White     = XMFLOAT4(1.0f, 1.0f, 1.0f, 1.0f);
	const XMFLOAT4 Black     = XMFLOAT4(0.0f, 0.0f, 0.0f, 1.0f);
	const XMFLOAT4 Red       = XMFLOAT4(1.0f, 0.0f, 0.0f, 1.0f);
	const XMFLOAT4 Green     = XMFLOAT4(0.0f, 1.0f, 0.0f, 1.0f);
	const XMFLOAT4 Blue      = XMFLOAT4(0.0f, 0.0f, 1.0f, 1.0f);
	const XMFLOAT4 Yellow    = XMFLOAT4(1.0f, 1.0f, 0.0f, 1.0f);
	const XMFLOAT4 Cyan      = XMFLOAT4(0.0f, 1.0f, 1.0f, 1.0f);
	const XMFLOAT4 Magenta   = XMFLOAT4(1.0f, 0.0f, 1.0f, 1.0f);
	const XMFLOAT4 Silver    = XMFLOAT4(0.75f, 0.75f, 0.75f, 1.0f);
	//const XMFLOAT4 LightSteelBlue = {0.69f, 0.77f, 0.87f, 1.0f};
}

#endif