#pragma once

#include "..\PhysObject.h"
#include "..\IPhysics.h"
#include <Common\SPrerequisites.h>

SP_NMSPACE_BEG

class S_API PhysTerrain : public PhysObject
{
private:
	SPhysTerrainParams m_Params;

protected:
	virtual void UpdateHelper();

public:
	PhysTerrain();
	virtual EPhysObjectType GetType() const { return ePHYSOBJ_TYPE_TERRAIN; }
	void Clear();
	void Create(const float* heightmap, unsigned int heightmapSz[2], const SPhysTerrainParams& params);
	void UpdateHeightmap(const float* heightmap, unsigned int heightmapSz[2], const AABB& bounds = AABB());
};

SP_NMSPACE_END
