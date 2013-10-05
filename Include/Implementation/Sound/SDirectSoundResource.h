// ******************************************************************************************

//	SpeedPoint DirectSound Sound Resource

// ******************************************************************************************

#pragma once
#include <SPrerequisites.h>
#include <Abstract\SSound.h>
#include "SDirectSoundSystem.h"

namespace SpeedPoint
{
	// SpeedPoint DirectSound Sound resource
	class S_API SDirectSoundResource : public SSound
	{
	public:
		SString			cSpecification;
		SDirectSoundSystem*	pDirectSoundSystem;
		// LPDIRECTSOUND9 pDirectSound???

		SResult Initialize( SSoundSystem* pSystem );
		SResult LoadFromFile( char* cFileName, char* specification );
		SString GetSpecification( void );
		SResult Clear( void );
	};
}