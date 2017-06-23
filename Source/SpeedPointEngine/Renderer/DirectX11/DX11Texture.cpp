//////////////////////////////////////////////////////////////////////////////////////////////////
//
//	SpeedPoint Game Engine
//	Copyright (c) 2011-2016 Pascal Rosenkranz, All rights reserved.
//
//////////////////////////////////////////////////////////////////////////////////////////////////

#include "DX11Texture.h"
#include "DX11Renderer.h"
#include "..\IRenderer.h"

#include <d2d1.h>
#include <wincodec.h>
#include <memory>

SP_NMSPACE_BEG


const char* GetWICPixelFormatName(WICPixelFormatGUID fmt)
{
	if (fmt == GUID_WICPixelFormatDontCare) return "GUID_WICPixelFormatDontCare";
	else if (fmt == GUID_WICPixelFormat1bppIndexed) return "GUID_WICPixelFormat1bppIndexed";
	else if (fmt == GUID_WICPixelFormat2bppIndexed) return "GUID_WICPixelFormat2bppIndexed";
	else if (fmt == GUID_WICPixelFormat4bppIndexed) return "GUID_WICPixelFormat4bppIndexed";
	else if (fmt == GUID_WICPixelFormat8bppIndexed) return "GUID_WICPixelFormat8bppIndexed";
	else if (fmt == GUID_WICPixelFormatBlackWhite) return "GUID_WICPixelFormatBlackWhite";
	else if (fmt == GUID_WICPixelFormat2bppGray) return "GUID_WICPixelFormat2bppGray";
	else if (fmt == GUID_WICPixelFormat4bppGray) return "GUID_WICPixelFormat4bppGray";
	else if (fmt == GUID_WICPixelFormat8bppGray) return "GUID_WICPixelFormat8bppGray";
	else if (fmt == GUID_WICPixelFormat8bppAlpha) return "GUID_WICPixelFormat8bppAlpha";
	else if (fmt == GUID_WICPixelFormat16bppBGR555) return "GUID_WICPixelFormat16bppBGR555";
	else if (fmt == GUID_WICPixelFormat16bppBGR565) return "GUID_WICPixelFormat16bppBGR565";
	else if (fmt == GUID_WICPixelFormat16bppBGRA5551) return "GUID_WICPixelFormat16bppBGRA5551";
	else if (fmt == GUID_WICPixelFormat16bppGray) return "GUID_WICPixelFormat16bppGray";
	else if (fmt == GUID_WICPixelFormat24bppBGR) return "GUID_WICPixelFormat24bppBGR";
	else if (fmt == GUID_WICPixelFormat24bppRGB) return "GUID_WICPixelFormat24bppRGB";
	else if (fmt == GUID_WICPixelFormat32bppBGR) return "GUID_WICPixelFormat32bppBGR";
	else if (fmt == GUID_WICPixelFormat32bppBGRA) return "GUID_WICPixelFormat32bppBGRA";
	else if (fmt == GUID_WICPixelFormat32bppPBGRA) return "GUID_WICPixelFormat32bppPBGRA";
	else if (fmt == GUID_WICPixelFormat32bppGrayFloat) return "GUID_WICPixelFormat32bppGrayFloat";
	else if (fmt == GUID_WICPixelFormat32bppRGB) return "GUID_WICPixelFormat32bppRGB";
	else if (fmt == GUID_WICPixelFormat32bppRGBA) return "GUID_WICPixelFormat32bppRGBA";
	else if (fmt == GUID_WICPixelFormat32bppPRGBA) return "GUID_WICPixelFormat32bppPRGBA";
	else if (fmt == GUID_WICPixelFormat48bppRGB) return "GUID_WICPixelFormat48bppRGB";
	else if (fmt == GUID_WICPixelFormat48bppBGR) return "GUID_WICPixelFormat48bppBGR";
	else if (fmt == GUID_WICPixelFormat64bppRGB) return "GUID_WICPixelFormat64bppRGB";
	else if (fmt == GUID_WICPixelFormat64bppRGBA) return "GUID_WICPixelFormat64bppRGBA";
	else if (fmt == GUID_WICPixelFormat64bppBGRA) return "GUID_WICPixelFormat64bppBGRA";
	else if (fmt == GUID_WICPixelFormat64bppPRGBA) return "GUID_WICPixelFormat64bppPRGBA";
	else if (fmt == GUID_WICPixelFormat64bppPBGRA) return "GUID_WICPixelFormat64bppPBGRA";
	else if (fmt == GUID_WICPixelFormat16bppGrayFixedPoint) return "GUID_WICPixelFormat16bppGrayFixedPoint";
	else if (fmt == GUID_WICPixelFormat32bppBGR101010) return "GUID_WICPixelFormat32bppBGR101010";
	else if (fmt == GUID_WICPixelFormat48bppRGBFixedPoint) return "GUID_WICPixelFormat48bppRGBFixedPoint";
	else if (fmt == GUID_WICPixelFormat48bppBGRFixedPoint) return "GUID_WICPixelFormat48bppBGRFixedPoint";
	else if (fmt == GUID_WICPixelFormat96bppRGBFixedPoint) return "GUID_WICPixelFormat96bppRGBFixedPoint";
	else if (fmt == GUID_WICPixelFormat96bppRGBFloat) return "GUID_WICPixelFormat96bppRGBFloat";
	else if (fmt == GUID_WICPixelFormat128bppRGBAFloat) return "GUID_WICPixelFormat128bppRGBAFloat";
	else if (fmt == GUID_WICPixelFormat128bppPRGBAFloat) return "GUID_WICPixelFormat128bppPRGBAFloat";
	else if (fmt == GUID_WICPixelFormat128bppRGBFloat) return "GUID_WICPixelFormat128bppRGBFloat";
	else if (fmt == GUID_WICPixelFormat32bppCMYK) return "GUID_WICPixelFormat32bppCMYK";
	else if (fmt == GUID_WICPixelFormat64bppRGBAFixedPoint) return "GUID_WICPixelFormat64bppRGBAFixedPoint";
	else if (fmt == GUID_WICPixelFormat64bppBGRAFixedPoint) return "GUID_WICPixelFormat64bppBGRAFixedPoint";
	else if (fmt == GUID_WICPixelFormat64bppRGBFixedPoint) return "GUID_WICPixelFormat64bppRGBFixedPoint";
	else if (fmt == GUID_WICPixelFormat128bppRGBAFixedPoint) return "GUID_WICPixelFormat128bppRGBAFixedPoint";
	else if (fmt == GUID_WICPixelFormat128bppRGBFixedPoint) return "GUID_WICPixelFormat128bppRGBFixedPoint";
	else if (fmt == GUID_WICPixelFormat64bppRGBAHalf) return "GUID_WICPixelFormat64bppRGBAHalf";
	else if (fmt == GUID_WICPixelFormat64bppRGBHalf) return "GUID_WICPixelFormat64bppRGBHalf";
	else if (fmt == GUID_WICPixelFormat48bppRGBHalf) return "GUID_WICPixelFormat48bppRGBHalf";
	else if (fmt == GUID_WICPixelFormat32bppRGBE) return "GUID_WICPixelFormat32bppRGBE";
	else if (fmt == GUID_WICPixelFormat16bppGrayHalf) return "GUID_WICPixelFormat16bppGrayHalf";
	else if (fmt == GUID_WICPixelFormat32bppGrayFixedPoint) return "GUID_WICPixelFormat32bppGrayFixedPoint";
	else if (fmt == GUID_WICPixelFormat32bppRGBA1010102) return "GUID_WICPixelFormat32bppRGBA1010102";
	else if (fmt == GUID_WICPixelFormat32bppRGBA1010102XR) return "GUID_WICPixelFormat32bppRGBA1010102XR";
	else if (fmt == GUID_WICPixelFormat64bppCMYK) return "GUID_WICPixelFormat64bppCMYK";
	else if (fmt == GUID_WICPixelFormat24bpp3Channels) return "GUID_WICPixelFormat24bpp3Channels";
	else if (fmt == GUID_WICPixelFormat32bpp4Channels) return "GUID_WICPixelFormat32bpp4Channels";
	else if (fmt == GUID_WICPixelFormat40bpp5Channels) return "GUID_WICPixelFormat40bpp5Channels";
	else if (fmt == GUID_WICPixelFormat48bpp6Channels) return "GUID_WICPixelFormat48bpp6Channels";
	else if (fmt == GUID_WICPixelFormat56bpp7Channels) return "GUID_WICPixelFormat56bpp7Channels";
	else if (fmt == GUID_WICPixelFormat64bpp8Channels) return "GUID_WICPixelFormat64bpp8Channels";
	else if (fmt == GUID_WICPixelFormat48bpp3Channels) return "GUID_WICPixelFormat48bpp3Channels";
	else if (fmt == GUID_WICPixelFormat64bpp4Channels) return "GUID_WICPixelFormat64bpp4Channels";
	else if (fmt == GUID_WICPixelFormat80bpp5Channels) return "GUID_WICPixelFormat80bpp5Channels";
	else if (fmt == GUID_WICPixelFormat96bpp6Channels) return "GUID_WICPixelFormat96bpp6Channels";
	else if (fmt == GUID_WICPixelFormat112bpp7Channels) return "GUID_WICPixelFormat112bpp7Channels";
	else if (fmt == GUID_WICPixelFormat128bpp8Channels) return "GUID_WICPixelFormat128bpp8Channels";
	else if (fmt == GUID_WICPixelFormat40bppCMYKAlpha) return "GUID_WICPixelFormat40bppCMYKAlpha";
	else if (fmt == GUID_WICPixelFormat80bppCMYKAlpha) return "GUID_WICPixelFormat80bppCMYKAlpha";
	else if (fmt == GUID_WICPixelFormat32bpp3ChannelsAlpha) return "GUID_WICPixelFormat32bpp3ChannelsAlpha";
	else if (fmt == GUID_WICPixelFormat40bpp4ChannelsAlpha) return "GUID_WICPixelFormat40bpp4ChannelsAlpha";
	else if (fmt == GUID_WICPixelFormat48bpp5ChannelsAlpha) return "GUID_WICPixelFormat48bpp5ChannelsAlpha";
	else if (fmt == GUID_WICPixelFormat56bpp6ChannelsAlpha) return "GUID_WICPixelFormat56bpp6ChannelsAlpha";
	else if (fmt == GUID_WICPixelFormat64bpp7ChannelsAlpha) return "GUID_WICPixelFormat64bpp7ChannelsAlpha";
	else if (fmt == GUID_WICPixelFormat72bpp8ChannelsAlpha) return "GUID_WICPixelFormat72bpp8ChannelsAlpha";
	else if (fmt == GUID_WICPixelFormat64bpp3ChannelsAlpha) return "GUID_WICPixelFormat64bpp3ChannelsAlpha";
	else if (fmt == GUID_WICPixelFormat80bpp4ChannelsAlpha) return "GUID_WICPixelFormat80bpp4ChannelsAlpha";
	else if (fmt == GUID_WICPixelFormat96bpp5ChannelsAlpha) return "GUID_WICPixelFormat96bpp5ChannelsAlpha";
	else if (fmt == GUID_WICPixelFormat112bpp6ChannelsAlpha) return "GUID_WICPixelFormat112bpp6ChannelsAlpha";
	else if (fmt == GUID_WICPixelFormat128bpp7ChannelsAlpha) return "GUID_WICPixelFormat128bpp7ChannelsAlpha";
	else if (fmt == GUID_WICPixelFormat144bpp8ChannelsAlpha) return "GUID_WICPixelFormat144bpp8ChannelsAlpha";
	else if (fmt == GUID_WICPixelFormat8bppY) return "GUID_WICPixelFormat8bppY";
	else if (fmt == GUID_WICPixelFormat8bppCb) return "GUID_WICPixelFormat8bppCb";
	else if (fmt == GUID_WICPixelFormat8bppCr) return "GUID_WICPixelFormat8bppCr";
	else if (fmt == GUID_WICPixelFormat16bppCbCr) return "GUID_WICPixelFormat16bppCbCr";
	else if (fmt == GUID_WICPixelFormat16bppYQuantizedDctCoefficients) return "GUID_WICPixelFormat16bppYQuantizedDctCoefficients";
	else if (fmt == GUID_WICPixelFormat16bppCbQuantizedDctCoefficients) return "GUID_WICPixelFormat16bppCbQuantizedDctCoefficients";
	else if (fmt == GUID_WICPixelFormat16bppCrQuantizedDctCoefficients) return "GUID_WICPixelFormat16bppCrQuantizedDctCoefficients";
	else return "Unknown format";
}


