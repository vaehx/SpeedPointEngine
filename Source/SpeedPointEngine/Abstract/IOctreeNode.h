// ******************************************************************************************

//	SpeedPoint Octree Node

//	This is a virtual class. You cannot instantiate it.
//	Please use Specific Implementation to instantiate.

// ******************************************************************************************

#pragma once
#include <SPrerequisites.h>
#include <Abstract\BoundBox.h>


SP_NMSPACE_BEG

struct S_API IOctreeNode;



// SpeedPoint Octree Node (abstract)
struct S_API IOctreeNode
{
public:
	// Check if this node is a leaf
	virtual bool IsLeaf( void ) = 0;

	// Get a child by its id (0-7)
	virtual IOctreeNode* GetChild( SP_UNIQUE id ) = 0;

	// Get Pologyon count
	virtual int GetPolygonCount( void ) = 0;

	// Get the BoundBox of this node
	virtual AABB GetBoundBox( void ) = 0;

	// Get the start indices of the triangles belonging to this octree node
	// if it is not a leaf it will return NULL
	virtual DWORD* GetTriangles( void ) = 0;

	// Clear the octree node and its childs recursively
	virtual void Clear( void ) = 0;
};


SP_NMSPACE_END