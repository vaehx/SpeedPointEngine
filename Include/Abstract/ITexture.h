// ********************************************************************************************

//	SpeedPoint Texture Resource

//	This is a virtual class. You cannot instantiate it.
//	Please use Specific Implementation to instantiate.

// ********************************************************************************************

#pragma once

#include <SPrerequisites.h>
#include <Util\SColor.h>
#include "Vector2.h"

SP_NMSPACE_BEG

struct S_API IGameEngine;

// Type of the texture
enum S_API ETextureType
{
	eTEXTURE_R8G8B8A8_UNORM,	
	//eTEXTURE_D16_FLOAT,
	eTEXTURE_D32_FLOAT,
	eTEXTURE_R32_FLOAT
};

#define COLOR_CHANNEL_FLOAT2UCHAR(f) (char)((f) * 255.0f)

inline unsigned int GetTextureBPP(ETextureType type)
{
	switch (type)
	{
	case eTEXTURE_R8G8B8A8_UNORM: return 4;
	case eTEXTURE_R32_FLOAT:
	case eTEXTURE_D32_FLOAT:
		return sizeof(float);
	}

	return 0;
}


enum S_API ECubemapSide
{
	eCUBEMAP_SIDE_NEGX,
	eCUBEMAP_SIDE_NEGY,
	eCUBEMAP_SIDE_NEGZ,
	eCUBEMAP_SIDE_POSX,
	eCUBEMAP_SIDE_POSY,
	eCUBEMAP_SIDE_POSZ
};


// SpeedPoint Texture Resource (abstract)
struct S_API ITexture
{
public:
	// -- Initialize --	

	// Initialize the texture
	virtual SResult Initialize( IGameEngine* pEngine, const SString& spec ) = 0;
	
	// Initialize the texture with extended params
	// - Set bDynamic to true, if you want to WRITE new data into the texture after creation
	// - Set bStaged to true, to create a RAM copy of the data, which allows you to read back data from the texture
	virtual SResult Initialize( IGameEngine* pEngine, const SString& spec, bool bDynamic, bool bStaged ) = 0;

	virtual bool IsStaged() const = 0;

	// Load a texture from file
	virtual SResult LoadFromFile(int w, int h, int mipLevels, char* cFileName) = 0;

	// baseName: Path and base filename of the 6 cubemap images without File extension
	//		("assets\\sky" -> "assets\\sky_(pos|neg)(x|y|z).bmp");
	virtual SResult LoadCubemapFromFile(int singleW, int singleH, char* baseName) = 0;

	// Initialize an empty texture with specified size and type and fill it with clearcolor.
	// If type is a depth map, then clearcolor.r is used to fill.
	virtual SResult CreateEmpty( int w, int h, int mipLevels, ETextureType type, SColor clearcolor ) = 0;

	// This function fails if the texture is not dynamic.
	// If the texture is staged, this will return the pointer to the staged data. This data is
	// then uploaded to the GPU when calling Unlock().
	// Note: pnRowPitch is set to the number of BYTES per row in the texture.
	virtual SResult Lock(void **pPixels, unsigned int* pnPixels, unsigned int* pnRowPitch = 0) = 0;

	virtual SResult Unlock() = 0;

	// Summary:
	//	Does a simple nearest-neighbor filtering on the staged data with address wrapping.
	//	If the texture is not staged, this function will fail.
	// Arguments:
	//	pData is assumed to hold data for one pixel
	virtual SResult SampleStaged(const Vec2f& texcoords, void* pData) const = 0;

	// This function fails if the texture is not a floating point or unorm texture.
	virtual SResult SampleStagedBilinear(Vec2f texcoords, void* pData) const = 0;

	// Be careful with it!
	virtual void* GetStagedData() = 0;

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
