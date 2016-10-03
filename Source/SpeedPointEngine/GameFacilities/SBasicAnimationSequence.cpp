// SpeedPoint Basic Animation Bundle

#include <Implementation\GameFacilities\AnimationSequence.h>

namespace SpeedPoint
{

	// **********************************************************************************

	S_API SResult AnimationSequence::Initialize( IAnimationBundle* bundle )
	{
		return S_ERROR;
	}

	// **********************************************************************************

	S_API IAnimationBundle* AnimationSequence::GetBundle( void )
	{
		return NULL;
	}

	// **********************************************************************************

	S_API void AnimationSequence::SetName( const SString& name )
	{		
	}
	
	// **********************************************************************************

	S_API SString* AnimationSequence::GetName( void )
	{
		return NULL;
	}

	// **********************************************************************************

	S_API SP_ID AnimationSequence::AddKey( float fTimecode, SVector3 vPosition )	
	{
		return SP_ID();
	}

	// **********************************************************************************

	S_API IAnimationKey* AnimationSequence::GetKey( SP_ID id )
	{
		return NULL;
	}

	// **********************************************************************************

	S_API void AnimationSequence::Clear( void )
	{
	}
}