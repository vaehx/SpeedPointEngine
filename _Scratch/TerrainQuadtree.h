// Terrain Quadtree

#include <memory>


template<typename F>
struct Vec3
{
	F x, y, z;

	Vec3() : x(0), y(0), z(0) {}
	Vec3(const Vec3<F>& v) : x(v.x), y(v.y), z(v.z) {}
	Vec3(F xx, F yy, F zz) : x(xx), y(yy), z(zz) {}

	Vec3<F> operator - (const Vec3<F>& v) const { return Vec3<F>(x - v.x, y - v.y, z - v.z); }
	Vec3<F> operator * (const F& k) const { return Vec3<F>(x * k, y * k, z * k); }
};

typedef Vec3<float> Vec3f;

struct AABB
{
	Vec3f min, max;

	AABB() {}
	AABB(const Vec3f& vMin, const Vec3f& vMax)
		: min(vMin),
		max(vMax) {}
	AABB(const AABB& aabb)
		: min(aabb.min),
		max(aabb.max) {}

	Vec3f GetCenter() const { return (max - min) * 0.5f; }

	bool ContainsPoint(const Vec3f& v) const
	{
		return v.x >= min.x && v.x <= max.x
			&& v.y >= min.y && v.y <= max.y
			&& v.z >= min.z && v.z <= max.z;
	}

	bool ContainsTriangle(const Vec3f& v1, const Vec3f& v2, const Vec3f& v3) const
	{
		return ContainsPoint(v1) || ContainsPoint(v2) || ContainsPoint(v3);
	}
};


struct SVertex
{
	float x, y, z;
};

typedef unsigned long SIndex;


struct STerrainQuadtree
{
	STerrainQuadtree* pParentTree;
	STerrainQuadtree* pSubTrees[4];
	unsigned long* pTriangles; // Stores indices to first index of each triangle
	unsigned int nTriangles;
	AABB aabb;


	STerrainQuadtree()
		: pParentTree(0),
		pTriangles(0),
		nTriangles(0)
	{
	}

	~STerrainQuadtree()
	{
		if (pTriangles)
			delete[] pTriangles;

		pTriangles = 0;
		nTriangles = 0;
		pParentTree = 0;

		for (unsigned int i = 0; i < 4; ++i)
		{
			if (pSubTrees[i])
			{
				delete pSubTrees[i];
				pSubTrees[i] = 0;
			}
		}
	}

	inline bool IsLeave() const
	{
		return pTriangles;
	}

	// assumes consistent triangle polygons
	void Create(const AABB& boundbox, unsigned int maxDepth,
		const SIndex* pIndices, const unsigned long nIndices,
		const SVertex* pVertices, const unsigned long nVertices,
		STerrainQuadtree* parent = 0, unsigned int curDepth = 0)
	{
		if (maxDepth == 0 || curDepth > maxDepth)
			return;

		aabb = boundbox;
		pParentTree = parent;

		if (curDepth < maxDepth)
		{
			// subtree / no leave

			Vec3f bbCenter = boundbox.GetCenter();
			AABB subTreeAABB[4] = {
				AABB(boundbox.min, Vec3f(bbCenter.x, boundbox.max.y, bbCenter.z)),
				AABB(Vec3f(bbCenter.x, boundbox.min.y, boundbox.min.z), Vec3f(boundbox.max.x, boundbox.max.y, bbCenter.z)),
				AABB(Vec3f(boundbox.min.x, boundbox.min.y, bbCenter.z), Vec3f(bbCenter.x, boundbox.max.y, boundbox.max.z)),
				AABB(Vec3f(bbCenter.x, boundbox.min.y, bbCenter.z), boundbox.max)
			};

			for (unsigned int i = 0; i < 4; ++i)
			{
				pSubTrees[i] = new STerrainQuadtree();
				pSubTrees[i]->Create(subTreeAABB[i], maxDepth, pIndices, nIndices, pVertices, nVertices, this, curDepth + 1);
			}
		}
		else
		{
			// is leave

			// make sure to set subtrees to 0
			for (unsigned int i = 0; i < 4; ++i)
				pSubTrees[i] = 0;

			// nIndicesSafe % 3 = 0
			unsigned long nIndicesSafe = nIndices - (nIndices % 3);

			pTriangles = new unsigned long[nIndicesSafe / 3];
			nTriangles = 0;

			// find tris inside aabb.
			for (unsigned long iTriIdx = 0; iTriIdx < nIndicesSafe; iTriIdx += 3)
			{
				const SVertex& v1 = pVertices[pIndices[iTriIdx]];
				const SVertex& v2 = pVertices[pIndices[iTriIdx + 1]];
				const SVertex& v3 = pVertices[pIndices[iTriIdx + 2]];

				if (aabb.ContainsTriangle(Vec3f(v1.x, v1.y, v1.z), Vec3f(v2.x, v2.y, v2.z), Vec3f(v3.x, v3.y, v3.z)))
				{
					pTriangles[nTriangles] = iTriIdx;
					++nTriangles;
				}
			}

			// shrink pTriangles
			if (nTriangles < (unsigned long)(nIndicesSafe / 3))
			{
				unsigned long* pTrianglesNew = new unsigned long[nTriangles];
				memcpy(pTrianglesNew, pTriangles, sizeof(unsigned long) * nTriangles);
				delete[] pTriangles;
				pTriangles = pTrianglesNew;
			}
		}
	}

	// returns false if at least one subtree is instanciated
	bool IsLeave() const
	{
		for (unsigned int i = 0; i < 0; ++i)
		{
			if (pSubTrees[i])
				return false;
		}
	}

	// Find Leave-Quadtree with p in it
	STerrainQuadtree* Find(const Vec3f& p)
	{
		if (!aabb.ContainsPoint(p))
			return 0;

		if (IsLeave())
			return this;

		for (unsigned int i = 0; i < 4; ++i)
		{
			if (!pSubTrees[i])
				continue;

			STerrainQuadtree* pCheck = pSubTrees[i]->Find(p);
			if (pCheck)
				return pCheck;
		}

		return 0; // not found
	}
};