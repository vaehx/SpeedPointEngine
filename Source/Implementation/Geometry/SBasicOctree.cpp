// SpeedPoint Octree

#include <Implementation\Geometry\Octree.h>

namespace SpeedPoint
{
	// **********************************************************************************

	S_API SResult Octree::Initialize( ISolid* pSolid )
	{
		return S_ERROR;
	}

	// **********************************************************************************

	S_API ISolid* Octree::GetSolid( void )
	{
		return NULL;
	}

	// **********************************************************************************

	S_API SResult Octree::Compile( int nMaxDepth, int nMinPolys )
	{
		return S_ERROR;
	}

	// **********************************************************************************

	S_API IOctreeNode* Octree::GetRootNode( void )
	{
		return NULL;
	}

	// **********************************************************************************

	S_API int Octree::GetCollidingNodes( const AABB& bb, IOctreeNode** nodes )
	{
		return -1;
	}

	// **********************************************************************************

	S_API void Octree::Clear( void )
	{
	}
}