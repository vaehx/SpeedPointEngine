// -------------------------------------------------------------------------------------------------
//	SpeedPoint Game Engine
//	Copyright (c) 2011-2017 Pascal Rosenkranz, All rights reserved.
// -------------------------------------------------------------------------------------------------

#pragma once

#include "..\IScene.h"
#include "Entity.h"
#include <Common\SPrerequisites.h>
#include <Common\ChunkedObjectPool.h>
#include <vector>

using std::vector;

SP_NMSPACE_BEG

class S_API Scene : public IScene
{
private:
	string m_Name;
	ChunkedObjectPool<CEntity> m_Entities;
	vector<IEntity*> m_ExternalEntities;

public:
	Scene();

	virtual ~Scene()
	{
		Clear();
	}

	virtual SResult Initialize();
	virtual void Clear();

	virtual void SetName(const string& name) { m_Name = name; };
	virtual const string& GetName() const { return m_Name; };

	virtual IEntity* SpawnEntity(const string& name, IEntityClass* recipe = 0);
	virtual void AddEntity(IEntity* pEntity);

	virtual vector<IEntity*> GetEntitiesByName(const string& name);
	virtual IEntity* GetFirstEntityByName(const string& name);
	virtual vector<IEntity*> GetEntities(const AABB& aabb = AABB(Vec3f(-FLT_MAX), Vec3f(FLT_MAX))) const;
};

SP_NMSPACE_END
