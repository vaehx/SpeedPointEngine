// ********************************************************************************************

//	SpeedPoint Basic Octree Node

// ********************************************************************************************

#pragma once
#include <SPrerequisites.h>
#include <Abstract\SOctreeNode.h>
#include <SBoundBox.h>

namespace SpeedPoint
{
	// SpeedPoint Octree Node
	class S_API SBasicOctreeNode : public SOctreeNode
	{
	public:
		SBasicOctreeNode*	pChilds[8];	// preserve 8 OctreeNode childs
		DWORD*			pTriangles;	// the polygons (start indices)
		SBasicOctree*		pOctree;	// pointer to the parent to get the solid
		SBoundBox		bbBoundBox;
		bool			bIsLeaf;
		int			nPolys;		

		// Default constructor
		SBasicOctreeNode() : bIsLeaf(false), nPolys(0), pOctree(NULL), pTriangles(NULL) {};

		bool IsLeaf( void );
		SOctreeNode* GetChild( SP_UNIQUE id );
		int GetPolygonCount( void );
		SBoundBox GetBoundBox( void );
		DWORD* GetTriangles( void );
		void Clear( void );
	};
}