// ********************************************************************************************

//	SpeedPoint Texture Resource

//	This is a virtual class. You cannot instantiate it.
//	Please use Specific Implementation to instantiate.

// ********************************************************************************************

#pragma once

#include <Common\SPrerequisites.h>
#include <Common\SColor.h>
#include <Common\Vector2.h>

SP_NMSPACE_BEG

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


// SpeedPoint Texture Resource
struct S_API ITexture
{
public:
	virtual ~ITexture() {}

	virtual void AddRef() = 0;
	virtual void Release() = 0;

	// Summary:
	//		Load the texture contents from a file.
	//		If the texture is already initialized, it will clear existing texture contents!
	// Description:
	//		If w == 0 or h == 0, the actual file size is used as the size and no scaling will happen
	//		If mipLevels == 0, the full mip chain will be generated
	// Arguments:
	//		filePath - system path to the texture file
	virtual SResult LoadFromFile(const string& specification, const string& filePath, unsigned int w = 0, unsigned int h = 0, unsigned int mipLevels = 0) = 0;

	// Summary:
	//		Load the texture contents from a cubemap.
	//		If the texture is already initialized, it will clear existing texture contents!
	// Description:
	//		If w == 0 or h == 0, the actual file size is used as the size and no scaling will happen
	// Parameters:
	//		baseName - System path and base filename of the 6 cubemap images without File extension
	//				   ("assets\\sky" -> "assets\\sky_(pos|neg)(x|y|z).bmp");	
	virtual SResult LoadCubemapFromFile(const string& specification, const string& basePath, unsigned int singleW = 0, unsigned int singleH = 0) = 0;

	// Summary:
	//		Initialize an empty texture with specified size and type and fill it with clearcolor.
	//		If type is a depth map, then clearcolor.r is used to fill.
	virtual SResult CreateEmpty(const string& specification, unsigned int w, unsigned int h, unsigned int mipLevels, ETextureType type, SColor clearcolor) = 0;


	// Status queries
public:
	virtual const string& GetSpecification(void) const = 0;
	virtual ETextureType GetType(void) = 0;
	virtual SResult GetSize(unsigned int* pW, unsigned int* pH) = 0;

	virtual bool IsInitialized() const = 0;
	virtual bool IsDynamic() const = 0;
	virtual bool IsStaged() const = 0;
	virtual bool IsCubemap() const = 0;


	// Data access and manipulation
public:
	// Summary:
	//		Locks the dynamic texture so that the pixels can be modified
	// Description:
	//		This function fails if the texture is not dynamic.
	//		If the texture is staged, this will return the pointer to the staged data. This data is
	//		then uploaded to the GPU when calling Unlock().
	//		Note: pnRowPitch is set to the number of BYTES per row in the texture.
	virtual SResult Lock(void **pPixels, unsigned int* pnPixels, unsigned int* pnRowPitch = 0) = 0;

	virtual SResult Unlock() = 0;

	// Summary:
	//		Sets all pixels of this dynamic texture to the given color.
	//		Fails, if this texture is not dynamic
	virtual SResult Fill(SColor color) = 0;

	// Summary:
	//		Does a simple nearest-neighbor filtering on the staged data with address wrapping.
	//		If the texture is not staged, this function will fail.
	// Arguments:
	//		pData - is assumed to hold data for one pixel
	virtual SResult SampleStaged(const Vec2f& texcoords, void* pData) const = 0;

	// Summary:
	//		Samples the staged texture with a bilinear filter
	//		This function fails if the texture is not a floating point or unorm texture.
	virtual SResult SampleStagedBilinear(Vec2f texcoords, void* pData) const = 0;

	// Summary:
	//		Gives direct access to the staged data
	// WARNING: Use this VERY carefully!
	virtual void* GetStagedData() = 0;
};

SP_NMSPACE_END
