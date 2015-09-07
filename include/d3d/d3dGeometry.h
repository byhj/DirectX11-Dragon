#ifndef D3DGEOMETRY_H
#define D3DGEOMETRY_H

#include <windows.h>
#include <vector>
#include <algorithm>

#include <xnamath.h>

class d3d::Geometry
{
public:
	d3d::Geometry() {}
   ~d3d::Geometry() {}

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
	void CreateGrid(float width, float depth, UINT m, UINT n, MeshData& mesh);

	/// Creates a sphere centered at the origin with the given radius.  The
	/// slices and stacks parameters control the degree of tessellation.

	void CreateSphere(float radius, UINT sliceCount, UINT stackCount, MeshData& mesh);

	/// Creates a geosphere centered at the origin with the given radius.  The
	/// depth controls the level of tessellation.

	void CreateGeosphere(float radius, UINT numSubdivisions, MeshData& meshData);

	/// Creates a cylinder parallel to the y-axis, and centered about the origin.  
	/// The bottom and top radius can vary to form various cone shapes rather than true
	// cylinders.  The slices and stacks parameters control the degree of tessellation.

	void CreateCylinder(float bottomRadius, float topRadius, float height, UINT sliceCount, UINT stackCount, MeshData& mesh);

private:
	void Subdivide(MeshData& meshData);
	void BuildCylinderTopCap(float bottomRadius, float topRadius, float height, UINT sliceCount, UINT stackCount, MeshData& meshData);
	void BuildCylinderBottomCap(float bottomRadius, float topRadius, float height, UINT sliceCount, UINT stackCount, MeshData& meshData);

};
#endif