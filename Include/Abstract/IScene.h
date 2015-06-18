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
struct IRenderableObject;
struct S_API ISkyBox;

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

	virtual void SetSkyBox(ISkyBox* pSkyBox) = 0;
	virtual ISkyBox* GetSkyBox() const = 0;

	virtual ITerrain* GetTerrain() const = 0;
	
	// Notice: Make sure to call ITerrain->GenLodLevelChunks() with your camera afterwards.	
	virtual ITerrain* CreateTerrain(unsigned int nSegs, unsigned int nChunkSegs, float fSideSz, float baseHeight,
		float fChunkStepDist, unsigned int nLodLevels, ITexture* pColorMap, ITexture* pDetailMap) = 0;

	virtual IStaticObject* LoadStaticObjectFromFile(const char* file) = 0;

	//virtual void RegisterObject(IObject* pObject) = 0;
	//virtual IObject* CreateObject(EObjectType type) = 0;

	virtual SResult CreateNormalsGeometry(IRenderableObject* object, IRenderableObject** pNormalGeometryObject) const = 0;

	virtual std::vector<SSceneNode>* GetSceneNodes() = 0;

	virtual void Clear() = 0;	
};


SP_NMSPACE_END