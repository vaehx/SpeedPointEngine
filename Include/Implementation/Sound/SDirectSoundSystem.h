// ******************************************************************************************

//	SpeedPoint DirectSound-SoundSystem

// ******************************************************************************************

#pragma once
#include <SPrerequisites.h>
#include <Abstract\SSoundSystem.h>

namespace SpeedPoint
{
	// SpeedPoint DirectSound-SoundSystem
	class S_API SDirectSoundSystem : public SSoundSystem
	{
	public:
		SpeedPointEngine*	pEngine;
		SP_ID			iCurrentMusicPlaying;

		SResult Initialize( SpeedPointEngine* pEng );
		SResult AddSound( char* cFileName, char* cSpecification, bool bLooped, SVector3 vPosition, SSound* pSound, SP_ID* pID );
		SSound* GetSound( SP_ID id );
		SSound* GetSound( SString cSpec );
		SResult PlaySound( SString cSpec );
		SResult PlayMusic( SString cSpec );
		SResult Clear( void );
	};
}