#define TEXTURE_FORMAT_MAPPING(format1, format2) \
	if (fmt1Tofmt2 & (fmt1 == format1)) { fmt2 = format2; return; } \
	else if (fmt2 == format2) { fmt1 = format1; return; }

S_API static void MapTextureType(DXGI_FORMAT& fmt1, ETextureType& fmt2, bool fmt1Tofmt2)
{
	TEXTURE_FORMAT_MAPPING(DXGI_FORMAT_R32_FLOAT, eTEXTURE_R32_FLOAT);
	TEXTURE_FORMAT_MAPPING(DXGI_FORMAT_R32_TYPELESS, eTEXTURE_R32_FLOAT);
	TEXTURE_FORMAT_MAPPING(DXGI_FORMAT_D32_FLOAT, eTEXTURE_D32_FLOAT);
	TEXTURE_FORMAT_MAPPING(DXGI_FORMAT_R16G16B16A16_FLOAT, eTEXTURE_R16G16B16A16_FLOAT);
	TEXTURE_FORMAT_MAPPING(DXGI_FORMAT_R16G16_FLOAT, eTEXTURE_R16G16_FLOAT);
	TEXTURE_FORMAT_MAPPING(DXGI_FORMAT_R8G8B8A8_UNORM, eTEXTURE_R8G8B8A8_UNORM);

	if (fmt1Tofmt2)
	{
		CLog::Log(S_WARNING, "Could not map dxgi format %d to SpeedPoint texture type", (int)fmt1);
		fmt2 = eTEXTURE_R8G8B8A8_UNORM;
	}
	else
	{
		CLog::Log(S_WARNING, "Could not map texture type %s to DXGI format", GetTextureTypeName(fmt2));
		fmt1 = DXGI_FORMAT_R8G8B8A8_UNORM;
	}
}



// -----------------------------------------------------------------------------------------------
DX11Texture::DX11Texture(DX11Renderer* pDXRenderer)
: m_pDXRenderer(pDXRenderer),
m_Specification("?notinitialized?"),
m_bDynamic(false),
m_pDXTexture(0),
m_pDXSRV(nullptr),
m_pLockedData(nullptr),
m_nLockedBytes(0),
m_pStagedData(0),
m_bStaged(false),
m_bLocked(false),
m_bIsCubemap(false),
m_bArray(false)
{
}

// -----------------------------------------------------------------------------------------------
DX11Texture::~DX11Texture()
{
	if (m_RefCount > 0)
	{
		CLog::Log(S_WARNING, "Warning: Destructing texture 0x%08X ('%s') with refcount = %u",
			(unsigned long)m_pDXTexture, m_Specification.c_str(), m_RefCount);
	}

	Clear();
}

