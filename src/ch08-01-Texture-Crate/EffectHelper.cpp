#include "EffectHelper.h"


namespace byhj
{
	EffectHelper::EffectHelper()
	{

	}
	EffectHelper::~EffectHelper()
	{

	}


	void EffectHelper::Init(ID3D11Device *pD3D11Device)
	{
		DWORD shaderFlags = 0;
#if defined( DEBUG ) || defined( _DEBUG )
		shaderFlags |= D3D10_SHADER_DEBUG;
		shaderFlags |= D3D10_SHADER_SKIP_OPTIMIZATION;
#endif

		ID3D10Blob* compiledShader = 0;
		ID3D10Blob* compilationMsgs = 0;
		HRESULT hr = D3DX11CompileFromFile(L"texture.fx", 0, 0, 0, "fx_5_0", shaderFlags,
			0, 0, &compiledShader, &compilationMsgs, 0);

		// compilationMsgs can store errors or warnings.
		if ( compilationMsgs!=0 )
		{
			MessageBoxA(0, ( char* )compilationMsgs->GetBufferPointer(), 0, 0);
			ReleaseCOM(compilationMsgs);
		}

		// Even if there are no compilationMsgs, check to make sure there were no other errors.
		if ( FAILED(hr) )
		{
			DXTrace(__FILE__, ( DWORD )__LINE__, hr, L"D3DX11CompileFromFile", true);
		}

		D3DX11CreateEffectFromMemory(compiledShader->GetBufferPointer(), compiledShader->GetBufferSize(),
			0, pD3D11Device, &m_pEffect);

		m_pEffectLight1Tech    = m_pEffect->GetTechniqueByName("Light1");
		m_pEffectLight2Tech    = m_pEffect->GetTechniqueByName("Light2");
		m_pEffectLight3Tech    = m_pEffect->GetTechniqueByName("Light3");
	
		m_pEffectLight0TexTech = m_pEffect->GetTechniqueByName("Light0Tex");
		m_pEffectLight1TexTech = m_pEffect->GetTechniqueByName("Light1Tex");
		m_pEffectLight2TexTech = m_pEffect->GetTechniqueByName("Light2Tex");
		m_pEffectLight3TexTech = m_pEffect->GetTechniqueByName("Light3Tex");

		m_pFxDirLight   = m_pEffect->GetVariableByName("g_DirLights");
		m_pFxMaterial   = m_pEffect->GetVariableByName("g_Mat");
		m_pFxEyePos     = m_pEffect->GetVariableByName("g_EyePos")->AsVector();
		m_pFxWorld = m_pEffect->GetVariableByName("g_World")->AsMatrix();
		m_pFxView  = m_pEffect->GetVariableByName("g_View")->AsMatrix();
		m_pFxProj  = m_pEffect->GetVariableByName("g_Proj")->AsMatrix();
		m_pFxDiffuseMap = m_pEffect->GetVariableByName("g_DiffuseMap")->AsShaderResource();

		// Done with compiled shader.
		ReleaseCOM(compiledShader);

		D3D11_INPUT_ELEMENT_DESC vertexDesc[] =
		{
			{"POSITION", 0, DXGI_FORMAT_R32G32B32_FLOAT, 0, 0, D3D11_INPUT_PER_VERTEX_DATA, 0},
			{"NORMAL", 0, DXGI_FORMAT_R32G32B32_FLOAT, 0, 12, D3D11_INPUT_PER_VERTEX_DATA, 0},
			{"TEXCOORD", 0, DXGI_FORMAT_R32G32_FLOAT, 0, 24, D3D11_INPUT_PER_VERTEX_DATA, 0}
		};

		D3DX11_PASS_DESC passDesc;
		m_pEffectLight1Tech->GetPassByIndex(0)->GetDesc(&passDesc);
		pD3D11Device->CreateInputLayout(vertexDesc, 3, passDesc.pIAInputSignature,
			passDesc.IAInputSignatureSize, &m_pInputLayout);
	}

	void EffectHelper::Render(ID3D11DeviceContext *pD3D11DeviceContext)
	{
		pD3D11DeviceContext->IASetInputLayout(m_pInputLayout);
	}

	void EffectHelper::Shutdown()
	{
		ReleaseCOM(m_pEffect);
	}

	void EffectHelper::SetEyePos(XMFLOAT4 &eyePos)
	{
		m_pFxEyePos->SetRawValue(&eyePos, 0, sizeof( XMFLOAT4 ));
	}

	void EffectHelper::SetDirLight(d3d::DirectionLight *dirLights)
	{
		m_pFxDirLight->SetRawValue(dirLights, 0, 2*sizeof( dirLights[0] ));
	}

	void EffectHelper::SetMaterial(d3d::Material &mat)
	{
		m_pFxMaterial->SetRawValue(&mat, 0, sizeof( mat )) ;
	}

	void EffectHelper::SetPointLight(d3d::PointLight &pointLight)
	{

	}

	void EffectHelper::SetSpotLight(d3d::SpotLight &spotLight)
	{

	}

	void EffectHelper::SetWorld(XMFLOAT4X4 World)
	{
		m_pFxWorld->SetMatrix(reinterpret_cast< float* >( &World ));
	}

	void EffectHelper::SetView(XMFLOAT4X4 View)
	{
		m_pFxView->SetMatrix(reinterpret_cast< float* >( &View ));

	}

	void EffectHelper::SetProj(XMFLOAT4X4 Proj)
	{
		m_pFxProj->SetMatrix(reinterpret_cast< float* >( &Proj ));

	}

	void EffectHelper::SetDiffuseMap(ID3D11ShaderResourceView *pDiffuseSRV)
	{
		m_pFxDiffuseMap->SetResource(pDiffuseSRV);
	}

}