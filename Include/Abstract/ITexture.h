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

// Type of the texture
enum S_API ETextureType
{
	eTEXTURE_R8G8B8A8_UNORM,	
	//eTEXTURE_D16_FLOAT,
	eTEXTURE_D32_FLOAT
};

#define COLOR_CHANNEL_FLOAT2UCHAR(f) (char)((f) * 255.0f)


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

	// Initialize an empty texture with specified size and type and fill it with clearcolor.
	// If type is a depth map, then clearcolor.r is used to fill.
	virtual SResult CreateEmpty( int w, int h, int mipLevels, ETextureType type, SColor clearcolor ) = 0;

	// Arguments:
	//	pPixels:	eTEXTURE_R8G8B8A8_UNORM -> char r, g, b, a;
	//			eTEXTURE_D32_FLOAT -> float d;
	virtual SResult Lock(void **pPixels, unsigned int* pnPixels) = 0;

	virtual SResult Unlock() = 0;

	// Get the specification
	virtual SString GetSpecification( void ) = 0;

	// ----

	// Get the type of this texture
	virtual ETextureType GetType( void ) = 0;

	// Get the size of this texture
	virtual SResult GetSize( unsigned int* pW, unsigned int* pH ) = 0;

	// Clear texture buffer
	virtual SResult Clear( void ) = 0;
};

SP_NMSPACE_END
