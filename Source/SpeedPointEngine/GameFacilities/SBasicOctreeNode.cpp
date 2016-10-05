// SpeedPoint Basic Octree Node

#include "OctreeNode.h"

namespace SpeedPoint
{
	// **********************************************************************************
	
	S_API bool OctreeNode::IsLeaf( void )
	{
		return false;
	}

	// **********************************************************************************
	
	S_API IOctreeNode* OctreeNode::GetChild( SP_UNIQUE id )
	{
		return NULL;
	}

	// **********************************************************************************

	S_API int OctreeNode::GetPolygonCount( void )
	{
		return -1;
	}

	// **********************************************************************************

	S_API AABB OctreeNode::GetBoundBox( void )
	{
		return AABB();
	}

	// **********************************************************************************

	S_API DWORD* OctreeNode::GetTriangles( void )
	{
		return NULL;
	}

	// **********************************************************************************

	S_API void OctreeNode::Clear( void )
	{
	}

}