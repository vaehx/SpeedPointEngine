// SpeedPoint Octree

#include <Implementation\Geometry\SBasicOctree.h>

namespace SpeedPoint
{
	// **********************************************************************************

	S_API SResult SBasicOctree::Initialize( ISolid* pSolid )
	{
		return S_ERROR;
	}

	// **********************************************************************************

	S_API ISolid* SBasicOctree::GetSolid( void )
	{
		return NULL;
	}

	// **********************************************************************************

	S_API SResult SBasicOctree::Compile( int nMaxDepth, int nMinPolys )
	{
		return S_ERROR;
	}

	// **********************************************************************************

	S_API IOctreeNode* SBasicOctree::GetRootNode( void )
	{
		return NULL;
	}

	// **********************************************************************************

	S_API int SBasicOctree::GetCollidingNodes( const SBoundBox& bb, IOctreeNode** nodes )
	{
		return -1;
	}

	// **********************************************************************************

	S_API void SBasicOctree::Clear( void )
	{
	}
}