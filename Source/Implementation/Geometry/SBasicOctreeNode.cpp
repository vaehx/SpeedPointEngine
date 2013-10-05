// SpeedPoint Basic Octree Node

#include <Implementation\Geometry\SBasicOctreeNode.h>

namespace SpeedPoint
{
	// **********************************************************************************
	
	S_API bool SBasicOctreeNode::IsLeaf( void )
	{
		return false;
	}

	// **********************************************************************************
	
	S_API SOctreeNode* SBasicOctreeNode::GetChild( SP_UNIQUE id )
	{
		return NULL;
	}

	// **********************************************************************************

	S_API int SBasicOctreeNode::GetPolygonCount( void )
	{
		return -1;
	}

	// **********************************************************************************

	S_API SBoundBox SBasicOctreeNode::GetBoundBox( void )
	{
		return SBoundBox();
	}

	// **********************************************************************************

	S_API DWORD* SBasicOctreeNode::GetTriangles( void )
	{
		return NULL;
	}

	// **********************************************************************************

	S_API void SBasicOctreeNode::Clear( void )
	{
	}

}