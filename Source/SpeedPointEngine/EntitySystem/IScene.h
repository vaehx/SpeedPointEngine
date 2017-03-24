//////////////////////////////////////////////////////////////////////////////////////////////////
//
//	SpeedPoint Game Engine
//	Copyright (c) 2011-2017 Pascal Rosenkranz, All rights reserved.
//
//////////////////////////////////////////////////////////////////////////////////////////////////

#pragma once

#include "IEntity.h"
#include <Common\SPrerequisites.h>
#include <Common\BoundBox.h>
#include <vector>

using std::vector;

SP_NMSPACE_BEG

//////////////////////////////////////////////////////////////////////////////////////////////////

// Provides a scene graph holding entities
struct S_API IScene
{
	virtual ~IScene() {}

	virtual SResult Initialize() = 0;
	virtual void Clear() = 0;

	virtual void SetName(const string& name) = 0;
	virtual const string& GetName() const = 0;

	virtual IEntity* SpawnEntity(const string& name, IEntityReceipt* receipt = 0) = 0;

	// Adds the entity to the scene graph as an external entity. Ownership of the
	// entity will not be taken over.
	virtual void AddEntity(IEntity* pEntity) = 0;

	// Returns a list of entities where each entity position lies inside the given aabb.
	// This method does NOT take the extents of the entity into account.
	// aabb - If not given returns all entities in the scene
	virtual vector<IEntity*> GetEntities(const AABB& aabb = AABB(Vec3f(-FLT_MAX), Vec3f(FLT_MAX))) const = 0;
};

SP_NMSPACE_END
