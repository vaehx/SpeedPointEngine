#pragma once

#include "PhysTerrain.h"
#include "..\IPhysics.h"
#include <Common\SPrerequisites.h>

SP_NMSPACE_BEG

#define RESTING_TOLERANCE 0.02f

class S_API CPhysics : public IPhysics
{
private:
	IComponentPool<PhysObject>* m_pObjects;
	PhysTerrain m_Terrain;

protected:
	virtual void SetPhysObjectPool(IComponentPool<PhysObject>* pPool);

public:
	CPhysics();
	~CPhysics();

	ILINE virtual PhysObject* CreatePhysObject();
	ILINE virtual void ClearPhysObjects();
	ILINE virtual void CreateTerrainProxy(const float* heightmap, unsigned int heightmapSz[2], const SPhysTerrainParams& params);
	ILINE virtual void UpdateTerrainProxy(const float* heightmap, unsigned int heightmapSz[2], const AABB& bounds = AABB());
	ILINE virtual void ClearTerrainProxy();
	ILINE virtual void Update(float fTime);
};

SP_NMSPACE_END