#include "d3dGeometry.h"

void D3DGeometry::CreateCube(float width, float height, float depth, MeshData &mesh)
{
	Vertex vertex[24];

	float w2 = 0.5f * width;
	float h2 = 0.5f * height;
	float d2 = 0.5f * depth;
	    
	//                    Position         Normal          TexCoord
	// Fill in the front face vertex data.
	vertex[0] = Vertex(-w2, -h2, -d2,  0.0f, 0.0f, -1.0f,  0.0f, 1.0f);
	vertex[1] = Vertex(-w2, +h2, -d2,  0.0f, 0.0f, -1.0f,  0.0f, 0.0f);
	vertex[2] = Vertex(+w2, +h2, -d2,  0.0f, 0.0f, -1.0f,  1.0f, 0.0f);
	vertex[3] = Vertex(+w2, -h2, -d2,  0.0f, 0.0f, -1.0f,  1.0f, 1.0f);

	// Fill in the back face vertex data.
	vertex[4] = Vertex(-w2, -h2, +d2,  0.0f, 0.0f, 1.0f,  1.0f, 1.0f);
	vertex[5] = Vertex(+w2, -h2, +d2,  0.0f, 0.0f, 1.0f,  0.0f, 1.0f);
	vertex[6] = Vertex(+w2, +h2, +d2,  0.0f, 0.0f, 1.0f,  0.0f, 0.0f);
	vertex[7] = Vertex(-w2, +h2, +d2,  0.0f, 0.0f, 1.0f,  1.0f, 0.0f);

	// Fill in the top face vertex data.
	vertex[8]  = Vertex(-w2, +h2, -d2,  0.0f, 1.0f, 0.0f,  0.0f, 1.0f);
	vertex[9]  = Vertex(-w2, +h2, +d2,  0.0f, 1.0f, 0.0f,  0.0f, 0.0f);
	vertex[10] = Vertex(+w2, +h2, +d2,  0.0f, 1.0f, 0.0f,  1.0f, 0.0f);
	vertex[11] = Vertex(+w2, +h2, -d2,  0.0f, 1.0f, 0.0f,  1.0f, 1.0f);

	// Fill in the bottom face vertex data.
	vertex[12] = Vertex(-w2, -h2, -d2,  0.0f, -1.0f, 0.0f,  1.0f, 1.0f);
	vertex[13] = Vertex(+w2, -h2, -d2,  0.0f, -1.0f, 0.0f,  0.0f, 1.0f);
	vertex[14] = Vertex(+w2, -h2, +d2,  0.0f, -1.0f, 0.0f,  0.0f, 0.0f);
	vertex[15] = Vertex(-w2, -h2, +d2,  0.0f, -1.0f, 0.0f,  1.0f, 0.0f);

	// Fill in the left face vertex data.
	vertex[16] = Vertex(-w2, -h2, +d2,  -1.0f, 0.0f, 0.0f,  0.0f, 1.0f);
	vertex[17] = Vertex(-w2, +h2, +d2,  -1.0f, 0.0f, 0.0f,  0.0f, 0.0f);
	vertex[18] = Vertex(-w2, +h2, -d2,  -1.0f, 0.0f, 0.0f,  1.0f, 0.0f);
	vertex[19] = Vertex(-w2, -h2, -d2,  -1.0f, 0.0f, 0.0f,  1.0f, 1.0f);

	// Fill in the right face vertex data.
	vertex[20] = Vertex(+w2, -h2, -d2,   1.0f, 0.0f, 0.0f,  0.0f, 1.0f);
	vertex[21] = Vertex(+w2, +h2, -d2,   1.0f, 0.0f, 0.0f,  0.0f, 0.0f);
	vertex[22] = Vertex(+w2, +h2, +d2,   1.0f, 0.0f, 0.0f,  1.0f, 0.0f);
	vertex[23] = Vertex(+w2, -h2, +d2,   1.0f, 0.0f, 0.0f,  1.0f, 1.0f);

	mesh.VertexData.assign(vertex, vertex+24);

	//////////////////////////Index////////////////////////
	UINT index[36];
	// Fill in the front face index data
	index[0] = 0; index[1] = 1; index[2] = 2;
	index[3] = 0; index[4] = 2; index[5] = 3;

	// Fill in the back face index data
	index[6] = 4; index[7]  = 5; index[8]  = 6;
	index[9] = 4; index[10] = 6; index[11] = 7;

	// Fill in the top face index data
	index[12] = 8; index[13] =  9; index[14] = 10;
	index[15] = 8; index[16] = 10; index[17] = 11;

	// Fill in the bottom face index data
	index[18] = 12; index[19] = 13; index[20] = 14;
	index[21] = 12; index[22] = 14; index[23] = 15;

	// Fill in the left face index data
	index[24] = 16; index[25] = 17; index[26] = 18;
	index[27] = 16; index[28] = 18; index[29] = 19;

	// Fill in the right face index data
	index[30] = 20; index[31] = 21; index[32] = 22;
	index[33] = 20; index[34] = 22; index[35] = 23;

	mesh.IndexData.assign(index, index + 36);
}

