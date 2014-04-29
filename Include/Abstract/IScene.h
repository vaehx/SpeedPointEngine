////////////////////////////////////////////////////////////////////////////////////////////////////////
//	SpeedPoint Engine Source File
//
//	(c) 2011-2014 Pascal Rosenkranz aka iSmokiieZz
//	All rights reserved.
////////////////////////////////////////////////////////////////////////////////////////////////////////

#pragma once
#include <SPrerequisites.h>

namespace SpeedPoint
{
	// Scene Graph Node
	struct S_API ISceneNode
	{
	public:		
		ISceneNode*	m_pParent;	// node will inherit position of this node
		ISceneNode**	m_pChilds;	// array of ptrs to child nodes. this array has fixed size (nMaxSceneNodeChilds in SSettings)
		SP_ID		m_ID;		// the id of the solid bound to this node
	};

	////////////////////////////////////////////////////////////////////////////////////////////////////////

	// Holding IDs to the objects of a scene (SceneGraph)
	class S_API IScene
	{
	public:
		// Summary:
		//	Allocate new node instance and (if set) gives ptr to instance
		virtual SResult AddNode(const SP_ID& id, ISceneNode** pOutputNode) = 0;
	};
}