// -----------------------------------------------------------------------------------------------
S_API SResult DX11Texture::LoadTextureImage(const string& cFileName, unsigned int& w, unsigned int& h, unsigned char** pBuffer, size_t& imageStride, size_t& imageSize, DXGI_FORMAT& loadedTextureFmt)
{
	HRESULT hRes;

	IWICImagingFactory* pImgFactory;
	hRes = CoCreateInstance(CLSID_WICImagingFactory, 0, CLSCTX_INPROC_SERVER, IID_IWICImagingFactory, (void**)&pImgFactory);
	if (Failure(hRes))
	{
		return CLog::Log(S_ERROR, "Failed Create WIC Imaging Factory!");
	}

	IWICBitmapDecoder* pImgDecoder;
	wchar_t* cWFilename = new wchar_t[cFileName.length() + 1];
	size_t nNumCharsConv;
	mbstowcs_s(&nNumCharsConv, cWFilename, cFileName.length() + 1, cFileName.c_str(), _TRUNCATE);
	hRes = pImgFactory->CreateDecoderFromFilename(cWFilename, 0, GENERIC_READ, WICDecodeMetadataCacheOnDemand, &pImgDecoder);
	if (Failure(hRes))
	{
		CLog::Log(S_DEBUG, "Failed Create WIC Image decoder for %s (cWFilename=%S)!", cFileName.c_str(), cWFilename);
		return S_ERROR;
	}



	//
	// TODO:
	//	for animated texture, loop through all frames and call pImgFactory->GetFrameCount() first.
	//

	IWICBitmapFrameDecode* pBmpFrameDecode;
	pImgDecoder->GetFrame(0, &pBmpFrameDecode);

	// get the pixel size of this frame
	WICPixelFormatGUID pxlFmtGUID;
	if (Failure(pBmpFrameDecode->GetPixelFormat(&pxlFmtGUID)))
	{
		return CLog::Log(S_ERROR, "Failed Get Pixel Format of desired frame!");
	}

	// some pixel format cannot be directly translated into a DXGI Format. So find a nearest match.	
	WICPixelFormatGUID pxlFmtGUIDOrig = pxlFmtGUID;
	if (pxlFmtGUID == GUID_WICPixelFormat24bppBGR) pxlFmtGUID = GUID_WICPixelFormat32bppRGBA;
	else if (pxlFmtGUID == GUID_WICPixelFormat8bppIndexed) pxlFmtGUID = GUID_WICPixelFormat32bppRGBA;
	else if (pxlFmtGUID == GUID_WICPixelFormat32bppPBGRA) pxlFmtGUID = GUID_WICPixelFormat32bppRGBA;

	// we'll probably need a dxgi format to create the dx texture instance	
	if (pxlFmtGUID == GUID_WICPixelFormat128bppRGBAFloat)		loadedTextureFmt = DXGI_FORMAT_R32G32B32A32_FLOAT;
	else if (pxlFmtGUID == GUID_WICPixelFormat64bppRGBAHalf)	loadedTextureFmt = DXGI_FORMAT_R16G16B16A16_FLOAT;
	else if (pxlFmtGUID == GUID_WICPixelFormat64bppRGBA)		loadedTextureFmt = DXGI_FORMAT_R16G16B16A16_UNORM;
	else if (pxlFmtGUID == GUID_WICPixelFormat32bppRGBA)		loadedTextureFmt = DXGI_FORMAT_R8G8B8A8_UNORM;
	else if (pxlFmtGUID == GUID_WICPixelFormat32bppBGRA)		loadedTextureFmt = DXGI_FORMAT_B8G8R8A8_UNORM;
	else if (pxlFmtGUID == GUID_WICPixelFormat32bppBGR)		loadedTextureFmt = DXGI_FORMAT_B8G8R8X8_UNORM;
	else if (pxlFmtGUID == GUID_WICPixelFormat32bppRGBA1010102XR)	loadedTextureFmt = DXGI_FORMAT_R10G10B10_XR_BIAS_A2_UNORM;
	else if (pxlFmtGUID == GUID_WICPixelFormat32bppRGBA1010102)	loadedTextureFmt = DXGI_FORMAT_R10G10B10A2_UNORM;
	else if (pxlFmtGUID == GUID_WICPixelFormat32bppRGBE)		loadedTextureFmt = DXGI_FORMAT_R9G9B9E5_SHAREDEXP;
	else if (pxlFmtGUID == GUID_WICPixelFormat16bppBGRA5551)	loadedTextureFmt = DXGI_FORMAT_B5G5R5A1_UNORM;
	else if (pxlFmtGUID == GUID_WICPixelFormat16bppBGR565)		loadedTextureFmt = DXGI_FORMAT_B5G6R5_UNORM;
	else if (pxlFmtGUID == GUID_WICPixelFormat32bppGrayFloat)	loadedTextureFmt = DXGI_FORMAT_R32_FLOAT;
	else if (pxlFmtGUID == GUID_WICPixelFormat16bppGrayHalf)	loadedTextureFmt = DXGI_FORMAT_R16_FLOAT;
	else if (pxlFmtGUID == GUID_WICPixelFormat16bppGray)		loadedTextureFmt = DXGI_FORMAT_R16_UNORM;
	else if (pxlFmtGUID == GUID_WICPixelFormat8bppGray)		loadedTextureFmt = DXGI_FORMAT_R8_UNORM;
	else if (pxlFmtGUID == GUID_WICPixelFormat8bppAlpha)		loadedTextureFmt = DXGI_FORMAT_A8_UNORM;
#if (_WIN32_WINNT >= _WIN32_WINNT_WIN8)
	else if (pxlFmtGUID == GUID_WICPixelFormat96bppRGBFloat)	loadedTextureFmt = DXGI_FORMAT_R32G32B32_FLOAT;
#endif
	else
	{
		return CLog::Log(S_ERROR, "Unsupported pixel fmt of texture: %s!", GetWICPixelFormatName(pxlFmtGUID));
	}

	// ----------------------------------------------------------------------------------------------------------------------
	// copy the pixels into a temporary buffer	

	UINT nLoadedWidth, nLoadedHeight;
	pBmpFrameDecode->GetSize(&nLoadedWidth, &nLoadedHeight);

	if (w == 0 || h == 0)
	{
		w = nLoadedWidth;
		h = nLoadedHeight;
	}

	UINT nBPP = BitsPerPixel(pxlFmtGUID, pImgFactory);
	if (nBPP == 0)
	{
		return CLog::Log(S_ERROR, "Could not retrieve bits per pixel for loaded texture!");
	}

	imageStride = (w * nBPP + 7) / 8;
	imageSize = imageStride * nLoadedHeight;

	uint8_t* temp = new uint8_t[imageSize];

	// ----------------------------------------------------------------------------------------------------------------------
	// check whether we need to scale or convert the loaded image

	if (nLoadedWidth == w && nLoadedHeight == h && memcmp(&pxlFmtGUID, &pxlFmtGUIDOrig, sizeof(GUID)) == 0)
	{
		hRes = pBmpFrameDecode->CopyPixels(0, static_cast<UINT>(imageStride), static_cast<UINT>(imageSize), temp);
		if (Failure(hRes))
			return CLog::Log(S_ERROR, "Failed to buffer texture!");
	}
	else if (nLoadedWidth != w && nLoadedHeight != h)
	{
		ScopedTextureLoadingObject<IWICBitmapScaler> pScaler;
		if (Failure(pImgFactory->CreateBitmapScaler(&pScaler)))
		{
			CLog::Log(S_DEBUG, "Failed pImgFactory->CreateBitmapScalar()");
			return S_ERROR;
		}

		if (Failure(pScaler->Initialize(pBmpFrameDecode, w, h,
			WICBitmapInterpolationModeFant))) // Maybe change this someday??
		{
			CLog::Log(S_DEBUG, "Failed pScaler->Initialize()");
			return S_ERROR;
		}

		WICPixelFormatGUID pfScaler;
		if (Failure(pScaler->GetPixelFormat(&pfScaler)))
		{
			CLog::Log(S_DEBUG, "Failed pScaler->GetPixelFormat()");
			return S_ERROR;
		}

		if (memcmp(&pxlFmtGUID, &pfScaler, sizeof(GUID)) == 0)
		{
			hRes = pScaler->CopyPixels(0, static_cast<UINT>(imageStride), static_cast<UINT>(imageSize), temp);
			if (Failure(hRes))
				return CLog::Log(S_ERROR, "Failed to buffer scaled texture!");
		}
		else
		{
			ScopedTextureLoadingObject<IWICFormatConverter> formatConverter;
			if (Failure(pImgFactory->CreateFormatConverter(&formatConverter)))
				return CLog::Log(S_ERROR, "Failed to create pixel format convert (1)!");

			hRes = formatConverter->Initialize(pScaler.Get(), pxlFmtGUID, WICBitmapDitherTypeErrorDiffusion, 0, 0, WICBitmapPaletteTypeCustom);
			if (Failure(hRes))
				return CLog::Log(S_ERROR, "Failed to initialize format converter (1)!");

			hRes = formatConverter->CopyPixels(0, static_cast<UINT>(imageStride), static_cast<UINT>(imageSize), temp);
			if (Failure(hRes))
				return CLog::Log(S_ERROR, "Failed to convert and copy pixels (1)!");
		}
	}
	else // convert only
	{
		ScopedTextureLoadingObject<IWICFormatConverter> formatConverter;
		if (Failure(pImgFactory->CreateFormatConverter(&formatConverter)))
			return CLog::Log(S_ERROR, "Failed to create pixel format convert (2)!");

		hRes = formatConverter->Initialize(pBmpFrameDecode, pxlFmtGUID, WICBitmapDitherTypeErrorDiffusion, 0, 0, WICBitmapPaletteTypeCustom);
		if (Failure(hRes))
			return CLog::Log(S_ERROR, "Failed to initialize format converter (2)!");

		hRes = formatConverter->CopyPixels(0, static_cast<UINT>(imageStride), static_cast<UINT>(imageSize), temp);
		if (Failure(hRes))
			return CLog::Log(S_ERROR, "Failed to convert and copy pixels (2)!");
	}


	*pBuffer = temp;


	SP_SAFE_RELEASE(pImgDecoder);
	SP_SAFE_RELEASE(pImgFactory);

	return S_SUCCESS;
}



// -----------------------------------------------------------------------------------------------
S_API void DX11Texture::GetCubemapImageName(string& name, ECubemapSide side)
{
	// Convert side to DX array slice side
	ECubemapSide dxSide = (ECubemapSide)GetDXCubemapArraySlice(side);

	switch (dxSide)
	{
	case eCUBEMAP_SIDE_NEGX: name += "_negx"; break;
	case eCUBEMAP_SIDE_NEGY: name += "_negy"; break;
	case eCUBEMAP_SIDE_NEGZ: name += "_negz"; break;
	case eCUBEMAP_SIDE_POSX: name += "_posx"; break;
	case eCUBEMAP_SIDE_POSY: name += "_posy"; break;
	case eCUBEMAP_SIDE_POSZ: name += "_posz"; break;
	default:
		CLog::Log(S_ERROR, "Invalid cubmap side: %d", (unsigned int)side);
		return;
	}

	name = name + ".bmp";
}

S_API unsigned int DX11Texture::GetDXCubemapArraySlice(ECubemapSide side)
{
	// Visit msdn at https://msdn.microsoft.com/en-us/library/windows/desktop/ff476906(v=vs.85).aspx	
	switch (side)
	{
	case eCUBEMAP_SIDE_NEGX: return 3; // -> POSX		
	case eCUBEMAP_SIDE_NEGY: return 0; // -> NEGX
	case eCUBEMAP_SIDE_NEGZ: return 4; // -> POSY
	case eCUBEMAP_SIDE_POSX: return 1; // -> NEGY
	case eCUBEMAP_SIDE_POSY: return 5; // -> POSZ
	case eCUBEMAP_SIDE_POSZ: return 2; // -> NEGZ
	default:
		return 0;
	}
}





