// ********************************************************************************************

//	SpeedPoint Animation Sequence

//	This is a virtual class. You cannot instantiate it.
//	Please use Specific Implementation to instantiate.

// ********************************************************************************************

#pragma once

#include <SPrerequisites.h>

namespace SpeedPoint
{
	// SpeedPoint Animation Sequence of Animation Keys (abstract)
	class S_API IAnimationSequence
	{
	public:	
		// Initialize by its holding animation bundle
		virtual SResult Initialize( SAnimationBundle* pBundle ) = 0;

		// Get the holding animation bundke
		virtual SAnimationBundle* GetBundle( void ) = 0;

		// Set the name of this sequence
		virtual void SetName( const SString& name ) = 0;

		// Get the name of this sequence
		virtual SString* GetName( void ) = 0;

		// Add a key to this animation sequence
		virtual SP_ID AddKey( float fTimecode, SVector3 vPosition ) = 0;

		// Get a key by its id
		virtual SAnimationKey* GetKey( SP_ID id ) = 0;

		// Clear all keys + their data as they are stored here
		virtual void Clear( void ) = 0;
		
		//// TODO : Are there still some functions we forgot?
	};
}