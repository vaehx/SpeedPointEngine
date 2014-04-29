// ******************************************************************************************

//	SpeedPoint DirectSound-SoundSystem

// ******************************************************************************************

#pragma once
#include <SPrerequisites.h>
#include <Abstract\ISoundSystem.h>

namespace SpeedPoint
{
	// SpeedPoint DirectSound-SoundSystem
	class S_API SDirectSoundSystem : public ISoundSystem
	{
	public:
		SpeedPointEngine*	pEngine;
		SP_ID			iCurrentMusicPlaying;

		SResult Initialize( SpeedPointEngine* pEng );
		SResult AddSound( char* cFileName, char* cSpecification, bool bLooped, SVector3 vPosition, ISound* pSound, SP_ID* pID );
		ISound* GetSound( SP_ID id );
		ISound* GetSound( SString cSpec );
		SResult PlaySound( SString cSpec );
		SResult PlayMusic( SString cSpec );
		SResult Clear( void );
	};
}