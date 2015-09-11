#ifndef EFFECTHELPER_H
#define EFFECTHELPER_H

#include <d3dx11.h>
#include <xnamath.h>
#include "d3dx11effect.h"
#include "d3d/Light.h"
#include "d3d/Utility.h"

namespace byhj
{


class EffectHelper
{
public: 
	EffectHelper();
	~EffectHelper();

public:

	void Init(ID3D11Device *pD3D11Device);
	void Render(ID3D11DeviceContext *pD3D11DeviceContext);
	void Shutdown();

	void SetWorld(XMFLOAT4X4 World);
	void SetView( XMFLOAT4X4 View);
	void SetProj( XMFLOAT4X4 Proj);

	void SetEyePos(XMFLOAT4 &eyePos);
	void SetDirLight(d3d::DirectionLight &dirLight);
	void SetPointLight(d3d::PointLight &pointLight);
	void SetSpotLight(d3d::SpotLight &spotLight);
	void SetMaterial(d3d::Material &mat);

	ID3DX11EffectTechnique* GetEffectTech()
	{
		return m_pEffectTech1;
	}

private:
	ID3DX11Effect          *m_pEffect        = nullptr;
	ID3DX11EffectTechnique *m_pEffectTech1   = nullptr;
	ID3DX11EffectTechnique *m_pEffectTech2   = nullptr;
	ID3DX11EffectTechnique *m_pEffectTech3   = nullptr;
											 
											 
	ID3DX11EffectVariable  *m_pFxDirLight    = nullptr;
	ID3DX11EffectVariable  *m_pFxMaterial    = nullptr;
	ID3DX11EffectMatrixVariable *m_pFxWorld  = nullptr;
	ID3DX11EffectMatrixVariable *m_pFxView   = nullptr;
	ID3DX11EffectMatrixVariable *m_pFxProj   = nullptr;
	ID3DX11EffectVectorVariable *m_pFxEyePos = nullptr;
	ID3D11InputLayout           *m_pInputLayout = nullptr;
};

}

#endif
