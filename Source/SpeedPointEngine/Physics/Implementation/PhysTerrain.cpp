///////////////////////////////////////////////////////////////////////////////////////////////////////////////////////
//
//	SpeedPoint Game Engine
//	Copyright (c) 2011-2017 Pascal Rosenkranz, All rights reserved.
//
///////////////////////////////////////////////////////////////////////////////////////////////////////////////////////

#include "PhysTerrain.h"
#include "PhysDebug.h"
#include <Common\Vector2.h>

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

	if (pnode->children)
	{
		for (unsigned int i = 0; i < pnode->num_children; ++i)
			UpdatePhysTerrainProxyNodeYBounds(&pnode->children[i], miny, maxy, bias);
	}
}

///////////////////////////////////////////////////////////////////////////////////////////////////////////////////////

S_API void PhysTerrain::Create(const float* heightmap, unsigned int heightmapSz[2], const SPhysTerrainParams& params)
{
	if (!heightmap || params.segments[0] == 0 || params.segments[1] == 0)
	{
		CLog::Log(S_ERROR, "Failed PhysTerrain::Create(): Invalid parameters");
		return;
	}

	if (m_pShape)
		delete m_pShape;

	m_Params = params;

	SetUnmoveable();

	geo::mesh* pmesh = (geo::mesh*)(m_pShape = new geo::mesh());
	pmesh->transform = Mat44::Identity;
	pmesh->num_points = (params.segments[1] + 1) * (params.segments[0] + 1);
	pmesh->num_indices = (params.segments[1] * params.segments[0]) * 6;
	pmesh->points = new Vec3f[pmesh->num_points];	
	pmesh->indices = new unsigned int[pmesh->num_indices];

	Vec2f segSz(params.size[0] / params.segments[0], params.size[1] / params.segments[1]);
	Vec2f pixelSzTC = 1.0f / Vec2f((float)heightmapSz[0], (float)heightmapSz[1]);
	Vec2f tc, remainder, roundedTC;
	float samples[4], h, y, miny = FLT_MAX, maxy = -FLT_MAX;
	unsigned int idx, vtx;
	for (unsigned int row = 0; row < (params.segments[1] + 1); ++row)
		for (unsigned int col = 0; col < (params.segments[0] + 1); ++col)
		{
			tc = Vec2f((float)col / params.segments[0], (float)row / params.segments[1]);
			tc -= (remainder = tc % pixelSzTC);

			samples[0] = SampleHeightmap(heightmap, tc + Vec2f(-0.5f, -0.5f) * pixelSzTC, heightmapSz);
			samples[1] = SampleHeightmap(heightmap, tc + Vec2f( 0.5f, -0.5f) * pixelSzTC, heightmapSz);
			samples[2] = SampleHeightmap(heightmap, tc + Vec2f(-0.5f,  0.5f) * pixelSzTC, heightmapSz);
			samples[3] = SampleHeightmap(heightmap, tc + Vec2f( 0.5f,  0.5f) * pixelSzTC, heightmapSz);
			
			h = lerp(lerp(samples[0], samples[1], remainder.x), lerp(samples[2], samples[3], remainder.x), remainder.y);			
			y = h * params.heightScale;
			miny = min(miny, y);
			maxy = max(maxy, y);

			pmesh->points[vtx = row * (params.segments[0] + 1) + col] = Vec3f(col * segSz.x, y, row * segSz.y) + params.offset;

			if (row < params.segments[1] && col < params.segments[0])
			{
				pmesh->indices[idx = (row * params.segments[0] + col) * 6] = vtx;
				pmesh->indices[idx + 1] = vtx + (params.segments[0] + 1);
				pmesh->indices[idx + 2] = vtx + (params.segments[0] + 1) + 1;
				pmesh->indices[idx + 3] = vtx;
				pmesh->indices[idx + 4] = vtx + (params.segments[0] + 1) + 1;
				pmesh->indices[idx + 5] = vtx + 1;
			}
		}

	pmesh->CreateTree(false, 5);
	UpdatePhysTerrainProxyNodeYBounds(&pmesh->root, miny, maxy, TERRAIN_PROXY_Y_BOUNDS_BIAS);
}

///////////////////////////////////////////////////////////////////////////////////////////////////////////////////////

S_API void PhysTerrain::UpdateHeightmap(const float* heightmap, unsigned int heightmapSz[2], const AABB& bounds)
{
	if (!m_pShape)
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
	segMin[0] = (unsigned int)max(floorf((bounds.vMin.x - params.offset.x) / segSz.x), 0.0f) * params.segments[0];
	segMin[1] = (unsigned int)max(floorf((bounds.vMin.y - params.offset.y) / segSz.y), 0.0f) * params.segments[1];
	
	segMax[0] = ((unsigned int)ceilf(min(bounds.vMax.x - params.offset.x, params.size[0] + segSz.x) / segSz.x) * params.segments[0]);
	segMax[1] = ((unsigned int)ceilf(min(bounds.vMax.y - params.offset.y, params.size[1] + segSz.y) / segSz.y) * params.segments[1]);

	segMin[0] = segMin[1] = 0;
	segMax[0] = params.segments[0];
	segMax[1] = params.segments[1];

	geo::mesh* pmesh = (geo::mesh*)m_pShape;
	float miny = FLT_MAX, maxy = -FLT_MAX, y;
	for (unsigned int row = 0; row < (params.segments[1] + 1); ++row)
		for (unsigned int col = 0; col < (params.segments[0] + 1); ++col)
		{
			if (row >= segMin[1] && row <= segMax[1] && col >= segMin[0] && col <= segMax[0])
			{
				tc = Vec2f((float)col / params.segments[0], (float)row / params.segments[1]);
				tc -= (remainder = tc % pixelSzTC);

				samples[0] = SampleHeightmap(heightmap, tc + Vec2f(-0.5f, -0.5f) * pixelSzTC, heightmapSz);
				samples[1] = SampleHeightmap(heightmap, tc + Vec2f(0.5f, -0.5f) * pixelSzTC, heightmapSz);
				samples[2] = SampleHeightmap(heightmap, tc + Vec2f(-0.5f, 0.5f) * pixelSzTC, heightmapSz);
				samples[3] = SampleHeightmap(heightmap, tc + Vec2f(0.5f, 0.5f) * pixelSzTC, heightmapSz);
				h = lerp(lerp(samples[0], samples[1], remainder.x), lerp(samples[2], samples[3], remainder.x), remainder.y);

				pmesh->points[row * (params.segments[0] + 1) + col].y = h * params.heightScale;
			}

			y = pmesh->points[row * (params.segments[0] + 1) + col].y;
			miny = min(miny, y);
			maxy = max(maxy, y);
		}

	// Update proxy bound boxes
	UpdatePhysTerrainProxyNodeYBounds(&pmesh->root, miny, maxy, TERRAIN_PROXY_Y_BOUNDS_BIAS);
}

///////////////////////////////////////////////////////////////////////////////////////////////////////////////////////

S_API void PhysTerrain::Clear()
{
	PhysObject::Clear();
}

SP_NMSPACE_END
 