// -----------------------------------------------------------------------------------------------
S_API SResult DX11Texture::LoadCubemapFromFile(const string& baseName, unsigned int singleW /*=0*/, unsigned int singleH /*=0*/)
{
	Clear();

	if (!IS_VALID_PTR(m_pDXRenderer))
		return CLog::Log(S_ERROR, "DX11Texture::LoadCubemapFromFile(): Renderer not initialized");

	SResult res;
	HRESULT hRes;

	// ----------------------------------------------------------------------------------------------------------------------
	DXGI_FORMAT loadedTextureFmt;
	std::vector<SLoadedCubemapSide> cmSides;	
		
	for (unsigned int i = 0; i < 6; ++i)
	{
		string filename = baseName;
		GetCubemapImageName(filename, (ECubemapSide)i);

		CLog::Log(S_DEBUG, "Cubmap Side Filename: %s", filename.c_str());

		SLoadedCubemapSide side;
		DXGI_FORMAT sideFmt;

		res = LoadTextureImage(filename, singleW, singleH, &side.pBuffer, side.imageStride, side.imageSize, sideFmt);
		if (Failure(res))
		{			
			for (auto itSide = cmSides.begin(); itSide != cmSides.end(); itSide++)
				delete[] itSide->pBuffer;

			return CLog::Log(S_ERROR, "Failed load cubemap image #%d", i);
		}		

		if (i == 0)
		{
			loadedTextureFmt = sideFmt;
		}
		else if (sideFmt != loadedTextureFmt)
		{
			for (auto itSide = cmSides.begin(); itSide != cmSides.end(); itSide++)
				delete[] itSide->pBuffer;

			return CLog::Log(S_ERROR, "Cubemap image side #%d format mismatch (%d - %d).", i, (unsigned int)loadedTextureFmt, (unsigned int)sideFmt);
		}

		cmSides.push_back(side);
	}	


	// ----------------------------------------------------------------------------------------------------------------------
	// Check if autogeneration of mip levels is supported

	UINT fmtSupport = 0;
	hRes = m_pDXRenderer->GetD3D11Device()->CheckFormatSupport(loadedTextureFmt, &fmtSupport);
	bool bMipAutoGenSupported = Success(hRes) && (fmtSupport & D3D11_FORMAT_SUPPORT_MIP_AUTOGEN);



	// ----------------------------------------------------------------------------------------------------------------------
	// Now create the directx texture

	unsigned int mipLevels = 2;

	D3D11_TEXTURE2D_DESC& textureDesc = m_DXTextureDesc;
	textureDesc.ArraySize = 6;
	textureDesc.BindFlags = (bMipAutoGenSupported) ? (D3D11_BIND_SHADER_RESOURCE | D3D11_BIND_RENDER_TARGET) : (D3D11_BIND_SHADER_RESOURCE);
	textureDesc.CPUAccessFlags = (m_bDynamic) ? D3D11_CPU_ACCESS_WRITE /* | D3D11_CPU_ACCESS_READ */ : 0;
	textureDesc.Format = loadedTextureFmt;
	textureDesc.Width = singleW;
	textureDesc.Height = singleH;
	textureDesc.SampleDesc.Count = 1;
	textureDesc.SampleDesc.Quality = 0; // No MS for now!

	textureDesc.MiscFlags = (bMipAutoGenSupported) ? D3D11_RESOURCE_MISC_GENERATE_MIPS : 0;
	textureDesc.MiscFlags |= D3D11_RESOURCE_MISC_TEXTURECUBE;

	textureDesc.BindFlags = (bMipAutoGenSupported) ? (D3D11_BIND_SHADER_RESOURCE | D3D11_BIND_RENDER_TARGET) : (D3D11_BIND_SHADER_RESOURCE);
	textureDesc.CPUAccessFlags = 0;
	textureDesc.Usage = D3D11_USAGE_DEFAULT;
	textureDesc.MipLevels = (bMipAutoGenSupported) ? mipLevels : 1;

	D3D11_SUBRESOURCE_DATA initData[6];

	for (unsigned int i = 0; i < 6; ++i)
	{
		initData[i].pSysMem = (void*)cmSides[i].pBuffer;
		initData[i].SysMemPitch = static_cast<UINT>(cmSides[i].imageStride);
		initData[i].SysMemSlicePitch = static_cast<UINT>(cmSides[i].imageSize);
	}

	m_pDXTexture = nullptr;

	// Create the texture
	hRes = m_pDXRenderer->GetD3D11Device()->CreateTexture2D(&textureDesc, (bMipAutoGenSupported) ? nullptr : &initData[0], &m_pDXTexture);

	if (Failure(hRes) || m_pDXTexture == nullptr)
	{
		return CLog::Log(S_ERROR, "Failed to create DirectX11 Texture!");
	}


	// ----------------------------------------------------------------------------------------------------------------------
	// Create the Shader Resource View

	D3D11_SHADER_RESOURCE_VIEW_DESC srvDesc;
	memset(&srvDesc, 0, sizeof(srvDesc));
	srvDesc.Format = loadedTextureFmt;
	srvDesc.ViewDimension = D3D11_SRV_DIMENSION_TEXTURECUBE;
	//srvDesc.Texture2D.MipLevels = (bMipAutoGenSupported) ? -1 : 1;
	srvDesc.TextureCube.MipLevels = (bMipAutoGenSupported) ? mipLevels : 1;
	srvDesc.TextureCube.MostDetailedMip = 0;

	hRes = m_pDXRenderer->GetD3D11Device()->CreateShaderResourceView(m_pDXTexture, &srvDesc, &m_pDXSRV);

	if (Failure(hRes))
	{
		m_pDXTexture->Release();
		return CLog::Log(S_ERROR, "Failed create shader resource view for texture!");
	}

	if (bMipAutoGenSupported)
	{
		ID3D11DeviceContext* pDXDevCon = m_pDXRenderer->GetD3D11DeviceContext();
		for (unsigned int i = 0; i < 6; ++i)
		{
			unsigned int subresource = srvDesc.TextureCube.MipLevels * i;			
			pDXDevCon->UpdateSubresource(m_pDXTexture, subresource, nullptr, cmSides[i].pBuffer, static_cast<UINT>(cmSides[i].imageStride), static_cast<UINT>(cmSides[i].imageSize));
		}

		pDXDevCon->GenerateMips(m_pDXSRV);
	}

	// ----------------------------------------------------------------------------------------------------------------------
	// Determine type

	switch (loadedTextureFmt)
	{
	case DXGI_FORMAT_R32_FLOAT: m_Type = eTEXTURE_R32_FLOAT; break;
	case DXGI_FORMAT_D32_FLOAT: m_Type = eTEXTURE_D32_FLOAT; break;
	case DXGI_FORMAT_R8G8B8A8_UNORM:
	default:
		m_Type = eTEXTURE_R8G8B8A8_UNORM; break;
	}

	for (auto itSide = cmSides.begin(); itSide != cmSides.end(); itSide++)
		delete[] itSide->pBuffer;

	m_bIsCubemap = true;
	return S_SUCCESS;



	// D3D11_SRV_DIMENSION_TEXTURECUBE

	/*
	D3D11_TEXTURE2D_DESC texDesc;
texDesc.Width = description.width;
texDesc.Height = description.height;
texDesc.MipLevels = 1;
texDesc.ArraySize = 6;
texDesc.Format = DXGI_FORMAT_R8G8B8A8_UNORM;
texDesc.CPUAccessFlags = 0;
texDesc.SampleDesc.Count = 1;
texDesc.SampleDesc.Quality = 0;
texDesc.Usage = D3D11_USAGE_DEFAULT;
texDesc.BindFlags = D3D11_BIND_SHADER_RESOURCE;
texDesc.CPUAccessFlags = 0;
texDesc.MiscFlags = D3D11_RESOURCE_MISC_TEXTURECUBE;

D3D11_SHADER_RESOURCE_VIEW_DESC SMViewDesc;
SMViewDesc.Format = texDesc.Format;
SMViewDesc.ViewDimension = D3D11_SRV_DIMENSION_TEXTURECUBE;
SMViewDesc.TextureCube.MipLevels =  texDesc.MipLevels;
SMViewDesc.TextureCube.MostDetailedMip = 0;

D3D11_SUBRESOURCE_DATA pData[6];
std::vector<vector4b> d[6]; // 6 images of type vector4b = 4 * unsigned char

for (int cubeMapFaceIndex = 0; cubeMapFaceIndex < 6; cubeMapFaceIndex++)
{   
    d[cubeMapFaceIndex].resize(description.width * description.height);

    // fill with red color  
    std::fill(
        d[cubeMapFaceIndex].begin(), 
        d[cubeMapFaceIndex].end(), 
        vector4b(255,0,0,255));

    pData[cubeMapFaceIndex].pSysMem = &d[cubeMapFaceIndex][0];// description.data;
    pData[cubeMapFaceIndex].SysMemPitch = description.width * 4;
    pData[cubeMapFaceIndex].SysMemSlicePitch = 0;
}

HRESULT hr = renderer->getDevice()->CreateTexture2D(&texDesc, 
    description.data[0] ? &pData[0] : nullptr, &m_pCubeTexture);
assert(hr == S_OK);

hr = renderer->getDevice()->CreateShaderResourceView(
    m_pCubeTexture, &SMViewDesc, &m_pShaderResourceView);
assert(hr == S_OK);
	*/
}

