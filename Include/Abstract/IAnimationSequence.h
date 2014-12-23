// ********************************************************************************************

//	SpeedPoint Animation Sequence

//	This is a virtual class. You cannot instantiate it.
//	Please use Specific Implementation to instantiate.

// ********************************************************************************************

#pragma once

#include <SPrerequisites.h>
#include <Abstract\Vector3.h>


SP_NMSPACE_BEG

struct S_API IAnimationBundle;
struct S_API IAnimationKey;

// SpeedPoint Animation Sequence of Animation Keys (abstract)
struct S_API IAnimationSequence
{
public:	
	// Initialize by its holding animation bundle
	virtual SResult Initialize( IAnimationBundle* pBundle ) = 0;

	// Get the holding animation bundke
	virtual IAnimationBundle* GetBundle( void ) = 0;

	// Set the name of this sequence
	virtual void SetName( const SString& name ) = 0;

	// Get the name of this sequence
	virtual SString* GetName( void ) = 0;

	// Add a key to this animation sequence
	virtual SP_ID AddKey( float fTimecode, SVector3 vPosition ) = 0;

	// Get a key by its id
	virtual IAnimationKey* GetKey( SP_ID id ) = 0;

	// Clear all keys + their data as they are stored here
	virtual void Clear( void ) = 0;
	
	//// TODO : Are there still some functions we forgot?
};


SP_NMSPACE_END