#ifndef D3DGEOMETRY_H
#define D3DGEOMETRY_H

#include <windows.h>
#include <vector>
#include <algorithm>
#include <DirectXMath.h>

using namespace DirectX;

class D3DGeometry
{
public:
	D3DGeometry() {}
   ~D3DGeometry() {}

public:
	struct  Vertex
	{
		Vertex() {}
		Vertex(const XMFLOAT3 &pos, const XMFLOAT3 &normal, const XMFLOAT2 &tc) 
		       :Pos(pos), Normal(normal), Tex(tc) {}

		Vertex(float px, float py, float pz, 
			   float nx, float ny, float nz,
			   float u, float v) : Pos(px, py, pz), Normal(nx, ny, nz), Tex(u, v) {}

	    XMFLOAT3 Pos;
		XMFLOAT3 Normal;
		XMFLOAT2 Tex;
	};

	struct MeshData
	{
        std::vector<Vertex> VertexData;
		std::vector<UINT> IndexData;
	};

	//void CreateSphere(int radius, MeshData &mesh);
	void CreatePlane(float width, float height, MeshData &mesh);
	void CreateCube(float widht, float height, float depth, MeshData &mesh);
	void CreateGird(float width, float depth, UINT m, UINT n, MeshData& mesh);
	//void CreateCylinder();

private:

};
#endif