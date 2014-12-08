////////////////////////////////////////////////////////////////////////////////////////////////////////
//	SpeedPoint Engine Source File
//
//	(c) 2011-2014 Pascal Rosenkranz aka iSmokiieZz
//	All rights reserved.
////////////////////////////////////////////////////////////////////////////////////////////////////////

#pragma once
#include <SPrerequisites.h>

SP_NMSPACE_BEG

struct S_API ITerrain;
struct S_API IObject;
struct S_API IGameEngine;
struct S_API ITexture;

// Scene Graph Node
struct S_API ISceneNode
{
public:		
	ISceneNode*	m_pParent;	// node will inherit position of this node
	ISceneNode**	m_pChilds;	// array of ptrs to child nodes. this array has fixed size (nMaxSceneNodeChilds in SSettings)
	IObject**	m_pObjects;
};

////////////////////////////////////////////////////////////////////////////////////////////////////////

// Holding IDs to the objects of a scene (SceneGraph)
struct S_API IScene
{
	virtual ~IScene() {}

	virtual SResult Initialize(IGameEngine* pGameEngine) = 0;

	virtual ITerrain* GetTerrain() = 0;
	virtual ITerrain* CreateTerrain(float width, float depth, unsigned int nX, unsigned int nZ, float baseHeight, ITexture* pColorMap, ITexture* pDetailMap) = 0;

	virtual void Clear() = 0;	
};


SP_NMSPACE_END