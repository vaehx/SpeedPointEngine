////////////////////////////////////////////////////////////////////////////////////////////////////////
//	SpeedPoint Engine Source File
//
//	(c) 2011-2014 Pascal Rosenkranz aka iSmokiieZz
//	All rights reserved.
////////////////////////////////////////////////////////////////////////////////////////////////////////

#pragma once
#include <SPrerequisites.h>
#include "BoundBox.h"
#include "IObject.h"

SP_NMSPACE_BEG

struct S_API ITerrain;
struct S_API IGameEngine;
struct S_API ITexture;
struct S_API IStaticObject;

// Scene Graph Node
struct SSceneNode
{
	AABB aabb;
	IObject* pObject;	
};

////////////////////////////////////////////////////////////////////////////////////////////////////////

// Holding IDs to the objects of a scene (SceneGraph)
struct S_API IScene
{
	virtual ~IScene() {}

	virtual SResult Initialize(IGameEngine* pGameEngine) = 0;

	virtual ITerrain* GetTerrain() = 0;
	virtual ITerrain* CreateTerrain(float width, float depth, unsigned int nX, unsigned int nZ, float baseHeight, ITexture* pColorMap, ITexture* pDetailMap) = 0;

	virtual IStaticObject* LoadStaticObjectFromFile(const char* file) = 0;

	//virtual void RegisterObject(IObject* pObject) = 0;
	//virtual IObject* CreateObject(EObjectType type) = 0;

	virtual void Clear() = 0;	
};


SP_NMSPACE_END