// ********************************************************************************************

//	SpeedPoint Texture Resource

//	This is a virtual class. You cannot instantiate it.
//	Please use Specific Implementation to instantiate.

// ********************************************************************************************

#pragma once

#include <SPrerequisites.h>
#include <Util\SColor.h>

SP_NMSPACE_BEG

struct S_API IGameEngine;


// SpeedPoint Texture Resource (abstract)
struct S_API ITexture
{
public:
	// -- Initialize --

	// Initialize the texture
	virtual SResult Initialize( IGameEngine* pEngine, const SString& spec ) = 0;
	
	// Initialize the texture with extended params
	virtual SResult Initialize( IGameEngine* pEngine, const SString& spec, bool bDynamic ) = 0;

	// Load a texture from file
	virtual SResult LoadFromFile( int w, int h, int mipLevels, char* cFileName ) = 0;

	// Initialize an empty texture with specified size and type
	virtual SResult CreateEmpty( int w, int h, int mipLevels, S_TEXTURE_TYPE type, SColor clearcolor ) = 0;

	// Get the specification
	virtual SString GetSpecification( void ) = 0;

	// ----

	// Get the type of this texture
	virtual S_TEXTURE_TYPE GetType( void ) = 0;

	// Get the size of this texture
	virtual SResult GetSize( int* pW, int* pH ) = 0;

	// Clear texture buffer
	virtual SResult Clear( void ) = 0;
};

SP_NMSPACE_END