//void CreateSphere(int radius, MeshData &mesh);

void D3DGeometry::CreatePlane(float width, float depth, MeshData &mesh)
{

	float w2 = width  * 0.5f;
	float d2 = depth * 0.5f;

	Vertex vertex[4];
	vertex[0] = Vertex(-w2, 0.0f, -d2,  0.0f, 1.0f, 0.0f,  0.0f, 1.0f);
	vertex[1] = Vertex(-w2, 0.0f,  d2,  0.0f, 1.0f, 0.0f,  0.0f, 0.0f);
	vertex[2] = Vertex( w2, 0.0f,  d2,  0.0f, 1.0f, 0.0f,  1.0f, 0.0f);
	vertex[3] = Vertex( w2, 0.0f, -d2,  0.0f, 1.0f, 0.0f,  1.0f, 1.0f);

	mesh.VertexData.assign(vertex, vertex + 4);

	UINT index[6];
	index[0] = 0; 
	index[1] = 1;
	index[2] = 2;
	index[3] = 2;
	index[4] = 3;
	index[5] = 4;

	mesh.IndexData.assign(index, index + 6);
}

void D3DGeometry::CreateGird(float width, float depth, UINT m, UINT n, MeshData& mesh)
{
	UINT vertexCount = m * n;
	UINT faceCount   = (m-1) * (n-1) * 2;

	float halfWidth = 0.5f * width;
	float halfDepth = 0.5f * depth;

	float dx = width / (n-1);
	float dz = depth / (m-1);
	float du = 1.0f / (n-1);
	float dv = 1.0f / (m-1);

	mesh.VertexData.resize(vertexCount);
	for(UINT i = 0; i < m; ++i)
	{
		float z = halfDepth - i * dz;
		for(UINT j = 0; j < n; ++j)
		{
			float x = -halfWidth + j * dx;

			mesh.VertexData[i*n + j].Pos      = XMFLOAT3(x, 0.0f, z);
			mesh.VertexData[i*n + j].Normal   = XMFLOAT3(0.0f, 1.0f, 0.0f);
			mesh.VertexData[i*n + j].Tex.x    = j * du;
			mesh.VertexData[i*n + j].Tex.y    = i * dv;
		}
	}

	mesh.IndexData.resize(faceCount * 3); // 3 indices per face
	UINT k = 0;
	for(UINT i = 0; i < m-1; ++i)
	{
		for(UINT j = 0; j < n-1; ++j)
		{
			mesh.IndexData[k]     = i*n + j;
			mesh.IndexData[k + 1] = i*n + j + 1;
			mesh.IndexData[k + 2] = (i + 1)*n + j;

			mesh.IndexData[k + 3] = (i + 1)*n + j;
			mesh.IndexData[k + 4] = i*n + j + 1;
			mesh.IndexData[k + 5] = (i + 1)*n + j + 1;

			k += 6; // next quad
		}
	}
}
