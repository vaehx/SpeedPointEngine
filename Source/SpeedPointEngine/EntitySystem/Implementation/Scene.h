
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

	virtual void SetName(const string& name);
	virtual const string& GetName() const;

	virtual IEntity* SpawnEntity(const string& name, IEntityReceipt* receipt = 0);
	virtual void AddEntity(IEntity* pEntity);

	virtual vector<IEntity*> GetEntities(const AABB& aabb = AABB(Vec3f(-FLT_MAX), Vec3f(FLT_MAX))) const;
};

SP_NMSPACE_END
