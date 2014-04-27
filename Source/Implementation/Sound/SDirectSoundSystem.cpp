// SpeedPoint DirectSound-SoundSystem

#include <Implementation\Sound\SDirectSoundSystem.h>
#include <Util\SVector3.h>

namespace SpeedPoint
{
	// **********************************************************************************

	S_API SResult SDirectSoundSystem::Initialize( SpeedPointEngine* pEng )
	{
		return S_ERROR;
	}

	// **********************************************************************************

	S_API SResult SDirectSoundSystem::AddSound( char* cFileName, char* cSpecification, bool bLooped, SVector3 vPosition, SSound* pSound, SP_ID* pID )
	{
		return S_ERROR;
	}

	// **********************************************************************************

	S_API SSound* SDirectSoundSystem::GetSound( SP_ID id )
	{
		return NULL;
	}

	// **********************************************************************************

	S_API SSound* SDirectSoundSystem::GetSound( SString cSpec )
	{
		return NULL;
	}

	// **********************************************************************************

	S_API SResult SDirectSoundSystem::PlaySound( SString cSpec )
	{
		return S_ERROR;
	}

	// **********************************************************************************

	S_API SResult SDirectSoundSystem::PlayMusic( SString cSpec )
	{
		return S_ERROR;
	}

	// **********************************************************************************

	S_API SResult SDirectSoundSystem::Clear( void )
	{
		return S_ERROR;
	}
}