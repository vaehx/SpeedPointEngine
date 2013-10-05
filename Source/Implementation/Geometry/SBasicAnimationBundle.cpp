// SpeedPoint Basic Animation Bundle

#include <Implementation\Geometry\SBasicAnimationBundle.h>

namespace SpeedPoint
{
	// **********************************************************************************

	S_API SResult SBasicAnimationBundle::Initialize( SSolid* pSolid )
	{
		if( pSolid == NULL ) return S_ABORTED;

		return S_ERROR;
	}

	// **********************************************************************************

	S_API SResult SBasicAnimationBundle::Tick( float fTimeFactor )
	{
		return S_ERROR;
	}

	// **********************************************************************************

	S_API void SBasicAnimationBundle::SetName( const SString& name )
	{
	}

	// **********************************************************************************

	S_API SString* SBasicAnimationBundle::GetName( void )
	{
		return NULL;
	}

	// **********************************************************************************

	S_API SAnimationJoint* SBasicAnimationBundle::AddJoint( SP_ID* pID )
	{
		return NULL;
	}

	// **********************************************************************************

	S_API SAnimationJoint* SBasicAnimationBundle::GetJoint( SP_ID id )
	{
		return NULL;
	}

	// **********************************************************************************

	S_API SAnimationSequence* SBasicAnimationBundle::AddAnimationSequence( SP_ID* pID )
	{
		return NULL;
	}

	// **********************************************************************************

	S_API SAnimationSequence* SBasicAnimationBundle::GetAnimationSequence( SP_ID id )
	{
		return NULL;
	}

	// **********************************************************************************

	S_API SResult SBasicAnimationBundle::RunAnimationSequence( SP_ID id )
	{
		return S_ERROR;
	}

	// **********************************************************************************

	S_API SP_ID SBasicAnimationBundle::GetRunningAnimationSequence( void )
	{
		return SP_ID();
	}

	// **********************************************************************************
	
	S_API void SBasicAnimationBundle::Clear( void )
	{
	}
}