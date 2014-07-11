// SpeedPoint Basic Animation Bundle

#include <Implementation\Geometry\AnimationBundle.h>

namespace SpeedPoint
{
	// **********************************************************************************

	S_API SResult AnimationBundle::Initialize( ISolid* pSolid )
	{
		if( pSolid == NULL ) return S_ABORTED;

		return S_ERROR;
	}

	// **********************************************************************************

	S_API SResult AnimationBundle::Tick( float fTimeFactor )
	{
		return S_ERROR;
	}

	// **********************************************************************************

	S_API void AnimationBundle::SetName( const SString& name )
	{
	}

	// **********************************************************************************

	S_API SString* AnimationBundle::GetName( void )
	{
		return NULL;
	}

	// **********************************************************************************

	S_API IAnimationJoint* AnimationBundle::AddJoint( SP_ID* pID )
	{
		return NULL;
	}

	// **********************************************************************************

	S_API IAnimationJoint* AnimationBundle::GetJoint( SP_ID id )
	{
		return NULL;
	}

	// **********************************************************************************

	S_API IAnimationSequence* AnimationBundle::AddAnimationSequence( SP_ID* pID )
	{
		return NULL;
	}

	// **********************************************************************************

	S_API IAnimationSequence* AnimationBundle::GetAnimationSequence( SP_ID id )
	{
		return NULL;
	}

	// **********************************************************************************

	S_API SResult AnimationBundle::RunAnimationSequence( SP_ID id )
	{
		return S_ERROR;
	}

	// **********************************************************************************

	S_API SP_ID AnimationBundle::GetRunningAnimationSequence( void )
	{
		return SP_ID();
	}

	// **********************************************************************************
	
	S_API void AnimationBundle::Clear( void )
	{
	}
}