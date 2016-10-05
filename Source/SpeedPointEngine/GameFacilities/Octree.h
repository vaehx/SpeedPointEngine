// ******************************************************************************************

//	SpeedPoint Octree

// ******************************************************************************************

#pragma once
#include "OctreeNode.h"
#include <Abstract\IOctree.h>
#include <Abstract\SPrerequisites.h>


SP_NMSPACE_BEG


// SpeedPoint Octree
class S_API Octree : public IOctree
{
public:
	OctreeNode	ndRootNode;
	ISolid*			pSolid;

	SResult Initialize( ISolid* pSolid );
	ISolid* GetSolid( void );
	SResult Compile( int nMaxDepth, int nMinPolys );
	IOctreeNode* GetRootNode( void );
	int GetCollidingNodes( const AABB& bb, IOctreeNode** nodes );
	void Clear( void );
};


SP_NMSPACE_END