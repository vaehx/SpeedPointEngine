// ******************************************************************************************

//	SpeedPoint Octree

// ******************************************************************************************

#pragma once
#include <SPrerequisites.h>
#include <Abstract\SOctree.h>
#include "SBasicOctreeNode.h"

namespace SpeedPoint
{
	// SpeedPoint Octree
	class S_API SBasicOctree : public SOctree
	{
	public:
		SBasicOctreeNode	ndRootNode;
		SSolid*			pSolid;

		SResult Initialize( SSolid* pSolid );
		SSolid* GetSolid( void );
		SResult Compile( int nMaxDepth, int nMinPolys );
		SOctreeNode* GetRootNode( void );
		int GetCollidingNodes( const SBoundBox& bb, SOctreeNode** nodes );
		void Clear( void );
	};
}