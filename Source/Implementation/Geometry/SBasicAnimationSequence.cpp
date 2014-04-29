// SpeedPoint Basic Animation Bundle

#include <Implementation\Geometry\SBasicAnimationSequence.h>

namespace SpeedPoint
{

	// **********************************************************************************

	S_API SResult SBasicAnimationSequence::Initialize( IAnimationBundle* bundle )
	{
		return S_ERROR;
	}

	// **********************************************************************************

	S_API IAnimationBundle* SBasicAnimationSequence::GetBundle( void )
	{
		return NULL;
	}

	// **********************************************************************************

	S_API void SBasicAnimationSequence::SetName( const SString& name )
	{		
	}
	
	// **********************************************************************************

	S_API SString* SBasicAnimationSequence::GetName( void )
	{
		return NULL;
	}

	// **********************************************************************************

	S_API SP_ID SBasicAnimationSequence::AddKey( float fTimecode, SVector3 vPosition )	
	{
		return SP_ID();
	}

	// **********************************************************************************

	S_API IAnimationKey* SBasicAnimationSequence::GetKey( SP_ID id )
	{
		return NULL;
	}

	// **********************************************************************************

	S_API void SBasicAnimationSequence::Clear( void )
	{
	}
}