#include "Model.h"
#include <iostream>
#include <fstream>
#include <string>

namespace byhj
{


void Model::Init(ID3D11Device *pD3D11Device, ID3D11DeviceContext *pD3D11DeviceContext, HWND hWnd)
{
	init_buffer(pD3D11Device, pD3D11DeviceContext);
	init_shader(pD3D11Device, hWnd);
	m_EffectHelper.Init(pD3D11Device);

}

void Model::Render(ID3D11DeviceContext *pD3D11DeviceContext, const d3d::MatrixBuffer &matrix, d3d::Camera *camera)
{
		//
	// Switch the number of lights based on key presses.
	//
	if (GetAsyncKeyState('0') & 0x8000)
		mLightCount = 0;

	if (GetAsyncKeyState('1') & 0x8000)
		mLightCount = 1;

	if (GetAsyncKeyState('2') & 0x8000)
		mLightCount = 2;

	if (GetAsyncKeyState('3') & 0x8000)
		mLightCount = 3;
	m_EffectHelper.Render(pD3D11DeviceContext);
	pD3D11DeviceContext->IASetPrimitiveTopology(D3D11_PRIMITIVE_TOPOLOGY_TRIANGLELIST);

	XMMATRIX world;
	cbMatrix.model = matrix.model;
	cbMatrix.view  = matrix.view;
	cbMatrix.proj  = matrix.proj;

	m_EffectHelper.SetWorld(cbMatrix.model);
	m_EffectHelper.SetView(cbMatrix.view);
	m_EffectHelper.SetProj(cbMatrix.proj);

	cbLight.g_EyePos  = camera->GetCamPos();
	m_EffectHelper.SetDirLight(m_DirLights);
	m_EffectHelper.SetEyePos(cbLight.g_EyePos);

	D3DX11_TECHNIQUE_DESC techDesc;
	ID3DX11EffectTechnique* activeTech = m_EffectHelper.m_pEffectTech1 ;
	switch ( mLightCount )
	{
	case 1:
		activeTech = m_EffectHelper.m_pEffectTech1;
		break;
	case 2:
		activeTech = m_EffectHelper.m_pEffectTech2;
		break;
	case 3:
		activeTech = m_EffectHelper.m_pEffectTech3;
		break;
	}
	activeTech->GetDesc(&techDesc);

	for ( UINT p = 0; p<techDesc.Passes; ++p )
	{
		// Set vertex buffer stride and offset
		unsigned int stride;
		unsigned int offset;
		stride = sizeof( Vertex );
		offset = 0;

		pD3D11DeviceContext->IASetVertexBuffers(0, 1, &m_pShapesVB, &stride, &offset);
		pD3D11DeviceContext->IASetIndexBuffer(m_pShapesIB, DXGI_FORMAT_R32_UINT, 0);

		// Draw the grid.
		world = XMLoadFloat4x4(&mGridWorld);
		XMStoreFloat4x4(&cbMatrix.model, world);
		m_EffectHelper.SetWorld(cbMatrix.model);
		m_EffectHelper.SetMaterial(m_GridMat);
		activeTech->GetPassByIndex(p)->Apply(0, pD3D11DeviceContext);
		pD3D11DeviceContext->DrawIndexed(mGridIndexCount, mGridIndexOffset, mGridVertexOffset);

		// Draw the box.
		world = XMLoadFloat4x4(&mBoxWorld);
		XMStoreFloat4x4(&cbMatrix.model, world);
		m_EffectHelper.SetWorld(cbMatrix.model);
		m_EffectHelper.SetMaterial(m_BoxMat);
		activeTech->GetPassByIndex(p)->Apply(0, pD3D11DeviceContext);
		pD3D11DeviceContext->DrawIndexed(mBoxIndexCount, mBoxIndexOffset, mBoxVertexOffset);

		// Draw the cylinders.
		m_EffectHelper.SetMaterial(m_CylinderMat);
		for ( int i = 0; i<10; ++i )
		{
			world = XMLoadFloat4x4(&mCylWorld[i]);
			XMStoreFloat4x4(&cbMatrix.model, world);
			m_EffectHelper.SetWorld(cbMatrix.model);
			activeTech->GetPassByIndex(p)->Apply(0, pD3D11DeviceContext);
			pD3D11DeviceContext->DrawIndexed(mCylinderIndexCount, mCylinderIndexOffset, mCylinderVertexOffset);
		}

		// Draw the spheres.
		m_EffectHelper.SetMaterial(m_SphereMat);
		for ( int i = 0; i<10; ++i )
		{
			world = XMLoadFloat4x4(&mSphereWorld[i]);
			XMStoreFloat4x4(&cbMatrix.model, world);
			m_EffectHelper.SetWorld(cbMatrix.model);
			activeTech->GetPassByIndex(p)->Apply(0, pD3D11DeviceContext);
			pD3D11DeviceContext->DrawIndexed(mSphereIndexCount, mSphereIndexOffset, mSphereVertexOffset);
		}


		//Draw the skull
		pD3D11DeviceContext->IASetVertexBuffers(0, 1, &m_pSkullVB, &stride, &offset);
		pD3D11DeviceContext->IASetIndexBuffer(m_pSkullIB, DXGI_FORMAT_R32_UINT, 0);
		world = XMLoadFloat4x4(&mSkullWorld);
		XMStoreFloat4x4(&cbMatrix.model, world);
		m_EffectHelper.SetWorld(cbMatrix.model);
		m_EffectHelper.SetMaterial(m_SkullMat);
		activeTech->GetPassByIndex(p)->Apply(0, pD3D11DeviceContext);
		pD3D11DeviceContext->DrawIndexed(m_IndexCount, 0, 0);
		
	}

}


void Model::init_light()
{
	m_DirLights[0].Ambient   = XMFLOAT4(0.2f, 0.2f, 0.2f, 1.0f);
	m_DirLights[0].Diffuse   = XMFLOAT4(0.5f, 0.5f, 0.5f, 1.0f);
	m_DirLights[0].Specular  = XMFLOAT4(0.5f, 0.5f, 0.5f, 1.0f);
	m_DirLights[0].Direction = XMFLOAT3(0.57735f, -0.57735f, 0.57735f);

	m_DirLights[1].Ambient  = XMFLOAT4(0.0f, 0.0f, 0.0f, 1.0f);
	m_DirLights[1].Diffuse  = XMFLOAT4(0.20f, 0.20f, 0.20f, 1.0f);
	m_DirLights[1].Specular = XMFLOAT4(0.25f, 0.25f, 0.25f, 1.0f);
	m_DirLights[1].Direction = XMFLOAT3(-0.57735f, -0.57735f, 0.57735f);

	m_DirLights[2].Ambient  = XMFLOAT4(0.0f, 0.0f, 0.0f, 1.0f);
	m_DirLights[2].Diffuse  = XMFLOAT4(0.2f, 0.2f, 0.2f, 1.0f);
	m_DirLights[2].Specular = XMFLOAT4(0.0f, 0.0f, 0.0f, 1.0f);
	m_DirLights[2].Direction = XMFLOAT3(0.0f, -0.707f, -0.707f);

	m_GridMat.Ambient  = XMFLOAT4(0.48f, 0.77f, 0.46f, 1.0f);
	m_GridMat.Diffuse  = XMFLOAT4(0.48f, 0.77f, 0.46f, 1.0f);
	m_GridMat.Specular = XMFLOAT4(0.2f, 0.2f, 0.2f, 16.0f);

	m_CylinderMat.Ambient  = XMFLOAT4(0.7f, 0.85f, 0.7f, 1.0f);
	m_CylinderMat.Diffuse  = XMFLOAT4(0.7f, 0.85f, 0.7f, 1.0f);
	m_CylinderMat.Specular = XMFLOAT4(0.8f, 0.8f, 0.8f, 16.0f);

	m_SphereMat.Ambient  = XMFLOAT4(0.1f, 0.2f, 0.3f, 1.0f);
	m_SphereMat.Diffuse  = XMFLOAT4(0.2f, 0.4f, 0.6f, 1.0f);
	m_SphereMat.Specular = XMFLOAT4(0.9f, 0.9f, 0.9f, 16.0f);

	m_BoxMat.Ambient  = XMFLOAT4(0.651f, 0.5f, 0.392f, 1.0f);
	m_BoxMat.Diffuse  = XMFLOAT4(0.651f, 0.5f, 0.392f, 1.0f);
	m_BoxMat.Specular = XMFLOAT4(0.2f, 0.2f, 0.2f, 16.0f);

	m_SkullMat.Ambient  = XMFLOAT4(0.8f, 0.8f, 0.8f, 1.0f);
	m_SkullMat.Diffuse  = XMFLOAT4(0.8f, 0.8f, 0.8f, 1.0f);
	m_SkullMat.Specular = XMFLOAT4(0.8f, 0.8f, 0.8f, 16.0f);
}


void Model::init_buffer(ID3D11Device *pD3D11Device, ID3D11DeviceContext *pD3D11DeviceContext)
{	
	init_light();

	XMMATRIX I = XMMatrixIdentity();
	XMStoreFloat4x4(&mGridWorld, I);

	XMMATRIX boxScale = XMMatrixScaling(3.0f, 1.0f, 3.0f);
	XMMATRIX boxOffset = XMMatrixTranslation(0.0f, 0.5f, 0.0f);
	XMStoreFloat4x4(&mBoxWorld, XMMatrixMultiply(boxScale, boxOffset));

	XMMATRIX skullScale = XMMatrixScaling(0.5f, 0.5f, 0.5f);
	XMMATRIX skullOffset = XMMatrixTranslation(0.0f, 1.0f, 0.0f);
	XMStoreFloat4x4(&mSkullWorld, XMMatrixMultiply(skullScale, skullOffset));

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

	D3D11_BUFFER_DESC shapesVBDesc;
	shapesVBDesc.Usage               = D3D11_USAGE_IMMUTABLE;
	shapesVBDesc.ByteWidth           = sizeof(Vertex) * totalVertexCount;
	shapesVBDesc.BindFlags           = D3D11_BIND_VERTEX_BUFFER;
	shapesVBDesc.CPUAccessFlags      = 0;
	shapesVBDesc.MiscFlags           = 0;
	shapesVBDesc.StructureByteStride = 0;

	D3D11_SUBRESOURCE_DATA shapeVBO;
	shapeVBO.pSysMem = &vertices[0];
	hr = pD3D11Device->CreateBuffer(&shapesVBDesc, &shapeVBO, &m_pShapesVB);
	//DebugHR(hr);

	std::vector<UINT> IndexData;
	IndexData.insert(IndexData.end(), box.IndexData.begin(), box.IndexData.end());
	IndexData.insert(IndexData.end(), grid.IndexData.begin(), grid.IndexData.end());
	IndexData.insert(IndexData.end(), sphere.IndexData.begin(), sphere.IndexData.end());
	IndexData.insert(IndexData.end(), cylinder.IndexData.begin(), cylinder.IndexData.end());
	/////////////////////////////Index Buffer//////////////////////////////

	D3D11_BUFFER_DESC shapeIBDesc;
	shapeIBDesc.Usage               = D3D11_USAGE_IMMUTABLE;
	shapeIBDesc.ByteWidth           = sizeof(UINT) * totalIndexCount;
	shapeIBDesc.BindFlags           = D3D11_BIND_INDEX_BUFFER;
	shapeIBDesc.CPUAccessFlags      = 0;
	shapeIBDesc.MiscFlags           = 0;
	shapeIBDesc.StructureByteStride = 0;

	D3D11_SUBRESOURCE_DATA shapeIBO;
	shapeIBO.pSysMem = &IndexData[0];
	hr = pD3D11Device->CreateBuffer(&shapeIBDesc, &shapeIBO, &m_pShapesIB);
	//DebugHR(hr);

	////////////////////////////////////Skull/////////////////////////////////////////

	//Load the model file
	std::ifstream fin("../../media/objects/skull.txt");
	if (!fin)
	{
		MessageBox(NULL, L"Can not open the model file!", 0, 0);
		return; 
	}
	std::string ignore;
	fin >> ignore >> m_VertexCount;
	fin >> ignore >> m_IndexCount;
	fin >> ignore >> ignore >> ignore >> ignore;

	m_VertexData.resize(m_VertexCount);
	for (UINT i = 0; i != m_VertexCount; ++i)
	{
		fin >> m_VertexData[i].Pos.x 
			>> m_VertexData[i].Pos.y
			>> m_VertexData[i].Pos.z;

		fin >> m_VertexData[i].Normal.x 
			>> m_VertexData[i].Normal.y
			>> m_VertexData[i].Normal.z;
	}
	fin >> ignore >> ignore >> ignore;
	/////////////////////////////Vertex Buffer//////////////////////////////

	D3D11_BUFFER_DESC skullVBDesc;
	skullVBDesc.Usage               = D3D11_USAGE_IMMUTABLE;
	skullVBDesc.ByteWidth           = sizeof(Vertex) * m_VertexCount;
	skullVBDesc.BindFlags           = D3D11_BIND_VERTEX_BUFFER;
	skullVBDesc.CPUAccessFlags      = 0;
	skullVBDesc.MiscFlags           = 0;
	skullVBDesc.StructureByteStride = 0;

	D3D11_SUBRESOURCE_DATA skullVBO;
	skullVBO.pSysMem = &m_VertexData[0];
	hr = pD3D11Device->CreateBuffer(&skullVBDesc, &skullVBO, &m_pSkullVB);
	//DebugHR(hr);

	/////////////////////////////Index Buffer//////////////////////////////

	m_IndexCount *= 3;
	m_IndexData.resize(m_IndexCount);
	for (UINT i = 0; i != m_IndexCount; ++i)
		fin >> m_IndexData[i];
	fin.close();

	D3D11_BUFFER_DESC skullIBDesc;
	skullIBDesc.Usage               = D3D11_USAGE_IMMUTABLE;
	skullIBDesc.ByteWidth           = sizeof(UINT) * m_IndexCount;
	skullIBDesc.BindFlags           = D3D11_BIND_INDEX_BUFFER;
	skullIBDesc.CPUAccessFlags      = 0;
	skullIBDesc.MiscFlags           = 0;
	skullIBDesc.StructureByteStride = 0;

	D3D11_SUBRESOURCE_DATA skullIBO;
	skullIBO.pSysMem = &m_IndexData[0];
	hr = pD3D11Device->CreateBuffer(&skullIBDesc, &skullIBO, &m_pSkullIB);
	//DebugHR(hr);
	////////////////////////////////Const Buffer//////////////////////////////////////


}


void Model::init_shader(ID3D11Device *pD3D11Device, HWND hWnd)
{

}


}