// -----------------------------------------------------------------------------------------------
size_t DX11Texture::BitsPerPixel(REFGUID targetGuid, IWICImagingFactory* pWIC)
{
	if (!pWIC)
		return 0;

	ScopedTextureLoadingObject<IWICComponentInfo> comInfo;
	if (Failure(pWIC->CreateComponentInfo(targetGuid, &comInfo)))
		return 0;

	// make sure we got the correct component type
	WICComponentType comType;
	if (Failure(comInfo->GetComponentType(&comType)))
		return 0;
	if (comType != WICPixelFormat)
		return 0;

	ScopedTextureLoadingObject<IWICPixelFormatInfo> pxlFmtInfo;
	if (Failure(comInfo->QueryInterface(__uuidof(IWICPixelFormatInfo), reinterpret_cast<void**>(&pxlFmtInfo))))
		return 0;

	UINT bpp;
	if (Failure(pxlFmtInfo->GetBitsPerPixel(&bpp)))
		return 0;

	return bpp;
}





// -----------------------------------------------------------------------------------------------
// remember that the w and h parameters will specify the output texture size to which the image will be scaled to
S_API SResult DX11Texture::LoadFromFile(const string& cFileName, unsigned int w /*=0*/, unsigned int h /*=0*/, unsigned int mipLevels /*=0*/)
{
	Clear();

	if (!IS_VALID_PTR(m_pDXRenderer))
		return CLog::Log(S_ERROR, "DX11Texture::LoadFromFile('%s'): Renderer not initialized", cFileName.c_str());

	SResult res;
	HRESULT hRes;

	// ----------------------------------------------------------------------------------------------------------------------
	DXGI_FORMAT loadedTextureFmt;
	unsigned char* pBuffer = 0;
	size_t imageStride, imageSize;

	res = LoadTextureImage(cFileName, w, h, &pBuffer, imageStride, imageSize, loadedTextureFmt);
	if (Failure(res))
	{
		return CLog::Log(S_ERROR, "DX11Texture::LoadFromFile('%s'): Failed LoadTextureImage()", cFileName.c_str());
	}


	// ----------------------------------------------------------------------------------------------------------------------
	// Check if autogeneration of mip levels is supported

	bool bMipAutoGenSupported = CheckMipMapAutogenSupported(loadedTextureFmt);

	// ----------------------------------------------------------------------------------------------------------------------
	// Now create the directx texture
	
	D3D11_TEXTURE2D_DESC& textureDesc = m_DXTextureDesc;
	textureDesc.ArraySize = 1;
	textureDesc.BindFlags = (bMipAutoGenSupported) ? (D3D11_BIND_SHADER_RESOURCE | D3D11_BIND_RENDER_TARGET) : (D3D11_BIND_SHADER_RESOURCE);
	textureDesc.CPUAccessFlags = (m_bDynamic) ? D3D11_CPU_ACCESS_WRITE /* | D3D11_CPU_ACCESS_READ */ : 0;
	textureDesc.Format = loadedTextureFmt;
	textureDesc.Width = w;
	textureDesc.Height = h;	
	textureDesc.SampleDesc.Count = 1;
	textureDesc.SampleDesc.Quality = 0; // No MS for now!

	if (!m_bDynamic)
	{
		textureDesc.MiscFlags = (bMipAutoGenSupported) ? D3D11_RESOURCE_MISC_GENERATE_MIPS : 0;
		textureDesc.BindFlags = (bMipAutoGenSupported) ? (D3D11_BIND_SHADER_RESOURCE | D3D11_BIND_RENDER_TARGET) : (D3D11_BIND_SHADER_RESOURCE);
		textureDesc.CPUAccessFlags = 0;
		textureDesc.Usage = D3D11_USAGE_DEFAULT;
		textureDesc.MipLevels = (bMipAutoGenSupported) ? 0 : 1;
	}
	else
	{
		textureDesc.MiscFlags = 0;
		textureDesc.BindFlags = D3D11_BIND_SHADER_RESOURCE;
		textureDesc.CPUAccessFlags = D3D11_CPU_ACCESS_WRITE;
		textureDesc.Usage = D3D11_USAGE_DYNAMIC;
		textureDesc.MipLevels = 1;
	}	
	
	D3D11_SUBRESOURCE_DATA initData;
	initData.pSysMem = (void*)pBuffer;
	initData.SysMemPitch = static_cast<UINT>(imageStride);
	initData.SysMemSlicePitch = static_cast<UINT>(imageSize);
	
	m_pDXTexture = nullptr;

	// Create the texture
	hRes = m_pDXRenderer->GetD3D11Device()->CreateTexture2D(&textureDesc, (!m_bDynamic && bMipAutoGenSupported) ? nullptr : &initData, &m_pDXTexture);

	if (Failure(hRes) || m_pDXTexture == nullptr)
	{
		return CLog::Log(S_ERROR, "Failed to create DirectX11 Texture!");
	}

#ifdef _DEBUG
	const string& nm = m_Specification;
	m_pDXTexture->SetPrivateData(WKPDID_D3DDebugObjectName, nm.length(), nm.c_str());
#endif


	// ----------------------------------------------------------------------------------------------------------------------
	// Create the Shader Resource View

	memset(&m_DXSRVDesc, 0, sizeof(m_DXSRVDesc));
	m_DXSRVDesc.Format = loadedTextureFmt;
	m_DXSRVDesc.ViewDimension = D3D11_SRV_DIMENSION_TEXTURE2D;
	
	m_DXSRVDesc.Texture2D.MostDetailedMip = 0;
	if (bMipAutoGenSupported)
		m_DXSRVDesc.Texture2D.MipLevels = (mipLevels == 0 ? -1 : mipLevels);
	else
		m_DXSRVDesc.Texture2D.MipLevels = 1;

	hRes = m_pDXRenderer->GetD3D11Device()->CreateShaderResourceView(m_pDXTexture, &m_DXSRVDesc, &m_pDXSRV);

	if (Failure(hRes))
	{
		m_pDXTexture->Release();
		return CLog::Log(S_ERROR, "Failed create shader resource view for texture!");
	}

	if (!m_bDynamic && bMipAutoGenSupported)
	{
		ID3D11DeviceContext* pDXDevCon = m_pDXRenderer->GetD3D11DeviceContext();
		pDXDevCon->UpdateSubresource(m_pDXTexture, 0, nullptr, pBuffer, static_cast<UINT>(imageStride), static_cast<UINT>(imageSize));
		pDXDevCon->GenerateMips(m_pDXSRV);
	}

	// ----------------------------------------------------------------------------------------------------------------------
	// Determine type

	MapTextureType(loadedTextureFmt, m_Type, true);

	// ----------------------------------------------------------------------------------------------------------------------
	// Store staged data

	if (m_bStaged)
	{
		if (IS_VALID_PTR(m_pStagedData))
			free(m_pStagedData);

		m_pStagedData = malloc(imageSize);
		memcpy(m_pStagedData, pBuffer, imageSize);
	}	

	CLog::Log(S_DEBUG, "Loaded texture %s", cFileName.c_str());

	return S_SUCCESS;
}

// -----------------------------------------------------------------------------------------------
S_API SResult DX11Texture::LoadArraySliceFromFile(unsigned int i, const string& filePath)
{
	SResult res;

	if (!m_pDXTexture)
		return CLog::Log(S_ERROR, "Cannot LoadArraySliceFromFile(): Texture not initialized (%s)", m_Specification.c_str());

	if (!m_bArray)
		return CLog::Log(S_ERROR, "Cannot load array slice in non-array texture (%s)", m_Specification.c_str());

	if (i > m_DXTextureDesc.ArraySize)
		return CLog::Log(S_ERROR, "Cannot load texture array slice %d: Only initialized with %d slices (%s)", i, m_DXTextureDesc.ArraySize, m_Specification.c_str());

	// Load image
	unsigned char* imageData;
	size_t imageStride, imageSize;
	DXGI_FORMAT loadedFmt;

	res = LoadTextureImage(filePath, m_DXTextureDesc.Width, m_DXTextureDesc.Height, &imageData, imageStride, imageSize, loadedFmt);
	
	if (Failure(res))
		return CLog::Log(S_ERROR, "Failed load texture array slice image (%s)", m_Specification.c_str());

	if (loadedFmt != m_DXTextureDesc.Format)
		return CLog::Log(S_ERROR, "DX11Texture(%s)::LoadArraySliceFromFile(%s): Loaded format incompatible with array format", m_Specification.c_str(), filePath.c_str());

	// Fill slice
	ID3D11DeviceContext* pD3DDevCtx = m_pDXRenderer->GetD3D11DeviceContext();
	pD3DDevCtx->UpdateSubresource(m_pDXTexture, i, 0, imageData, (UINT)imageStride, (UINT)imageSize);

	// Update mips
	pD3DDevCtx->GenerateMips(m_pDXSRV);

	return S_SUCCESS;
}

