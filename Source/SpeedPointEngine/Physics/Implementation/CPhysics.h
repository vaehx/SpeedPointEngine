#pragma once

#include "PhysTerrain.h"
#include "..\IPhysics.h"
#include <Common\SPrerequisites.h>

SP_NMSPACE_BEG

class S_API CPhysics : public IPhysics
{
private:
	IComponentPool<PhysObject>* m_pObjects;
	vector<std::pair<PhysObject*, PhysObject*>> m_Colliding;
	PhysTerrain m_Terrain;
	bool m_bPaused;
	bool m_bHelpersShown;

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

	ILINE virtual void Pause(bool pause = true) { m_bPaused = pause; };
	ILINE virtual bool IsPaused() const { return m_bPaused; };
	ILINE virtual void ShowHelpers(bool show = true);
	ILINE virtual bool HelpersShown() const { return m_bHelpersShown; };
};

SP_NMSPACE_END
