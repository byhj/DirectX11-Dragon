#include "Object.h"
#include <D3DX11.h>

namespace byhj
{

void Object::Init(ID3D11Device *pD3D11Device, ID3D11DeviceContext *pD3D11DeviceContext, HWND hWnd)
{
	init_buffer(pD3D11Device, pD3D11DeviceContext);
	init_shader(pD3D11Device, hWnd);
}

void Object::Render(ID3D11DeviceContext *pD3D11DeviceContext, const d3d::MatrixBuffer &matrix)
{
	cbMatrix.model = matrix.model;
	cbMatrix.view  = matrix.view;
	cbMatrix.proj  = matrix.proj;

	// Set vertex buffer stride and offset
	unsigned int stride;
	unsigned int offset;
	stride = sizeof( Vertex );
	offset = 0;
	pD3D11DeviceContext->IASetVertexBuffers(0, 1, &m_pVertexBuffer, &stride, &offset);
	pD3D11DeviceContext->IASetIndexBuffer(m_pIndexBuffer, DXGI_FORMAT_R32_UINT, 0);
	pD3D11DeviceContext->IASetInputLayout(m_pInputLayout);
	pD3D11DeviceContext->IASetPrimitiveTopology(D3D11_PRIMITIVE_TOPOLOGY_TRIANGLELIST);


	D3DX11_TECHNIQUE_DESC techDesc;
	m_pEffectTechnique->GetDesc(&techDesc);
	for ( UINT p = 0; p<techDesc.Passes; ++p )
	{

		m_pWorld->SetMatrix(reinterpret_cast<float*>(&cbMatrix.model));
		m_pView->SetMatrix(reinterpret_cast<float*>(&cbMatrix.view));
		m_pProj->SetMatrix(reinterpret_cast<float*>(&cbMatrix.proj));
		m_pEffectTechnique->GetPassByIndex(p)->Apply(0, pD3D11DeviceContext);
	    pD3D11DeviceContext->DrawIndexed(mGridIndexCount, mGridIndexOffset, mGridVertexOffset);
	    
	    // Draw the box.
	    XMMATRIX world ;
	    world = XMLoadFloat4x4(&mBoxWorld);
		XMStoreFloat4x4(&cbMatrix.model, world);
		m_pWorld->SetMatrix(reinterpret_cast< float* >( &cbMatrix.model ));
		m_pView->SetMatrix(reinterpret_cast< float* >( &cbMatrix.view ));
		m_pProj->SetMatrix(reinterpret_cast< float* >( &cbMatrix.proj ));
		m_pEffectTechnique->GetPassByIndex(p)->Apply(0, pD3D11DeviceContext);
	    pD3D11DeviceContext->DrawIndexed(mBoxIndexCount, mBoxIndexOffset, mBoxVertexOffset);
	    
	    // Draw center sphere.
	    world = XMLoadFloat4x4(&mCenterSphere);
	    XMStoreFloat4x4(&cbMatrix.model, world);
		m_pWorld->SetMatrix(reinterpret_cast< float* >( &cbMatrix.model ));
		m_pView->SetMatrix(reinterpret_cast< float* >( &cbMatrix.view ));
		m_pProj->SetMatrix(reinterpret_cast< float* >( &cbMatrix.proj ));
		m_pEffectTechnique->GetPassByIndex(p)->Apply(0, pD3D11DeviceContext);
	    pD3D11DeviceContext->DrawIndexed(mSphereIndexCount, mSphereIndexOffset, mSphereVertexOffset);
	    
	    // Draw the cylinders.
	    for ( int i = 0; i<10; ++i )
	    {
	    	world = XMLoadFloat4x4(&mCylWorld[i]);
			XMStoreFloat4x4(&cbMatrix.model, world);
			m_pWorld->SetMatrix(reinterpret_cast< float* >( &cbMatrix.model ));
			m_pView->SetMatrix(reinterpret_cast< float* >( &cbMatrix.view ));
			m_pProj->SetMatrix(reinterpret_cast< float* >( &cbMatrix.proj ));
			m_pEffectTechnique->GetPassByIndex(p)->Apply(0, pD3D11DeviceContext);
	    	pD3D11DeviceContext->DrawIndexed(mCylinderIndexCount, mCylinderIndexOffset, mCylinderVertexOffset);
	    }
	    
	    // Draw the spheres.
	    for ( int i = 0; i<10; ++i )
	    {
	    	world = XMLoadFloat4x4(&mSphereWorld[i]);
			XMStoreFloat4x4(&cbMatrix.model, world);
			m_pWorld->SetMatrix(reinterpret_cast< float* >( &cbMatrix.model ));
			m_pView->SetMatrix(reinterpret_cast< float* >( &cbMatrix.view ));
			m_pProj->SetMatrix(reinterpret_cast< float* >( &cbMatrix.proj ));
			m_pEffectTechnique->GetPassByIndex(p)->Apply(0, pD3D11DeviceContext);
	    	pD3D11DeviceContext->DrawIndexed(mSphereIndexCount, mSphereIndexOffset, mSphereVertexOffset);
	    }

	}


}

void Object::Shutdown()
{
	ReleaseCOM(m_pVertexBuffer)
	ReleaseCOM(m_pIndexBuffer)
	ReleaseCOM(m_pInputLayout)
}


void Object::init_buffer(ID3D11Device *pD3D11Device, ID3D11DeviceContext *pD3D11DeviceContext)
{

	XMMATRIX boxScale = XMMatrixScaling(2.0f, 1.0f, 2.0f);
	XMMATRIX boxOffset = XMMatrixTranslation(0.0f, 0.5f, 0.0f);
	XMStoreFloat4x4(&mBoxWorld, XMMatrixMultiply(boxScale, boxOffset));

	XMMATRIX centerSphereScale = XMMatrixScaling(2.0f, 2.0f, 2.0f);
	XMMATRIX centerSphereOffset = XMMatrixTranslation(0.0f, 2.0f, 0.0f);
	XMStoreFloat4x4(&mCenterSphere, XMMatrixMultiply(centerSphereScale, centerSphereOffset));

	for(int i = 0; i < 5; ++i)
	{
		XMStoreFloat4x4(&mCylWorld[i*2+0], XMMatrixTranslation(-5.0f, 1.5f, -10.0f + i*5.0f));
		XMStoreFloat4x4(&mCylWorld[i*2+1], XMMatrixTranslation(+5.0f, 1.5f, -10.0f + i*5.0f));

		XMStoreFloat4x4(&mSphereWorld[i*2+0], XMMatrixTranslation(-5.0f, 3.5f, -10.0f + i*5.0f));
		XMStoreFloat4x4(&mSphereWorld[i*2+1], XMMatrixTranslation(+5.0f, 3.5f, -10.0f + i*5.0f));
	}

	HRESULT hr;
	d3d::Geometry::MeshData box;
	d3d::Geometry::MeshData grid;
	d3d::Geometry::MeshData sphere;
	d3d::Geometry::MeshData cylinder;
	d3d::Geometry geoGen;

	geoGen.CreateCube(1.0f, 1.0f, 1.0f, box);
	geoGen.CreateGrid(20.0f, 30.0f, 60, 40, grid);
	geoGen.CreateSphere(0.5f, 20, 20, sphere);
	//geoGen.CreateGeosphere(0.5f, 2, sphere);
	geoGen.CreateCylinder(0.5f, 0.3f, 3.0f, 20, 20, cylinder);
	// Cache the vertex offsets to each object in the concatenated vertex buffer.
	mBoxVertexOffset      = 0;
	mGridVertexOffset     = box.VertexData.size();
	mSphereVertexOffset   = mGridVertexOffset + grid.VertexData.size();
	mCylinderVertexOffset = mSphereVertexOffset + sphere.VertexData.size();

	// Cache the vertex offsets to each object in the concatenated vertex buffer.
	mBoxVertexOffset      = 0;
	mGridVertexOffset     = box.VertexData.size();
	mSphereVertexOffset   = mGridVertexOffset + grid.VertexData.size();
	mCylinderVertexOffset = mSphereVertexOffset + sphere.VertexData.size();

	// Cache the index count of each object.
	mBoxIndexCount      = box.IndexData.size();
	mGridIndexCount     = grid.IndexData.size();
	mSphereIndexCount   = sphere.IndexData.size();
	mCylinderIndexCount = cylinder.IndexData.size();

	// Cache the starting index for each object in the concatenated index buffer.
	mBoxIndexOffset      = 0;
	mGridIndexOffset     = mBoxIndexCount;
	mSphereIndexOffset   = mGridIndexOffset + mGridIndexCount;
	mCylinderIndexOffset = mSphereIndexOffset + mSphereIndexCount;

	UINT totalVertexCount =  box.VertexData.size() + grid.VertexData.size() + 
		                     sphere.VertexData.size() + cylinder.VertexData.size();

	UINT totalIndexCount =  mBoxIndexCount + mGridIndexCount + 
		                    mSphereIndexCount + mCylinderIndexCount;

	std::vector<Vertex> vertices(totalVertexCount);
	UINT k = 0;
	for(size_t i = 0; i < box.VertexData.size(); ++i, ++k)
	{
		vertices[k].Pos   = box.VertexData[i].Pos;
		vertices[k].Normal = box.VertexData[i].Normal;
	}

	for(size_t i = 0; i < grid.VertexData.size(); ++i, ++k)
	{
		vertices[k].Pos   = grid.VertexData[i].Pos;
		vertices[k].Normal = grid.VertexData[i].Normal;
	}

	for(size_t i = 0; i < sphere.VertexData.size(); ++i, ++k)
	{
		vertices[k].Pos   = sphere.VertexData[i].Pos;
		vertices[k].Normal = sphere.VertexData[i].Normal;
	}

	for(size_t i = 0; i < cylinder.VertexData.size(); ++i, ++k)
	{
		vertices[k].Pos   = cylinder.VertexData[i].Pos;
		vertices[k].Normal = cylinder.VertexData[i].Normal;
	}

	D3D11_BUFFER_DESC vertexBufferDesc;
	vertexBufferDesc.Usage               = D3D11_USAGE_IMMUTABLE;
	vertexBufferDesc.ByteWidth           = sizeof(Vertex) * totalVertexCount;
	vertexBufferDesc.BindFlags           = D3D11_BIND_VERTEX_BUFFER;
	vertexBufferDesc.CPUAccessFlags      = 0;
	vertexBufferDesc.MiscFlags           = 0;
	vertexBufferDesc.StructureByteStride = 0;

	D3D11_SUBRESOURCE_DATA VBO;
	VBO.pSysMem = &vertices[0];
	hr = pD3D11Device->CreateBuffer(&vertexBufferDesc, &VBO, &m_pVertexBuffer);
	DebugHR(hr);

	std::vector<UINT> IndexData;
	IndexData.insert(IndexData.end(), box.IndexData.begin(), box.IndexData.end());
	IndexData.insert(IndexData.end(), grid.IndexData.begin(), grid.IndexData.end());
	IndexData.insert(IndexData.end(), sphere.IndexData.begin(), sphere.IndexData.end());
	IndexData.insert(IndexData.end(), cylinder.IndexData.begin(), cylinder.IndexData.end());
	/////////////////////////////Index Buffer//////////////////////////////

	D3D11_BUFFER_DESC indexBufferDesc;
	indexBufferDesc.Usage               = D3D11_USAGE_IMMUTABLE;
	indexBufferDesc.ByteWidth           = sizeof(UINT) * totalIndexCount;
	indexBufferDesc.BindFlags           = D3D11_BIND_INDEX_BUFFER;
	indexBufferDesc.CPUAccessFlags      = 0;
	indexBufferDesc.MiscFlags           = 0;
	indexBufferDesc.StructureByteStride = 0;

	D3D11_SUBRESOURCE_DATA IBO;
	IBO.pSysMem = &IndexData[0];
	hr = pD3D11Device->CreateBuffer(&indexBufferDesc, &IBO, &m_pIndexBuffer);
	DebugHR(hr);


}

void Object::init_shader(ID3D11Device *pD3D11Device, HWND hWnd)
{
	DWORD shaderFlags = 0;
#if defined( DEBUG ) || defined( _DEBUG )
	shaderFlags |= D3D10_SHADER_DEBUG;
	shaderFlags |= D3D10_SHADER_SKIP_OPTIMIZATION;
#endif

	ID3D10Blob* compiledShader = 0;
	ID3D10Blob* compilationMsgs = 0;
	HRESULT hr = D3DX11CompileFromFile(L"model.fx", 0, 0, 0, "fx_5_0", shaderFlags,
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

	m_pEffectTechnique = m_pEffect->GetTechniqueByName("ModelTech");


	m_pWorld = m_pEffect->GetVariableByName("g_World")->AsMatrix();
	m_pView  = m_pEffect->GetVariableByName("g_View")->AsMatrix();
	m_pProj  = m_pEffect->GetVariableByName("g_Proj")->AsMatrix();

	// Done with compiled shader.
	ReleaseCOM(compiledShader);

	D3D11_INPUT_ELEMENT_DESC vertexDesc[] =
	{
		{"POSITION", 0, DXGI_FORMAT_R32G32B32_FLOAT, 0, 0, D3D11_INPUT_PER_VERTEX_DATA, 0},
		{"NORMAL", 0, DXGI_FORMAT_R32G32B32_FLOAT, 0, 12, D3D11_INPUT_PER_VERTEX_DATA, 0}
	};

	D3DX11_PASS_DESC passDesc;
	m_pEffectTechnique->GetPassByIndex(0)->GetDesc(&passDesc);

	pD3D11Device->CreateInputLayout(vertexDesc, 2, passDesc.pIAInputSignature,
		passDesc.IAInputSignatureSize, &m_pInputLayout);


}

}