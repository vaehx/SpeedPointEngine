// ********************************************************************************************

//	SpeedPoint Sound Resource

//	This is a virtual class. You cannot instantiate it.
//	Please use Specific Implementation to instantiate.

// ********************************************************************************************

#pragma once

#include <SPrerequisites.h>


SP_NMSPACE_BEG

struct S_API ISoundSystem;

// SpeedPoint Sound Resource (abstract)
struct S_API ISound
{
public:	
	// Initialize with a soundsystem
	virtual SResult Initialize( ISoundSystem* pSystem ) = 0;		

	// Load the sound from a file and set specification
	virtual SResult LoadFromFile( char* cFilename, char* specification ) = 0;

	// Get the specification
	virtual SString GetSpecification( void ) = 0;

	// Clear the sound data
	virtual SResult Clear( void ) = 0;
};


SP_NMSPACE_END