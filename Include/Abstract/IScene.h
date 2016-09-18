////////////////////////////////////////////////////////////////////////////////////////////////////////
//	SpeedPoint Engine Source File
//
//	(c) 2011-2016 Pascal Rosenkranz
//	All rights reserved.
////////////////////////////////////////////////////////////////////////////////////////////////////////

#pragma once
#include <SPrerequisites.h>
#include "IEntity.h"

SP_NMSPACE_BEG

struct S_API IGameEngine;

////////////////////////////////////////////////////////////////////////////////////////////////////////

// Holding IDs to the objects of a scene (SceneGraph)
struct S_API IScene
{
	virtual ~IScene() {}

	virtual SResult Initialize(IGameEngine* pGameEngine) = 0;
	virtual void Clear() = 0;

	virtual void SetName(const string& name) = 0;
	virtual const string& GetName() const = 0;

	virtual const vector<IEntity*>& GetEntities() const = 0;

	// Loads SPM Model from file
	virtual IEntity* LoadObjectFromFile(const char* file, const char* objName) = 0;
	virtual IEntity* SpawnEntity() = 0;

	// Adds this object to the scene, which will take ownership of the entity !
	virtual void AddObject(IEntity* pEntity) = 0;
};

SP_NMSPACE_END