// -----------------------------------------------------------------------------------------------
S_API SResult DX11Texture::ResizeArray(unsigned int count)
{
	HRESULT hr;

	if (!m_pDXTexture || !m_bArray)
		return CLog::Log(S_ERROR, "Failed ResizeArray(): Texture is not an array or not initialized at all (%s)", m_Specification.c_str());

	if (count == 0)
		return CLog::Log(S_ERROR, "Failed ResizeArray(): count must be at least 1 (was %d, %s)", count, m_Specification.c_str());

	if (count == m_DXTextureDesc.ArraySize)
		return S_SUCCESS;

	// Copy old data into new initial subresource data
	ID3D11Device* pD3DDevice = m_pDXRenderer->GetD3D11Device();
	ID3D11DeviceContext* pD3DDeviceContext = m_pDXRenderer->GetD3D11DeviceContext();

	D3D11_SUBRESOURCE_DATA *data = new D3D11_SUBRESOURCE_DATA[count];
	
	unsigned int bytePerPixel = GetTextureBPP(m_Type);
	unsigned int sliceSz = m_DXTextureDesc.Width * m_DXTextureDesc.Height * bytePerPixel;
	unsigned int sliceRowPitch = m_DXTextureDesc.Width * bytePerPixel;
	unsigned int nCopiedSubresources = min(count, m_DXTextureDesc.ArraySize);
	for (unsigned int i = 0; i < nCopiedSubresources; ++i)
	{
		D3D11_MAPPED_SUBRESOURCE slice;
		pD3DDeviceContext->Map(m_pDXTexture, i, D3D11_MAP_READ, 0, &slice);

		data[i].pSysMem = new char[sliceSz];
		data[i].SysMemPitch = sliceRowPitch;
		for (unsigned int row = 0; row < m_DXTextureDesc.Height; ++row)
		{
			memcpy((char*)data[i].pSysMem + data[i].SysMemPitch * row,
				(const char*)slice.pData + slice.RowPitch * row,
				data[i].SysMemPitch);
		}

		pD3DDeviceContext->Unmap(m_pDXTexture, i);
	}

	// Release old texture
	m_pDXTexture->Release();
	m_pDXTexture = 0;

	if (m_pDXSRV)
	{
		m_pDXSRV->Release();
		m_pDXSRV = 0;
	}

	// Create new array texture
	m_DXTextureDesc.ArraySize = count;
	hr = pD3DDevice->CreateTexture2D(&m_DXTextureDesc, data, &m_pDXTexture);
	
	for (unsigned int i = 0; i < count; ++i)
		delete[] data[i].pSysMem;
	delete[] data;
	
	if (FAILED(hr))
	{
		return CLog::Log(S_ERROR, "Failed ResizeArray(count=%d): Failed create new texture array (%s)", m_Specification.c_str());
	}

	// Create SRV
	m_DXSRVDesc.Texture2DArray.ArraySize = m_DXTextureDesc.ArraySize;
	hr = pD3DDevice->CreateShaderResourceView(m_pDXTexture, &m_DXSRVDesc, &m_pDXSRV);
	if (FAILED(hr))
		return CLog::Log(S_ERROR, "Failed ResizeArray(count=%d): Failed create SRV (%s)", count, m_Specification.c_str());
}

// -----------------------------------------------------------------------------------------------
S_API bool DX11Texture::CheckMipMapAutogenSupported(DXGI_FORMAT format)
{
	if (!m_pDXRenderer)
		return false;
	
	UINT fmtSupport = 0;
	HRESULT hRes = m_pDXRenderer->GetD3D11Device()->CheckFormatSupport(format, &fmtSupport);
	return Success(hRes) && (fmtSupport & D3D11_FORMAT_SUPPORT_MIP_AUTOGEN);
}

// -----------------------------------------------------------------------------------------------
S_API SResult DX11Texture::CreateEmptyIntrnl(unsigned int arraySize, unsigned int w, unsigned int h, unsigned int mipLevels, ETextureType type, SColor clearcolor)
{
	HRESULT hr;

	Clear();

	if (!IS_VALID_PTR(m_pDXRenderer))
		return CLog::Log(S_ERROR, "DX11Texture::CreateEmpty(): Renderer not initialized");

	m_Type = type;
	m_bDynamic = true; // TODO ?
	m_bStaged = true;

	// Now create the directx texture
	m_DXTextureDesc.ArraySize = arraySize;
	m_DXTextureDesc.Width = w;
	m_DXTextureDesc.Height = h;
	m_DXTextureDesc.SampleDesc.Count = 1;
	m_DXTextureDesc.SampleDesc.Quality = 0; // No MS for now!	

	MapTextureType(m_DXTextureDesc.Format, type, false);

	bool bMipAutoGenSupported = CheckMipMapAutogenSupported(m_DXTextureDesc.Format);

	if (!m_bDynamic)
	{
		m_DXTextureDesc.MiscFlags = (bMipAutoGenSupported) ? D3D11_RESOURCE_MISC_GENERATE_MIPS : 0;
		m_DXTextureDesc.BindFlags = (bMipAutoGenSupported) ? (D3D11_BIND_SHADER_RESOURCE | D3D11_BIND_RENDER_TARGET) : (D3D11_BIND_SHADER_RESOURCE);
		m_DXTextureDesc.CPUAccessFlags = 0;
		m_DXTextureDesc.Usage = D3D11_USAGE_DEFAULT;
		m_DXTextureDesc.MipLevels = (bMipAutoGenSupported) ? 0 : 1;
	}
	else
	{
		m_DXTextureDesc.MiscFlags = 0;
		m_DXTextureDesc.BindFlags = D3D11_BIND_SHADER_RESOURCE;
		m_DXTextureDesc.CPUAccessFlags = D3D11_CPU_ACCESS_WRITE;
		m_DXTextureDesc.Usage = D3D11_USAGE_DYNAMIC;
		m_DXTextureDesc.MipLevels = 1;
	}

	// Fill texture with clear color
	unsigned int nPixels = w * h;
	D3D11_SUBRESOURCE_DATA* initData = new D3D11_SUBRESOURCE_DATA[m_DXTextureDesc.ArraySize];
	switch (type)
	{
	case eTEXTURE_R8G8B8A8_UNORM:
		{
			for (unsigned int iSlice = 0; iSlice < m_DXTextureDesc.ArraySize; ++iSlice)
			{
				char* pEmptyPixels = new char[nPixels * 4];
				for (unsigned int iPxl = 0; iPxl < nPixels * 4; iPxl += 4)
				{
					pEmptyPixels[iPxl] = (char)(clearcolor.r * 255.0f);
					pEmptyPixels[iPxl + 1] = (char)(clearcolor.g * 255.0f);
					pEmptyPixels[iPxl + 2] = (char)(clearcolor.b * 255.0f);
					pEmptyPixels[iPxl + 3] = (char)(clearcolor.a * 255.0f);
				}

				initData[iSlice].pSysMem = pEmptyPixels;
				initData[iSlice].SysMemPitch = w * 4;
				initData[iSlice].SysMemSlicePitch = nPixels * 4;
			}

			break;
		}

	case eTEXTURE_D32_FLOAT:
	case eTEXTURE_R32_FLOAT:
		{
			for (unsigned int iSlice = 0; iSlice < m_DXTextureDesc.ArraySize; ++iSlice)
			{
				float* pEmptyPixels = new float[nPixels];
				for (unsigned int iPxl = 0; iPxl < nPixels; ++iPxl)
					pEmptyPixels[iPxl] = clearcolor.r;

				initData[iSlice].pSysMem = pEmptyPixels;
				initData[iSlice].SysMemPitch = w * sizeof(float);
				initData[iSlice].SysMemSlicePitch = nPixels * sizeof(float);
			}

			break;
		}

	default:
		CLog::Log(S_WARNING, "Cannot fill new empty texture: Type not supported (%s)", GetTextureTypeName(type));
		break;
	}

	// Create texture and SRV
	m_pDXTexture = nullptr;

	hr = m_pDXRenderer->GetD3D11Device()->CreateTexture2D(&m_DXTextureDesc, (bMipAutoGenSupported ? nullptr : initData), &m_pDXTexture);

	if (Failure(hr) || m_pDXTexture == nullptr)
		return CLog::Log(S_ERROR, "Failed to create empty DirectX11 Texture (CreateTexture2D failed)!");

#ifdef _DEBUG
	const string& nm = m_Specification;
	m_pDXTexture->SetPrivateData(WKPDID_D3DDebugObjectName, nm.length(), nm.c_str());
#endif

	D3D11_SHADER_RESOURCE_VIEW_DESC srvDesc;
	memset(&srvDesc, 0, sizeof(srvDesc));
	srvDesc.Format = m_DXTextureDesc.Format;
	
	unsigned int numMips = 1;
	if (bMipAutoGenSupported)
		numMips = (mipLevels == 0 ? -1 : mipLevels);
	
	if (m_DXTextureDesc.ArraySize > 1)
	{
		srvDesc.ViewDimension = D3D11_SRV_DIMENSION_TEXTURE2DARRAY;
		srvDesc.Texture2DArray.ArraySize = m_DXTextureDesc.ArraySize;
		srvDesc.Texture2DArray.FirstArraySlice = 0;
		srvDesc.Texture2DArray.MipLevels = numMips;
	}
	else
	{
		srvDesc.ViewDimension = D3D11_SRV_DIMENSION_TEXTURE2D;
		srvDesc.Texture2D.MipLevels = numMips;
	}

	if (Failure(m_pDXRenderer->GetD3D11Device()->CreateShaderResourceView(m_pDXTexture, &srvDesc, &m_pDXSRV)))
	{
		m_pDXTexture->Release();
		return CLog::Log(S_ERROR, "Failed create shader resource view for empty texture!");
	}

	// Generate mips if possible
	ID3D11DeviceContext* pDXDevCon = m_pDXRenderer->GetD3D11DeviceContext();
	if (!m_bDynamic && bMipAutoGenSupported)
	{
		// Is this necessary here??
		for (unsigned int i = 0; i < m_DXTextureDesc.ArraySize; ++i)
			pDXDevCon->UpdateSubresource(m_pDXTexture, i, nullptr, initData[i].pSysMem, initData[i].SysMemPitch, initData[i].SysMemSlicePitch);

		pDXDevCon->GenerateMips(m_pDXSRV);
	}
	else if (m_bDynamic)
	{
		//pDXDevCon->GenerateMips(m_pDXSRV);
	}

	// Store staging data
	// TODO: Staging data for texture array
	m_pStagedData = 0;
	if (m_bStaged && m_DXTextureDesc.ArraySize == 1)
	{
		m_pStagedData = malloc(initData[0].SysMemSlicePitch);
		memcpy(m_pStagedData, initData[0].pSysMem, initData[0].SysMemSlicePitch);
	}

	
	for (unsigned int i = 0; i < m_DXTextureDesc.ArraySize; ++i)
		delete[] initData[i].pSysMem;
	delete[] initData;
	initData = 0;

	return S_SUCCESS;
}

