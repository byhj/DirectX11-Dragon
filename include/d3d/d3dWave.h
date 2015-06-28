#ifndef D3DWAVE_H
#define D3DWAVE_H

#include <Windows.h>
#include <xnamath.h>

class D3DWave
{
public:
	D3DWave();
	~D3DWave();

	UINT RowCount()const;
	UINT ColumnCount()const;
	UINT VertexCount()const;
	UINT TriangleCount()const;

	// Returns the solution at the ith grid point.
	const XMFLOAT3& operator[](int i)const { return mCurrSolution[i]; }

	void Init(UINT m, UINT n, float dx, float dt, float speed, float damping);
	void Update(float dt);
	void Disturb(UINT i, UINT j, float magnitude);

private:
	UINT mNumRows;
	UINT mNumCols;

	UINT mVertexCount;
	UINT mTriangleCount;

	// Simulation constants we can precompute.
	float mK1;
	float mK2;
	float mK3;

	float mTimeStep;
	float mSpatialStep;

	XMFLOAT3* mPrevSolution;
	XMFLOAT3* mCurrSolution;
};

#endif 