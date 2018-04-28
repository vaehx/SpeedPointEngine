#include "ImageLoader.h"

SP_NMSPACE_BEG

S_API const char* GetWICPixelFormatName(WICPixelFormatGUID fmt)
{
	if (fmt == GUID_WICPixelFormatDontCare) return "GUID_WICPixelFormatDontCare";
	else if (fmt == GUID_WICPixelFormatUndefined) return "GUID_WICPixelFormatUndefined";
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



S_API SResult CImageLoader::Load(const string& file, SLoadedImage* pImage, unsigned int scaleToWidth, unsigned int scaleToHeight, WICPixelFormatGUID convertToFmt)
{
	HRESULT hRes;

	if (!pImage)
		return S_INVALIDPARAM;

	SLoadedImage& bitmap = *pImage;

	IWICImagingFactory* pImgFactory;
	hRes = CoCreateInstance(CLSID_WICImagingFactory, 0, CLSCTX_INPROC_SERVER, IID_IWICImagingFactory, (void**)&pImgFactory);
	if (Failure(hRes))
	{
		return CLog::Log(S_ERROR, "Failed create WIC Imaging Factory!");
	}

	IWICBitmapDecoder* pImgDecoder;
	wchar_t* cWFilename = new wchar_t[file.length() + 1];
	size_t nNumCharsConv;
	mbstowcs_s(&nNumCharsConv, cWFilename, file.length() + 1, file.c_str(), _TRUNCATE);
	hRes = pImgFactory->CreateDecoderFromFilename(cWFilename, 0, GENERIC_READ, WICDecodeMetadataCacheOnDemand, &pImgDecoder);
	if (Failure(hRes))
	{
		_com_error err(hRes);
		CLog::Log(S_DEBUG, "Failed create WIC Image decoder for %s: %s!", file.c_str(), err.ErrorMessage());
		return S_ERROR;
	}


	//
	// TODO:
	//	for animated images, loop through all frames and call pImgFactory->GetFrameCount() first.
	//



	// Decode frame
	IWICBitmapFrameDecode* pBmpFrameDecode;
	pImgDecoder->GetFrame(0, &pBmpFrameDecode);


	// Get pixel format from loaded bitmap frame and target format
	WICPixelFormatGUID srcFmtWIC;
	if (Failure(pBmpFrameDecode->GetPixelFormat(&srcFmtWIC)))
	{
		return CLog::Log(S_ERROR, "Failed get pixel format of desired frame!");
	}

	WICPixelFormatGUID fmtUndefined = GUID_WICPixelFormatUndefined;
	WICPixelFormatGUID dstFmtWIC = convertToFmt;
	if (memcmp(&dstFmtWIC, &fmtUndefined, sizeof(GUID)) == 0)
		dstFmtWIC = srcFmtWIC;

	// ----------------------------------------------------------------------------------------------------------------------
	// Create temporary buffer

	UINT nLoadedWidth, nLoadedHeight;
	pBmpFrameDecode->GetSize(&nLoadedWidth, &nLoadedHeight);

	if (scaleToWidth == 0 || scaleToHeight == 0)
	{
		bitmap.width = nLoadedWidth;
		bitmap.height = nLoadedHeight;
	}
	else
	{
		bitmap.width = scaleToWidth;
		bitmap.height = scaleToHeight;
	}

	UINT nBPP = BitsPerPixel(dstFmtWIC, pImgFactory);
	if (nBPP == 0)
	{
		return CLog::Log(S_ERROR, "DX11Texture::LoadTextureImage(): Could not retrieve bits per pixel for loaded texture!");
	}

	bitmap.imageStride = (bitmap.width * nBPP + 7) / 8;
	bitmap.imageSize = bitmap.imageStride * bitmap.height;
	bitmap.bytePerPixel = (unsigned int)(nBPP / 8);

	bitmap.buffer = new uint8_t[bitmap.imageSize];

	// ----------------------------------------------------------------------------------------------------------------------
	// Scale or convert the image to the desired format

	bool needConversion = false;
	IWICBitmapSource* pConversionSrc = 0;
	IWICBitmapScaler* pScaler = 0;

	if (nLoadedWidth == bitmap.width && nLoadedHeight == bitmap.height && memcmp(&dstFmtWIC, &srcFmtWIC, sizeof(GUID)) == 0)
	{
		// We can directly copy pixels without modification 
		hRes = pBmpFrameDecode->CopyPixels(0, static_cast<UINT>(bitmap.imageStride), static_cast<UINT>(bitmap.imageSize), bitmap.buffer);
		if (Failure(hRes))
			return CLog::Log(S_ERROR, "Failed to buffer texture!");

		needConversion = false;
	}
	else if (nLoadedWidth != bitmap.width || nLoadedHeight != bitmap.height)
	{
		// Create scaler
		if (Failure(pImgFactory->CreateBitmapScaler(&pScaler)))
		{
			CLog::Log(S_DEBUG, "Failed pImgFactory->CreateBitmapScalar()");
			return S_ERROR;
		}

		if (Failure(pScaler->Initialize(pBmpFrameDecode, bitmap.width, bitmap.height,
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

		if (memcmp(&dstFmtWIC, &pfScaler, sizeof(GUID)) == 0)
		{
			// Scale directly
			hRes = pScaler->CopyPixels(0, static_cast<UINT>(bitmap.imageStride), static_cast<UINT>(bitmap.imageSize), bitmap.buffer);
			if (Failure(hRes))
				return CLog::Log(S_ERROR, "Failed to buffer scaled texture while loading %s!", file.c_str());
		}
		else
		{
			// Convert with the scaler output as source bitmap
			needConversion = true;
			pConversionSrc = pScaler;
		}
	}
	else
	{
		needConversion = true;
		pConversionSrc = pBmpFrameDecode;
	}


	if (needConversion)
	{
		ScopedObject<IWICFormatConverter> formatConverter;
		if (Failure(pImgFactory->CreateFormatConverter(&formatConverter)))
			return CLog::Log(S_ERROR, "DX11Texture::LoadTextureImage(): Failed to create pixel format convert!");

		hRes = formatConverter->Initialize(pConversionSrc, dstFmtWIC, WICBitmapDitherTypeErrorDiffusion, 0, 0, WICBitmapPaletteTypeCustom);
		if (Failure(hRes))
			return CLog::Log(S_ERROR, "DX11Texture::LoadTextureImage(): Failed to initialize format converter (%s -> %s) for file '%s'!",
				GetWICPixelFormatName(srcFmtWIC), GetWICPixelFormatName(dstFmtWIC), file.c_str());

		BOOL bCanConvert;
		hRes = formatConverter->CanConvert(srcFmtWIC, dstFmtWIC, &bCanConvert);
		if (Failure(hRes))
			return CLog::Log(S_ERROR, "DX11Texture::LoadTextureImage(): Could not determine if format %s is convertable to %s",
				GetWICPixelFormatName(srcFmtWIC), GetWICPixelFormatName(dstFmtWIC));

		if (!bCanConvert)
			return CLog::Log(S_ERROR, "DX11Texture::LoadTextureImage(): Format %s cannot be converted to %s!",
				GetWICPixelFormatName(srcFmtWIC), GetWICPixelFormatName(dstFmtWIC));

		hRes = formatConverter->CopyPixels(0, static_cast<UINT>(bitmap.imageStride), static_cast<UINT>(bitmap.imageSize), bitmap.buffer);
		if (Failure(hRes))
			return CLog::Log(S_ERROR, "DX11Texture::LoadTextureImage(): Failed to convert and copy pixels for file %s!", file.c_str());
	}

	bitmap.pixelFormat = dstFmtWIC;

	SP_SAFE_RELEASE(pScaler);
	SP_SAFE_RELEASE(pImgDecoder);
	SP_SAFE_RELEASE(pImgFactory);

	return S_SUCCESS;
}

S_API size_t CImageLoader::BitsPerPixel(REFGUID targetGuid, IWICImagingFactory* pWIC)
{
	if (!pWIC)
		return 0;

	ScopedObject<IWICComponentInfo> comInfo;
	if (Failure(pWIC->CreateComponentInfo(targetGuid, &comInfo)))
		return 0;

	// make sure we got the correct component type
	WICComponentType comType;
	if (Failure(comInfo->GetComponentType(&comType)))
		return 0;
	if (comType != WICPixelFormat)
		return 0;

	ScopedObject<IWICPixelFormatInfo> pxlFmtInfo;
	if (Failure(comInfo->QueryInterface(__uuidof(IWICPixelFormatInfo), reinterpret_cast<void**>(&pxlFmtInfo))))
		return 0;

	UINT bpp;
	if (Failure(pxlFmtInfo->GetBitsPerPixel(&bpp)))
		return 0;

	return bpp;
}

SP_NMSPACE_END