// -----------------------------------------------------------------------------------------------
S_API SResult DX11Texture::CreateEmpty(unsigned int w, unsigned int h, unsigned int mipLevels, ETextureType type, SColor clearcolor)
{	
	return CreateEmptyIntrnl(1, w, h, mipLevels, type, clearcolor);
}

// -----------------------------------------------------------------------------------------------
S_API SResult DX11Texture::CreateEmptyArray(unsigned int count, unsigned int w, unsigned int h,
	ETextureType type, unsigned int mipLevels, SColor clearcolor)
{
	SResult r;
	r = CreateEmptyIntrnl(count, w, h, mipLevels, type, clearcolor);
	if (Failure(r))
		return r;

	m_bArray = true;
	return S_SUCCESS;
}

// -----------------------------------------------------------------------------------------------
S_API SResult DX11Texture::D3D11_InitializeFromExistingResource(ID3D11Texture2D* pResource, DXGI_FORMAT format /*= DXGI_FORMAT_UNKNOWN*/)
{
	if (!pResource)
		return CLog::Log(S_ERROR, "DX11Texture::D3D11_InitializeFromExistingResource(): Invalid resource given");

	Clear();

	m_pDXTexture = pResource;
	m_pDXTexture->AddRef();
	m_pDXTexture->GetDesc(&m_DXTextureDesc);

	MapTextureType(m_DXTextureDesc.Format, m_Type, true);
	m_bDynamic = false;
	m_bStaged = false;

	// Create SRV
	D3D11_SHADER_RESOURCE_VIEW_DESC srvDesc;
	memset(&srvDesc, 0, sizeof(srvDesc));
	srvDesc.Format = (format == DXGI_FORMAT_UNKNOWN ? m_DXTextureDesc.Format : format);
	srvDesc.ViewDimension = D3D11_SRV_DIMENSION_TEXTURE2D;		// maybe someday use MS?
	srvDesc.Texture2D.MipLevels = 1;

	if (Failure(m_pDXRenderer->GetD3D11Device()->CreateShaderResourceView(m_pDXTexture, &srvDesc, &m_pDXSRV)))
		return CLog::Log(S_ERROR, "DX11Texture::D3D11_InitializeFromExistingResource(): Failed create SRV");

	return S_SUCCESS;
}

// -----------------------------------------------------------------------------------------------
S_API SResult DX11Texture::Fill(SColor color)
{
	unsigned int *pPixels, nPixels;
	if (Failure(Lock((void**)&pPixels, &nPixels)))
		return CLog::Log(S_DEBUG, "Failed fill texture: Lock failed");

	for (unsigned int i = 0; i < nPixels; ++i)
	{
		pPixels[i] = color.ToInt_RGBA();
	}

	return Unlock();
}

// -----------------------------------------------------------------------------------------------
S_API SResult DX11Texture::FillArraySlice(unsigned int i, const SColor& color)
{
	HRESULT hr;

	if (!m_pDXTexture)
		return CLog::Log(S_ERROR, "Failed DX11Texture::FillArraySlice(): Texture not initialized (%s)", m_Specification.c_str());

	if (!m_bArray)
		return CLog::Log(S_ERROR, "Failed DX11Texture::FillArraySlice(): Texture not an array texture (%s)", m_Specification.c_str());

	if (i >= m_DXTextureDesc.ArraySize)
		return CLog::Log(S_INVALIDPARAM, "Failed DX11Texture::FillArraySlice(%d): Array only has %d slices (%s)",
			i, m_DXTextureDesc.ArraySize, m_Specification.c_str());

	ID3D11DeviceContext* pD3DDevCtx = m_pDXRenderer->GetD3D11DeviceContext();
	D3D11_MAPPED_SUBRESOURCE mappedSubresource;
	hr = pD3DDevCtx->Map(m_pDXTexture, i, D3D11_MAP_WRITE_DISCARD, 0, &mappedSubresource);
	if (FAILED(hr))
		return CLog::Log(S_ERROR, "Failed DX11Texture::FillArraySlice(): Failed map array slice subresource (%s)", m_Specification.c_str());

	unsigned int bytePerPixel = GetTextureBPP(m_Type);
	char* pRowData = new char[m_DXTextureDesc.Width * bytePerPixel];
	unsigned int clearColorUInt = color.ToInt_RGBA();
	for (unsigned int x = 0; x < m_DXTextureDesc.Width; ++x)
	{
		switch (m_Type)
		{
		case eTEXTURE_R8G8B8A8_UNORM:
			((unsigned int*)pRowData)[x] = clearColorUInt;
			break;
		case eTEXTURE_R32_FLOAT:
		case eTEXTURE_D32_FLOAT:
			((float*)pRowData)[x] = color.r;
			break;
		case eTEXTURE_R16G16_FLOAT:
			float* pFloatRowData = (float*)pRowData;
			pFloatRowData[x * 2] = color.r;
			pFloatRowData[x * 2 + 1] = color.g;
			break;
		default:
			for (unsigned int i = 0; i < bytePerPixel; ++i)
			{
				if (i < 4)
					pRowData[x + i] = ((char*)&clearColorUInt)[i];
				else
					pRowData[x + i] = 0;
			}
			break;
		}
	}

	for (unsigned int y = 0; y < m_DXTextureDesc.Height; ++y)
	{
		memcpy((char*)mappedSubresource.pData + y * mappedSubresource.RowPitch, pRowData, m_DXTextureDesc.Width * bytePerPixel);
	}

	pD3DDevCtx->Unmap(m_pDXTexture, i);
}

// -----------------------------------------------------------------------------------------------
S_API bool DX11Texture::IsInitialized() const
{
	return (m_RefCount > 0 && IS_VALID_PTR(m_pDXTexture) && IS_VALID_PTR(m_pDXSRV));
}

// -----------------------------------------------------------------------------------------------
S_API const string& DX11Texture::GetSpecification(void) const
{
	return m_Specification;
}

// -----------------------------------------------------------------------------------------------
S_API void DX11Texture::SetSpecification(const string& specification)
{
	m_Specification = specification;

#ifdef _DEBUG
	if (m_pDXTexture)
		m_pDXTexture->SetPrivateData(WKPDID_D3DDebugObjectName, m_Specification.length(), m_Specification.c_str());
#endif
}

// -----------------------------------------------------------------------------------------------
S_API ETextureType DX11Texture::GetType(void)
{
	return m_Type;
}

// -----------------------------------------------------------------------------------------------
S_API SResult DX11Texture::GetSize(unsigned int* pW, unsigned int* pH)
{
	if (!IS_VALID_PTR(m_pDXTexture))
		return S_NOTINIT;

	if (IS_VALID_PTR(pW))
		*pW = m_DXTextureDesc.Width;

	if (IS_VALID_PTR(pH))
		*pH = m_DXTextureDesc.Height;

	return S_SUCCESS;
}

// -----------------------------------------------------------------------------------------------
S_API unsigned int DX11Texture::GetArraySize() const
{
	if (!m_bArray || !m_pDXTexture)
		return 0;

	return m_DXTextureDesc.ArraySize;
}

