// ********************************************************************************************

//	SpeedPoint Sound System

//	This is a virtual class. You cannot instantiate it.
//	Please use Specific Implementation to instantiate.

// ********************************************************************************************

#pragma once

#include <SPrerequisites.h>

namespace SpeedPoint
{
	// SpeedPoint Sound System (abstract)
	struct S_API ISoundSystem
	{
	public:
		// Initialize this sound system with the engine
		virtual SResult Initialize( SpeedPointEngine* pEngine ) = 0;

		// Load a sound resource from file freely positionable in the world
		virtual SResult AddSound( char* cFileName, char* cSpecification, bool bLooped, SVector3 vPosition, ISound* pSound, SP_ID* pUID ) = 0;

		// Get a sound by its id
		virtual ISound* GetSound( SP_ID id ) = 0;

		// Get a sound by its specification
		virtual ISound* GetSound( SString cSpec ) = 0;

		// Play a sound by specification
		virtual SResult PlaySound( SString cSpec ) = 0;

		// Play a sound as a music by specification
		virtual SResult PlayMusic( SString cSpec ) = 0;

		// Clearout all sounds
		virtual SResult Clear( void ) = 0;
	};
}