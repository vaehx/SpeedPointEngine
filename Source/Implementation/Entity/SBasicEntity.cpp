// SpeedPoint Basic Entity

#include <Implementation\Entity\SBasicEntity.h>
#include <RunPowderScripting\RunPowderEnvironment.h>

namespace SpeedPoint
{
	// **********************************************************************************

	S_API void SBasicEntity::SetName( const SString& name )
	{
		cName = name;
	}

	// **********************************************************************************

	S_API SString* SBasicEntity::GetName( void )
	{
		return &cName;
	}

	// **********************************************************************************

	S_API float SBasicEntity::GetInteractionRadius( void )
	{
		return fInteractionRadius;
	}

	// **********************************************************************************
	
	S_API void SBasicEntity::SetInteractionRadius( float fRadius )
	{
		fInteractionRadius = fRadius;
	}

	// **********************************************************************************

	S_API void SBasicEntity::InitScriptEnvironment( RunPowderEnvironment* pEnv )
	{
		pScriptEnvironment = pEnv;
	}
	
	// **********************************************************************************

	S_API SResult SBasicEntity::InitScriptAttributes( void )
	{
		if( pScriptEnvironment == NULL ) return S_ABORTED;

///// TODO: Implement proper RunPowder environment function member call
		return S_ERROR;
	}
}