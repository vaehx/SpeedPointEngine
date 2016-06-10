////////////////////////////////////////////////////////////////////////////////////////////////////////
//	SpeedPoint Engine Source File
//
//	(c) 2011-2015 Pascal Rosenkranz aka iSmokiieZz
//	All rights reserved.
////////////////////////////////////////////////////////////////////////////////////////////////////////

#pragma once
#include <SPrerequisites.h>
#include "BoundBox.h"
#include "IEntity.h"

SP_NMSPACE_BEG

struct S_API ITerrain;
struct S_API IGameEngine;
struct S_API ITexture;
struct S_API IStaticObject;
struct IRenderableObject;
struct S_API ISkyBox;
struct S_API SInitialGeometryDesc;

enum ESceneNodeType
{
	eSCENENODE_UNKNOWN = 0,
	eSCENENODE_STATIC,
	eSCENENODE_ENTITY,
	eSCENENODE_LIGHT
};

// Scene Graph Node
struct SSceneNode
{
	AABB aabb;
	ESceneNodeType type;

	IStaticObject* pStatic;	// only set if type == eSCENENODE_STATIC
	IObject* pObject;			// only set if type == eSCENENODE_ENTITY
	ILight* pLight;	// only set if type == eSCENENODE_LIGHT

	SSceneNode()
		: type(eSCENENODE_UNKNOWN),
		pStatic(0),
		pObject(0),
		pLight(0)
	{
	}

	SSceneNode(const SSceneNode& rhs)
		: aabb(rhs.aabb),
		type(rhs.type),
		pStatic(rhs.pStatic),
		pObject(rhs.pObject),
		pLight(rhs.pLight)
	{
	}

	~SSceneNode()
	{
		type = eSCENENODE_UNKNOWN;
		pStatic = 0;
		pObject = 0;
		pLight = 0;
	}
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
	//
	// center - Set to true to make (0,0) the center of the terrain
	virtual ITerrain* CreateTerrain(unsigned int nSegs, unsigned int nChunkSegs, float fSideSz, float baseHeight, float fChunkStepDist, unsigned int nLodLevels, bool center = true) = 0;

	// Loads SPM Model from file
	virtual IStaticObject* LoadStaticObjectFromFile(const char* file, const char* objName) = 0;

	//virtual void RegisterObject(IObject* pObject) = 0;
	//virtual IObject* CreateObject(EObjectType type) = 0;

	virtual SResult CreateNormalsGeometry(IRenderableComponent* renderable, SInitialGeometryDesc* pNormalsGeometry) const = 0;

	virtual std::vector<SSceneNode>* GetSceneNodes() = 0;
	
	virtual SResult AddSceneNode(const SSceneNode& node) = 0;
	virtual SResult AddStaticObject(IStaticObject* pStatic) = 0;	
	
	// Add dynamic object / entity
	virtual SResult AddObject(IObject* pObject) = 0;

	virtual void Clear() = 0;
};


SP_NMSPACE_END