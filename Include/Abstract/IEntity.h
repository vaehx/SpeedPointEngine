// ********************************************************************************************

//	SpeedPoint Entity

//	This is a virtual class. You cannot instantiate it.
//	Please use Specific Implementation to instantiate.

// ********************************************************************************************

#pragma once

#include <SPrerequisites.h>


SP_NMSPACE_BEG

class S_API RunPowderEnvironment;


// SpeedPoint Entity (abstract)
struct S_API IEntity
{
public:
	// Set the name of this entity
	virtual void SetName( const SString& src ) = 0;

	// Get the name of this entity
	virtual SString* GetName( void ) = 0;

	// Get the interaction radius
	virtual float GetInteractionRadius( void ) = 0;

	// Set the interaction radius
	virtual void SetInteractionRadius( float fRadius ) = 0;

	// Init Script environment where this entity should find a script for itself
	virtual void InitScriptEnvironment( RunPowderEnvironment* pEnv ) = 0;
	
	// Init default Script Attributes		
	virtual SResult InitScriptAttributes( void ) = 0;		
};


SP_NMSPACE_END