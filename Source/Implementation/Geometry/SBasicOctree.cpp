// SpeedPoint Octree

#include <Implementation\Geometry\SBasicOctree.h>

namespace SpeedPoint
{
	// **********************************************************************************

	S_API SResult SBasicOctree::Initialize( SSolid* pSolid )
	{
		return S_ERROR;
	}

	// **********************************************************************************

	S_API SSolid* SBasicOctree::GetSolid( void )
	{
		return NULL;
	}

	// **********************************************************************************

	S_API SResult SBasicOctree::Compile( int nMaxDepth, int nMinPolys )
	{
		return S_ERROR;
	}

	// **********************************************************************************

	S_API SOctreeNode* SBasicOctree::GetRootNode( void )
	{
		return NULL;
	}

	// **********************************************************************************

	S_API int SBasicOctree::GetCollidingNodes( const SBoundBox& bb, SOctreeNode** nodes )
	{
		return -1;
	}

	// **********************************************************************************

	S_API void SBasicOctree::Clear( void )
	{
	}
}