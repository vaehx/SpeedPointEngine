// ******************************************************************************************

//	SpeedPoint Octree

// ******************************************************************************************

#pragma once
#include <SPrerequisites.h>
#include <Abstract\IOctree.h>
#include "SBasicOctreeNode.h"

namespace SpeedPoint
{
	// SpeedPoint Octree
	class S_API SBasicOctree : public IOctree
	{
	public:
		SBasicOctreeNode	ndRootNode;
		ISolid*			pSolid;

		SResult Initialize( ISolid* pSolid );
		ISolid* GetSolid( void );
		SResult Compile( int nMaxDepth, int nMinPolys );
		IOctreeNode* GetRootNode( void );
		int GetCollidingNodes( const SBoundBox& bb, IOctreeNode** nodes );
		void Clear( void );
	};
}