#include "Geometry.h"
#include "d3d/d3dUtil.h"

namespace byhj
{

void Geometry::Init(ID3D11Device *pD3D11Device, ID3D11DeviceContext *pD3D11DeviceContext, HWND hWnd)
{
	init_buffer(pD3D11Device, pD3D11DeviceContext);
	init_shader(pD3D11Device, hWnd);
}


void Geometry::init_buffer(ID3D11Device *pD3D11Device, ID3D11DeviceContext *pD3D11DeviceContext)
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
	D3DGeometry::MeshData box;
	D3DGeometry::MeshData grid;
	D3DGeometry::MeshData sphere;
	D3DGeometry::MeshData cylinder;
	D3DGeometry geoGen;
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

	////////////////////////////////Const Buffer//////////////////////////////////////

	D3D11_BUFFER_DESC mvpDesc;	
	ZeroMemory(&mvpDesc, sizeof(D3D11_BUFFER_DESC));
	mvpDesc.Usage          = D3D11_USAGE_DEFAULT;
	mvpDesc.ByteWidth      = sizeof(MatrixBuffer);
	mvpDesc.BindFlags      = D3D11_BIND_CONSTANT_BUFFER;
	mvpDesc.CPUAccessFlags = 0;
	mvpDesc.MiscFlags      = 0;
	hr = pD3D11Device->CreateBuffer(&mvpDesc, NULL, &m_pMVPBuffer);
	DebugHR(hr);
}

void Geometry::init_shader(ID3D11Device *pD3D11Device, HWND hWnd)
{
	D3D11_INPUT_ELEMENT_DESC pInputLayoutDesc[2];
	pInputLayoutDesc[0].SemanticName         = "POSITION";
	pInputLayoutDesc[0].SemanticIndex        = 0;
	pInputLayoutDesc[0].Format               = DXGI_FORMAT_R32G32B32_FLOAT;
	pInputLayoutDesc[0].InputSlot            = 0;
	pInputLayoutDesc[0].AlignedByteOffset    = 0;
	pInputLayoutDesc[0].InputSlotClass       = D3D11_INPUT_PER_VERTEX_DATA;
	pInputLayoutDesc[0].InstanceDataStepRate = 0;

	pInputLayoutDesc[1].SemanticName         = "NORMAL";
	pInputLayoutDesc[1].SemanticIndex        = 0;
	pInputLayoutDesc[1].Format               = DXGI_FORMAT_R32G32B32_FLOAT;
	pInputLayoutDesc[1].InputSlot            = 0;
	pInputLayoutDesc[1].AlignedByteOffset    = 12;
	pInputLayoutDesc[1].InputSlotClass       = D3D11_INPUT_PER_VERTEX_DATA;
	pInputLayoutDesc[1].InstanceDataStepRate = 0;

	unsigned numElements = ARRAYSIZE(pInputLayoutDesc);

	CubeShader.init(pD3D11Device, hWnd);
	CubeShader.attachVS(L"model.vsh", pInputLayoutDesc, numElements);
	CubeShader.attachPS(L"model.psh");
	CubeShader.end();
}

}