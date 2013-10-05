// ******************************************************************************************

//	SpeedPoint Octree

//	This is a virtual class. You cannot instantiate it.
//	Please use Specific Implementation to instantiate.

// ******************************************************************************************

#pragma once
#include <SPrerequisites.h>

namespace SpeedPoint
{
	// SpeedPoint Octree (abstract)
	class S_API SOctree
	{
	public:
		// Initialize this octree with its holding solid
		virtual SResult Initialize( SSolid* pSolid ) = 0;

		// Get the solid that is holding the octree
		virtual SSolid* GetSolid( void ) = 0;

		// Get the root Octree node
		virtual SOctreeNode* GetRootNode( void ) = 0;

		// Compile octree. Will fail if pointer to solid not given
		virtual SResult Compile( int nMaxDepth, int nMinPolys ) = 0;

		// Get all octree nodes that collide with bound box
		virtual int GetCollidingNodes( const SBoundBox& bb, SOctreeNode** nodes ) = 0;

		// Clear the octree and its childs recursively
		virtual void Clear( void ) = 0;
	};
}