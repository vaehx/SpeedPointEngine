// ********************************************************************************************

//	SpeedPoint Basic Octree Node

// ********************************************************************************************

#pragma once
#include <SPrerequisites.h>
#include <Abstract\IOctreeNode.h>
#include <Util\SBoundBox.h>


SP_NMSPACE_BEG

class S_API Octree;

// SpeedPoint Octree Node
class S_API OctreeNode : public IOctreeNode
{
public:
	OctreeNode*	pChilds[8];	// preserve 8 OctreeNode childs
	DWORD*			pTriangles;	// the polygons (start indices)
	Octree*		pOctree;	// pointer to the parent to get the solid
	SBoundBox		bbBoundBox;
	bool			bIsLeaf;
	int			nPolys;		

	// Default constructor
	OctreeNode() : bIsLeaf(false), nPolys(0), pOctree(NULL), pTriangles(NULL) {};

	bool IsLeaf( void );
	IOctreeNode* GetChild( SP_UNIQUE id );
	int GetPolygonCount( void );
	SBoundBox GetBoundBox( void );
	DWORD* GetTriangles( void );
	void Clear( void );
};


SP_NMSPACE_END