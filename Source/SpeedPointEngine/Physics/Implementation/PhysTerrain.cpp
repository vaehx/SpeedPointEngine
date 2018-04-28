///////////////////////////////////////////////////////////////////////////////////////////////////////////////////////
//
//	SpeedPoint Game Engine
//	Copyright (c) 2011-2017 Pascal Rosenkranz, All rights reserved.
//
///////////////////////////////////////////////////////////////////////////////////////////////////////////////////////

#include "PhysTerrain.h"
#include "PhysDebug.h"
#include <Common\Vector2.h>
#include <stack>

#define TERRAIN_PROXY_Y_BOUNDS_BIAS 0.5f

SP_NMSPACE_BEG

inline float SampleHeightmap(const float* heightmap, const Vec2f& tc, const unsigned int heightmapSz[2])
{
	unsigned int pc[2];
	pc[0] = (unsigned int)(tc.x * (float)heightmapSz[0]) % heightmapSz[0];
	pc[1] = (unsigned int)(tc.y * (float)heightmapSz[1]) % heightmapSz[1];
	return heightmap[pc[1] * heightmapSz[0] + pc[0]];
}

void UpdatePhysTerrainProxyNodeYBounds(geo::mesh_tree_node* pnode, float miny, float maxy, float bias = 0.0f)
{
	//	 Assuming it's a quad-tree !!
	pnode->aabb.vMin.y = miny - bias;
	pnode->aabb.vMax.y = maxy + bias;

	if (pnode->pchildren)
	{
		for (unsigned int i = 0; i < pnode->num_children; ++i)
			UpdatePhysTerrainProxyNodeYBounds(&pnode->pchildren[i], miny, maxy, bias);
	}
}

///////////////////////////////////////////////////////////////////////////////////////////////////////////////////////

S_API PhysTerrain::PhysTerrain()
{
}

///////////////////////////////////////////////////////////////////////////////////////////////////////////////////////

inline void DumpMeshTree(const geo::mesh* pmesh)
{
	struct open_node
	{
		const geo::mesh_tree_node* pnode;
		unsigned int depth;
		open_node(const geo::mesh_tree_node* _pnode, unsigned int _depth) : pnode(_pnode), depth(_depth) {}
	};

	string indent;
	std::stack<open_node> openNodes;
	openNodes.emplace(&pmesh->root, 0);
	while (!openNodes.empty())
	{
		open_node node = openNodes.top();
		openNodes.pop();

		indent = "";
		for (unsigned int i = 0; i < node.depth; ++i)
			indent += "  ";

		CLog::Log(S_DEBUG, "  %s[%d tris]", indent.c_str(), node.pnode->tris.size());

		if (node.pnode->pchildren)
		{
			for (unsigned int i = 0; i < node.pnode->num_children; ++i)
				openNodes.emplace(&node.pnode->pchildren[i], node.depth + 1);
		}
	}
}

S_API void PhysTerrain::Create(const float* heightmap, unsigned int heightmapSz[2], const SPhysTerrainParams& params)
{
	if (!heightmap || params.segments[0] == 0 || params.segments[1] == 0)
	{
		CLog::Log(S_ERROR, "Failed PhysTerrain::Create(): Invalid parameters");
		return;
	}

	m_Params = params;

#define EXCESSIVE_PROXY_TREE_DEPTH 100
	if (m_Params.maxTrisPerLeaf > EXCESSIVE_PROXY_TREE_DEPTH)
	{
		CLog::Log(S_WARN, "PhysTerrain::Create(): maxTrisPerLeaf is excessively high (%d), setting to %d!",
			m_Params.maxTrisPerLeaf, EXCESSIVE_PROXY_TREE_DEPTH);

		m_Params.maxTrisPerLeaf = EXCESSIVE_PROXY_TREE_DEPTH;
	}

	SetBehavior(ePHYSOBJ_BEHAVIOR_STATIC);

	LARGE_INTEGER freq, start, end;
	QueryPerformanceFrequency(&freq);
	QueryPerformanceCounter(&start);

	geo::terrain_mesh* pmesh = new geo::terrain_mesh();
	pmesh->segmentsPerSide = params.segments[0];
	pmesh->num_points = (params.segments[1] + 1) * (params.segments[0] + 1);
	pmesh->points = new Vec3f[pmesh->num_points];
	pmesh->aabb.Reset();

	Vec2f segSz(params.size[0] / params.segments[0], params.size[1] / params.segments[1]);
	pmesh->segmentSz = segSz.x;

	Vec2f pixelSzTC = 1.0f / Vec2f((float)heightmapSz[0] - 1, (float)heightmapSz[1] - 1);
	Vec2f tc, remainder, roundedTC;
	float samples[4], h, y, miny = FLT_MAX, maxy = -FLT_MAX;
	unsigned int idx, vtx;
	for (unsigned int row = 0; row < (params.segments[1] + 1); ++row)
		for (unsigned int col = 0; col < (params.segments[0] + 1); ++col)
		{
			tc = Vec2f((float)col / (float)params.segments[0], (float)row / (float)params.segments[1]);
			tc -= (remainder = tc % pixelSzTC);

			remainder /= pixelSzTC;

			samples[0] = SampleHeightmap(heightmap, tc + Vec2f(-0.5f, -0.5f) * pixelSzTC, heightmapSz);
			samples[1] = SampleHeightmap(heightmap, tc + Vec2f( 0.5f, -0.5f) * pixelSzTC, heightmapSz);
			samples[2] = SampleHeightmap(heightmap, tc + Vec2f(-0.5f,  0.5f) * pixelSzTC, heightmapSz);
			samples[3] = SampleHeightmap(heightmap, tc + Vec2f( 0.5f,  0.5f) * pixelSzTC, heightmapSz);

			h = lerp(lerp(samples[0], samples[1], remainder.x), lerp(samples[2], samples[3], remainder.x), remainder.y);			
			y = h * params.heightScale;
			miny = min(miny, y);
			maxy = max(maxy, y);

			pmesh->points[vtx = (row * (params.segments[0] + 1) + col)] = Vec3f(col * segSz.x, y, row * segSz.y) + params.offset;
			pmesh->aabb.AddPoint(pmesh->points[vtx]);
		}

	QueryPerformanceCounter(&end);
	double elapsed = (double)(end.QuadPart - start.QuadPart) / freq.QuadPart;
	CLog::Log(S_DEBUG, "Created terrain proxy mesh in %.4f milliseconds", elapsed * 1000.0f);

	//DumpMeshTree(pmesh);
	//UpdatePhysTerrainProxyNodeYBounds(&pmesh->root, miny, maxy, TERRAIN_PROXY_Y_BOUNDS_BIAS);
	
	SetProxyPtr(pmesh);

	UpdateHelper();
}

