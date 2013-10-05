// SpeedPoint Basic Solid

#include <Implementation\Geometry\SBasicSolid.h>

namespace SpeedPoint
{
	// **********************************************************************************
	//		GENERAL



	// **********************************************************************************

	S_API SResult SBasicSolid::Initialize( SpeedPointEngine* pEngine, bool bDynamic )
	{
		return S_ERROR;
	}

	// **********************************************************************************

	S_API SResult SBasicSolid::Clear( void )
	{
		return S_ERROR;
	}

	// **********************************************************************************

	S_API SP_ID SBasicSolid::GetVertexBuffer( void )
	{
		return SP_ID();
	}

	// **********************************************************************************

	S_API SP_ID SBasicSolid::GetIndexBuffer( void )
	{
		return SP_ID();
	}



	// **********************************************************************************
	//		PRIMITIVE



	// **********************************************************************************
	
	S_API SP_ID SBasicSolid::AddPrimitive( const SPrimitive& primitive )
	{
		return SP_ID();
	}

	// **********************************************************************************
	
	S_API SPrimitive* SBasicSolid::GetPrimitive( SP_ID id )
	{
		return NULL;
	}

	// **********************************************************************************

	S_API SPrimitive* SBasicSolid::GetPrimitive( SP_UNIQUE index )
	{
		return NULL;
	}

	// **********************************************************************************

	S_API int SBasicSolid::GetPrimitiveCount( void )
	{
		return -1;
	}




	// **********************************************************************************

	//		MATERIAL




	// **********************************************************************************
	
	S_API void SBasicSolid::SetMaterial( const SMaterial& mat )
	{
		return;
	}

	// **********************************************************************************

	S_API SMaterial* SBasicSolid::GetMaterial( void )
	{
		return NULL;
	}



	// **********************************************************************************
	//		BOUND BOX



	// **********************************************************************************

	S_API SBoundBox* SBasicSolid::RecalculateBoundBox( void )
	{
		return NULL;
	}

	// **********************************************************************************

	S_API SBoundBox* SBasicSolid::GetBoundBox( void )
	{
		return NULL;
	}

	// **********************************************************************************

	S_API SBoundBox* SBasicSolid::SetBoundBox( const SBoundBox& bb )
	{
		return NULL;
	}



	// **********************************************************************************
	//		OCTREE


	// **********************************************************************************

	S_API SOctree* SBasicSolid::GetOctree( void )
	{
		return NULL;
	}




	// **********************************************************************************
	//		ANIMATIONS


	// **********************************************************************************

	S_API void SBasicSolid::SetAnimationBundle( SP_ID iBundle )
	{
		return;
	}

	// **********************************************************************************

	S_API SP_ID SBasicSolid::GetAnimationBundle( void )
	{
		return SP_ID();
	}

	// **********************************************************************************

	S_API SResult SBasicSolid::RunAnimation( SString cName )
	{
		return S_ERROR;
	}

	// **********************************************************************************

	S_API SResult SBasicSolid::GetCurrentAnimation( SString* dest )
	{
		return S_ERROR;
	}

	// **********************************************************************************

	S_API SResult SBasicSolid::TickAnimation( float fFrameDelay )
	{
		return S_ERROR;
	}

}