// -----------------------------------------------------------------------------------------------
S_API SResult DX11Texture::Lock(void **pPixels, unsigned int* pnPixels, unsigned int* pnRowPitch /* = 0*/)
{
	if (!IS_VALID_PTR(m_pDXRenderer))
		return CLog::Log(S_ERROR, "DX11Texture::Lock(): Renderer not initialized");

	if (!m_bDynamic)
		return CLog::Log(S_ERROR, "Tried DX11Texture::Lock on non-dynamic texture (%s)", m_Specification.c_str());

	if (m_bLocked)
		return CLog::Log(S_ERROR, "Cannot lock DX11Texture (%s): Already locked!", m_Specification.c_str());

	if (!IS_VALID_PTR(pPixels) || !IS_VALID_PTR(pnPixels))
		return S_INVALIDPARAM;

	if (!IsInitialized())
		return S_NOTINIT;

	ID3D11DeviceContext* pDXDevCon = m_pDXRenderer->GetD3D11DeviceContext();
	if (!IS_VALID_PTR(pDXDevCon))
		return S_NOTINIT;
	
	unsigned int bytePerLockedPixel = GetTextureBPP(m_Type);

	if (m_bStaged)
	{
		if (!IS_VALID_PTR(m_pStagedData))
			return CLog::Log(S_ERROR, "Failed lock staged texture (%s): Staged data invalid (0x%p)!", m_Specification.c_str(), m_pStagedData);

		m_bLocked = true;
		*pPixels = m_pStagedData;
		m_pLockedData = 0;

		if (IS_VALID_PTR(pnRowPitch))
			*pnRowPitch = m_DXTextureDesc.Width * bytePerLockedPixel;
	}
	else
	{
		D3D11_MAPPED_SUBRESOURCE mappedSubresource;
		memset(&mappedSubresource, 0, sizeof(D3D11_MAPPED_SUBRESOURCE));
		if (FAILED(pDXDevCon->Map(m_pDXTexture, 0, D3D11_MAP_WRITE_DISCARD, 0, &mappedSubresource)))
		{
			m_pDXRenderer->FrameDump((string("Failed map texture (") + m_Specification + ") for Lock!").c_str());
			return S_ERROR;
		}

		m_bLocked = true;
		*pPixels = mappedSubresource.pData;
		m_pLockedData = mappedSubresource.pData;

		if (IS_VALID_PTR(pnRowPitch))
			*pnRowPitch = mappedSubresource.RowPitch;		
	}		

	*pnPixels = m_DXTextureDesc.Width * m_DXTextureDesc.Height;	
	m_nLockedBytes = (*pnPixels) * bytePerLockedPixel;

	return S_SUCCESS;
}

// -----------------------------------------------------------------------------------------------
S_API SResult DX11Texture::Unlock()
{
	if (!IS_VALID_PTR(m_pDXRenderer))
		return CLog::Log(S_ERROR, "DX11Texture::Unlock(): Renderer not initialized");

	if (!IsInitialized())
		return S_NOTINIT;

	if (!m_bLocked)
		return CLog::Log(S_WARN, "Tried unlock texture (%s) which is not locked!", m_Specification.c_str());

	assert(m_nLockedBytes > 0);

	if (!m_bStaged && !IS_VALID_PTR(m_pLockedData))
		return S_ERROR;

	ID3D11DeviceContext* pDXDevCon = m_pDXRenderer->GetD3D11DeviceContext();
	if (!IS_VALID_PTR(pDXDevCon))
		return S_NOTINIT;

	m_bLocked = false;

	if (m_bStaged)
	{
		// copy over staged data
		HRESULT hr;
		D3D11_MAPPED_SUBRESOURCE mappedSubresource;
		hr = pDXDevCon->Map(m_pDXTexture, 0, D3D11_MAP_WRITE_DISCARD, 0, &mappedSubresource);
		
		if (FAILED(hr))
			return CLog::Log(S_ERROR, "Failed map texture (%s) for staged update!", m_Specification.c_str());

		unsigned int bytePerLockedPixel = GetTextureBPP(m_Type);

		// We have to copy each line separately to fit DX's pitch
		for (unsigned int ln = 0; ln < m_DXTextureDesc.Height; ++ln)
		{
			memcpy((char*)mappedSubresource.pData + ln * mappedSubresource.RowPitch,
				(const char*)m_pStagedData + ln * m_DXTextureDesc.Width * bytePerLockedPixel,
				m_DXTextureDesc.Width * bytePerLockedPixel);
		}
	}

	pDXDevCon->Unmap(m_pDXTexture, 0);

	m_pLockedData = 0;
	m_nLockedBytes = 0;

	return S_SUCCESS;
}

// -----------------------------------------------------------------------------------------------
S_API SResult DX11Texture::SampleStagedBilinear(Vec2f texcoords, void* pData) const
{
	if (!m_bStaged || !IS_VALID_PTR(m_pStagedData))
		return S_NOTINIT;

	if (!IS_VALID_PTR(pData))
		return S_INVALIDPARAM;

	if (m_Type != eTEXTURE_D32_FLOAT && m_Type != eTEXTURE_R32_FLOAT && m_Type != eTEXTURE_R8G8B8A8_UNORM)
		return S_ERROR;

	Vec2f pixelSizeInTexcoords = 1.0f / Vec2f((float)m_DXTextureDesc.Width, (float)m_DXTextureDesc.Height);

	// Round texcoords to floor
	Vec2f remainder = texcoords % pixelSizeInTexcoords;
	Vec2f roundedTC = texcoords - remainder;

	// transform remainder from [0;pixelSizeInTexcoords] to [0;1]
	remainder /= pixelSizeInTexcoords;		

	// Get all 4 samples
	Vec4f samples[4];
	switch (m_Type)
	{
	case eTEXTURE_D32_FLOAT:
	case eTEXTURE_R32_FLOAT:
	case eTEXTURE_R8G8B8A8_UNORM:
		SampleStaged(roundedTC + Vec2f(-0.5f, -0.5f) * pixelSizeInTexcoords, (void*)&samples[0]);
		SampleStaged(roundedTC + Vec2f( 0.5f, -0.5f) * pixelSizeInTexcoords, (void*)&samples[1]);
		SampleStaged(roundedTC + Vec2f(-0.5f,  0.5f) * pixelSizeInTexcoords, (void*)&samples[2]);		
		SampleStaged(roundedTC + Vec2f( 0.5f,  0.5f) * pixelSizeInTexcoords, (void*)&samples[3]);
		break;
	/*case eTEXTURE_R8G8B8A8_UNORM:
		SampleStaged(roundedTC + Vec2f(-0.5f, -0.5f) * pixelSizeInTexcoords, (void*)&samples[0]);
		SampleStaged(roundedTC + Vec2f( 0.5f, -0.5f) * pixelSizeInTexcoords, (void*)&samples[1]);
		SampleStaged(roundedTC + Vec2f(-0.5f,  0.5f) * pixelSizeInTexcoords, (void*)&samples[2]);
		SampleStaged(roundedTC + Vec2f( 0.5f,  0.5f) * pixelSizeInTexcoords, (void*)&samples[3]);
		break;*/
	default:
		return CLog::Log(S_ERROR, "Cannot SampleStagedBilinear() a texture without floating point format");
	}

	// Interpolate
	Vec4f interpolated[] = 
	{
		Lerp(samples[0], samples[1], remainder.x),
		Lerp(samples[2], samples[3], remainder.x)
	};

	Vec4f out = Lerp(interpolated[0], interpolated[1], remainder.y);

	switch (m_Type)
	{
	case eTEXTURE_D32_FLOAT:
	case eTEXTURE_R32_FLOAT:
		{
			float* pOut = (float*)pData;
			*pOut = out.x;
			break;
		}
	
	case eTEXTURE_R8G8B8A8_UNORM:	
		{
			Vec4f* pOut = (Vec4f*)pData;
			*pOut = out;
			break;
		}

	default:
		return S_ERROR;
	}

	return S_SUCCESS;
}

// -----------------------------------------------------------------------------------------------
S_API SResult DX11Texture::SampleStaged(const Vec2f& texcoords, void* pData) const
{
	if (!m_bStaged || !IS_VALID_PTR(m_pStagedData))
		return S_NOTINIT;

	if (!IS_VALID_PTR(pData))
		return S_INVALIDPARAM;

	// convert texture coordinates to pixel coordinates
	unsigned int pixelCoords[2];
	pixelCoords[0] = (unsigned int)(texcoords.x * (float)m_DXTextureDesc.Width);
	pixelCoords[1] = (unsigned int)(texcoords.y * (float)m_DXTextureDesc.Height);

	// Apply wrap address mode
	pixelCoords[0] = pixelCoords[0] % m_DXTextureDesc.Width;
	pixelCoords[1] = pixelCoords[1] % m_DXTextureDesc.Height;

	// sample data
	unsigned int bpp;
	switch (m_Type)
	{
	case eTEXTURE_R8G8B8A8_UNORM: bpp = 4; break;
	case eTEXTURE_R32_FLOAT:
	case eTEXTURE_D32_FLOAT:
		bpp = sizeof(float);
	}

	unsigned int bytePos = (pixelCoords[1] * m_DXTextureDesc.Width + pixelCoords[0]) * bpp;
	memcpy(pData, static_cast<char*>(m_pStagedData) + bytePos, bpp);
	return S_SUCCESS;
}

// -----------------------------------------------------------------------------------------------
S_API void* DX11Texture::GetStagedData()
{
	return m_pStagedData;
}

// -----------------------------------------------------------------------------------------------
S_API void DX11Texture::Clear(void)
{
	if (IS_VALID_PTR(m_pStagedData))
		free(m_pStagedData);

	m_pStagedData = 0;

	SP_SAFE_RELEASE(m_pDXSRV);
	SP_SAFE_RELEASE(m_pDXTexture);

	m_pLockedData = 0;
	m_nLockedBytes = 0;
	m_bLocked = false;

	m_Type = eTEXTURE_R8G8B8A8_UNORM;
	m_bIsCubemap = false;
	m_bArray = false;
}




SP_NMSPACE_END
