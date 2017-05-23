#pragma once

#include "..\PhysObject.h"
#include "..\IPhysics.h"
#include <Common\SPrerequisites.h>

SP_NMSPACE_BEG

class S_API PhysTerrain : public PhysObject
{
private:
	SPhysTerrainParams m_Params;
public:
	void Clear();
	void Create(const float* heightmap, unsigned int heightmapSz[2], const SPhysTerrainParams& params);
	void Update(const float* heightmap, unsigned int heightmapSz[2], const AABB& bounds = AABB());
};

SP_NMSPACE_END