///////////////////////////////////////////////////////////////////////////////////////////////////////////////////////

S_API void PhysTerrain::UpdateHeightmap(const float* heightmap, unsigned int heightmapSz[2], const AABB& bounds)
{
	if (!m_Proxy.pshape)
	{
		CLog::Log(S_ERROR, "Failed PhysTerrain::Update(): Create() never called");
		return;
	}

	if (!heightmap)
	{
		CLog::Log(S_ERROR, "Failed PhysTerrain::Update(): heightmap invalid");
		return;
	}

	const SPhysTerrainParams& params = m_Params;

	Vec2f segSz(params.size[0] / params.segments[0], params.size[1] / params.segments[1]);
	Vec2f pixelSzTC = 1.0f / Vec2f((float)heightmapSz[0], (float)heightmapSz[1]);
	Vec2f tc, remainder, roundedTC;
	float samples[4], h;

	unsigned int segMin[2], segMax[2];
	segMin[0] = (unsigned int)max(floorf((bounds.vMin.x - params.offset.x) / segSz.x), 0.0f);
	segMin[1] = (unsigned int)max(floorf((bounds.vMin.z - params.offset.z) / segSz.y), 0.0f);
	
	segMax[0] = (unsigned int)ceilf(min(bounds.vMax.x - params.offset.x, params.size[0]) / segSz.x);
	segMax[1] = (unsigned int)ceilf(min(bounds.vMax.z - params.offset.z, params.size[1]) / segSz.y);

	geo::terrain_mesh* pmesh = dynamic_cast<geo::terrain_mesh*>(m_Proxy.pshape);
	pmesh->aabb.Reset();
	unsigned int ipoint;
	for (unsigned int row = 0; row < (params.segments[1] + 1); ++row)
		for (unsigned int col = 0; col < (params.segments[0] + 1); ++col)
		{
			ipoint = row * (params.segments[0] + 1) + col;
			if (row >= segMin[1] && row <= segMax[1] && col >= segMin[0] && col <= segMax[0])
			{
				tc = Vec2f((float)col / params.segments[0], (float)row / params.segments[1]);
				tc += 1.0f - (remainder = tc % pixelSzTC);
				remainder /= pixelSzTC;

				// simulate bilinear filtering
				samples[0] = SampleHeightmap(heightmap, tc + Vec2f(-0.5f, -0.5f) * pixelSzTC, heightmapSz);
				samples[1] = SampleHeightmap(heightmap, tc + Vec2f(0.5f, -0.5f) * pixelSzTC, heightmapSz);
				samples[2] = SampleHeightmap(heightmap, tc + Vec2f(-0.5f, 0.5f) * pixelSzTC, heightmapSz);
				samples[3] = SampleHeightmap(heightmap, tc + Vec2f(0.5f, 0.5f) * pixelSzTC, heightmapSz);
				h = lerp(lerp(samples[0], samples[1], remainder.x), lerp(samples[2], samples[3], remainder.x), remainder.y);

				pmesh->points[ipoint].y = h * params.heightScale;
			}

			pmesh->aabb.AddPoint(pmesh->points[ipoint]);
		}

	// Update helper
	if (m_Proxy.phelper && m_Proxy.phelper->IsShown())
		m_Proxy.phelper->UpdateFromShape(pmesh, bounds);
}

///////////////////////////////////////////////////////////////////////////////////////////////////////////////////////

S_API void PhysTerrain::UpdateHelper()
{
}

///////////////////////////////////////////////////////////////////////////////////////////////////////////////////////

S_API void PhysTerrain::Clear()
{
	PhysObject::Clear();
	if (m_Proxy.phelper)
	{
		m_Proxy.phelper->Clear();
		delete m_Proxy.phelper;
		m_Proxy.phelper = 0;
	}
}

SP_NMSPACE_END
 