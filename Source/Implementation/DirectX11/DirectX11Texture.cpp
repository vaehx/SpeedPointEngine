//////////////////////////////////////////////////////////////////////////////////////////////////
//
//	This file is part of the SpeedPoint Game Engine
//
//	written by Pascal R. aka iSmokiieZz
//	(c) 2011-2014, All rights reserved.
//
//////////////////////////////////////////////////////////////////////////////////////////////////

#include <Implementation\DirectX11\DirectX11Texture.h>
#include <Implementation\DirectX11\DirectX11Renderer.h>
#include <Implementation\DirectX11\DirectX11.h>
#include <SpeedPointEngine.h>
#include <Abstract\IRenderer.h>

#include <d2d1.h>
#include <wincodec.h>
#include <memory>

SP_NMSPACE_BEG

// -----------------------------------------------------------------------------------
DirectX11Texture::DirectX11Texture()
: m_pEngine(0),
m_bDynamic(false),
m_pDXTexture(0),
m_pDXRenderer(nullptr),
m_pDXSRV(nullptr),
m_pLockedData(nullptr),
m_nLockedBytes(0)
{
}

// -----------------------------------------------------------------------------------
DirectX11Texture::DirectX11Texture(const DirectX11Texture& o)
: m_pEngine(o.m_pEngine),
m_Type(o.m_Type),
m_bDynamic(o.m_bDynamic),
m_pLockedData(nullptr),
m_nLockedBytes(0)
{
}

// -----------------------------------------------------------------------------------
DirectX11Texture::~DirectX11Texture()
{
	Clear();
}

// -----------------------------------------------------------------------------------
S_API SResult DirectX11Texture::Initialize(IGameEngine* pEngine, const SString& spec)
{	
	return Initialize(pEngine, spec, false);
}

// -----------------------------------------------------------------------------------
S_API SResult DirectX11Texture::Initialize(IGameEngine* pEngine, const SString& spec, bool bDynamic)
{
	SP_ASSERTR(pEngine, S_INVALIDPARAM);

	m_pEngine = pEngine;

	IRenderer* pRenderer = pEngine->GetRenderer();
	SP_ASSERTR(pRenderer->GetType() == S_DIRECTX11, S_INVALIDPARAM);

	m_pDXRenderer = (DirectX11Renderer*)pRenderer;
	m_Specification = spec;
	m_bDynamic = bDynamic;

	return S_SUCCESS;
}

// -----------------------------------------------------------------------------------
// remember that the w and h parameters will specify the output texture size to which the image will be scaled to
S_API SResult DirectX11Texture::LoadFromFile(int w, int h, int mipLevels, char* cFileName)
{	
	SP_ASSERTR(m_pEngine && m_pDXRenderer, S_NOTINIT);


	HRESULT hRes;

	IWICImagingFactory* pImgFactory;
	hRes = CoCreateInstance(CLSID_WICImagingFactory, 0, CLSCTX_INPROC_SERVER, IID_IWICImagingFactory, (void**)&pImgFactory);
	if (Failure(hRes))
		return m_pEngine->LogE("Failed Create WIC Imaging Factory!");

	IWICBitmapDecoder* pImgDecoder;	
	wchar_t* cWFilename = new wchar_t[50];
	size_t nNumCharsConv;	
	unsigned short nWordSize = 60 / sizeof(unsigned short);
	mbstowcs_s(&nNumCharsConv, cWFilename, nWordSize, cFileName, _TRUNCATE);
	hRes = pImgFactory->CreateDecoderFromFilename(cWFilename, 0, GENERIC_READ, WICDecodeMetadataCacheOnDemand, &pImgDecoder);
	if (Failure(hRes))
		return m_pEngine->LogE("Failed Create WIC Image decoder!");



	//
	// TODO:
	//	for animated texture, loop through all frames and call pImgFactory->GetFrameCount() first.
	//

	IWICBitmapFrameDecode* pBmpFrameDecode;
	pImgDecoder->GetFrame(0, &pBmpFrameDecode);

	// get the pixel size of this frame
	WICPixelFormatGUID pxlFmtGUID;	
	if (Failure(pBmpFrameDecode->GetPixelFormat(&pxlFmtGUID)))
		return m_pEngine->LogE("Failed Get Pixel Format of desired frame!");

	// some pixel format cannot be directly translated into a DXGI Format. So find a nearest match.	
	WICPixelFormatGUID pxlFmtGUIDOrig = pxlFmtGUID;
	if (pxlFmtGUID == GUID_WICPixelFormat24bppBGR) pxlFmtGUID = GUID_WICPixelFormat32bppRGBA;
	else if (pxlFmtGUID == GUID_WICPixelFormat8bppIndexed) pxlFmtGUID = GUID_WICPixelFormat32bppRGBA;

	// we'll probably need a dxgi format to create the dx texture instance	
	DXGI_FORMAT loadedTextureFmt;	
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
		return m_pEngine->LogE("Unsupported pixel fmt of texture: Unkown format!");		
		// m_pEngine->LogE << "Failed convert pixel fmt: Unknown format: " << pxlFmtGUID << "!";
	}	


	// copy the pixels into a temporary buffer	
	UINT nLoadedWidth, nLoadedHeight;
	pBmpFrameDecode->GetSize(&nLoadedWidth, &nLoadedHeight);

	UINT nBPP = BitsPerPixel(pxlFmtGUID, pImgFactory);
	if (nBPP == 0)
		return m_pEngine->LogE("Could not retrieve bits per pixel for loaded texture!");

	size_t imageStride = (w * nBPP + 7) / 8;
	size_t imageSize = imageStride * nLoadedHeight;

	std::unique_ptr<uint8_t[]> temp(new uint8_t[imageSize]);

	// check whether we need to scale or convert the loaded image
	if (nLoadedWidth == w && nLoadedHeight == h && memcmp(&pxlFmtGUID, &pxlFmtGUIDOrig, sizeof(GUID)) == 0)
	{
		hRes = pBmpFrameDecode->CopyPixels(0, static_cast<UINT>(imageStride), static_cast<UINT>(imageSize), temp.get());
		if (Failure(hRes))
			return m_pEngine->LogE("Failed to buffer texture!");
	}
	else if (nLoadedWidth != w && nLoadedHeight != h)
	{
		ScopedTextureLoadingObject<IWICBitmapScaler> pScaler;
		if (Failure(pImgFactory->CreateBitmapScaler(&pScaler)))
			return S_ERROR;

		if (Failure(pScaler->Initialize(pBmpFrameDecode, w, h,
			WICBitmapInterpolationModeFant))) // Maybe change this someday??
		{
			return S_ERROR;
		}
		
		WICPixelFormatGUID pfScaler;
		if (Failure(pScaler->GetPixelFormat(&pfScaler)))
			return S_ERROR;

		if (memcmp(&pxlFmtGUID, &pfScaler, sizeof(GUID)) == 0)
		{
			hRes = pScaler->CopyPixels(0, static_cast<UINT>(imageStride), static_cast<UINT>(imageSize), temp.get());
			if (Failure(hRes))
				return m_pEngine->LogE("Failed to buffer scaled texture!");
		}
		else
		{
			ScopedTextureLoadingObject<IWICFormatConverter> formatConverter;
			if (Failure(pImgFactory->CreateFormatConverter(&formatConverter)))
				return m_pEngine->LogE("Failed to create pixel format convert (1)!");

			hRes = formatConverter->Initialize(pScaler.Get(), pxlFmtGUID, WICBitmapDitherTypeErrorDiffusion, 0, 0, WICBitmapPaletteTypeCustom);
			if (Failure(hRes))
				return m_pEngine->LogE("Failed to initialize format converter (1)!");

			hRes = formatConverter->CopyPixels(0, static_cast<UINT>(imageStride), static_cast<UINT>(imageSize), temp.get());
			if (Failure(hRes))
				return m_pEngine->LogE("Failed to convert and copy pixels (1)!");
		}
	}	
	else // convert only
	{
		ScopedTextureLoadingObject<IWICFormatConverter> formatConverter;
		if (Failure(pImgFactory->CreateFormatConverter(&formatConverter)))
			return m_pEngine->LogE("Failed to create pixel format convert (2)!");

		hRes = formatConverter->Initialize(pBmpFrameDecode, pxlFmtGUID, WICBitmapDitherTypeErrorDiffusion, 0, 0, WICBitmapPaletteTypeCustom);
		if (Failure(hRes))
			return m_pEngine->LogE("Failed to initialize format converter (2)!");

		hRes = formatConverter->CopyPixels(0, static_cast<UINT>(imageStride), static_cast<UINT>(imageSize), temp.get());
		if (Failure(hRes))
			return m_pEngine->LogE("Failed to convert and copy pixels (2)!");
	}


	// Check if autogeneration of mip levels is supported 	
	UINT fmtSupport = 0;
	hRes = m_pDXRenderer->GetD3D11Device()->CheckFormatSupport(loadedTextureFmt, &fmtSupport);
	bool bMipAutoGenSupported = Success(hRes) && (fmtSupport & D3D11_FORMAT_SUPPORT_MIP_AUTOGEN);


	SP_SAFE_RELEASE(pImgDecoder);
	SP_SAFE_RELEASE(pImgFactory);

	m_pEngine->LogD(SString("Loaded Texture ") + cFileName + "!");


	// Now create the directx texture
	D3D11_TEXTURE2D_DESC& textureDesc = m_DXTextureDesc;
	textureDesc.ArraySize = 1;
	textureDesc.BindFlags = (bMipAutoGenSupported) ? (D3D11_BIND_SHADER_RESOURCE | D3D11_BIND_RENDER_TARGET) : (D3D11_BIND_SHADER_RESOURCE);
	textureDesc.CPUAccessFlags = (m_bDynamic) ? D3D11_CPU_ACCESS_WRITE /* | D3D11_CPU_ACCESS_READ */ : 0;
	textureDesc.Format = loadedTextureFmt;
	textureDesc.Width = w;
	textureDesc.Height = h;
	textureDesc.MipLevels = (bMipAutoGenSupported) ? 0 : 1;
	textureDesc.MiscFlags = (bMipAutoGenSupported) ? D3D11_RESOURCE_MISC_GENERATE_MIPS : 0;		
	textureDesc.SampleDesc.Count = 1;
	textureDesc.SampleDesc.Quality = 0; // No MS for now!
	textureDesc.Usage = (m_bDynamic) ? D3D11_USAGE_DYNAMIC : D3D11_USAGE_DEFAULT;
	
	D3D11_SUBRESOURCE_DATA initData;
	initData.pSysMem = temp.get();
	initData.SysMemPitch = static_cast<UINT>(imageStride);
	initData.SysMemSlicePitch = static_cast<UINT>(imageSize);

	m_pDXTexture = nullptr;
	hRes = m_pDXRenderer->GetD3D11Device()->CreateTexture2D(&textureDesc, (bMipAutoGenSupported) ? nullptr : &initData, &m_pDXTexture);
	if (Failure(hRes) || m_pDXTexture == nullptr)
		return m_pEngine->LogE("Failed to create DirectX11 Texture!");
	
	D3D11_SHADER_RESOURCE_VIEW_DESC srvDesc;
	memset(&srvDesc, 0, sizeof(srvDesc));
	srvDesc.Format = loadedTextureFmt;
	srvDesc.ViewDimension = D3D11_SRV_DIMENSION_TEXTURE2D;
	srvDesc.Texture2D.MipLevels = (bMipAutoGenSupported) ? -1 : 1;
	if (Failure(m_pDXRenderer->GetD3D11Device()->CreateShaderResourceView(m_pDXTexture, &srvDesc, &m_pDXSRV)))
	{
		m_pDXTexture->Release();
		return m_pEngine->LogE("Failed create shader resource view for texture!");
	}

	if (bMipAutoGenSupported)
	{
		ID3D11DeviceContext* pDXDevCon = m_pDXRenderer->GetD3D11DeviceContext();
		pDXDevCon->UpdateSubresource(m_pDXTexture, 0, nullptr, temp.get(), static_cast<UINT>(imageStride), static_cast<UINT>(imageSize));
		pDXDevCon->GenerateMips(m_pDXSRV);
	}

	m_pEngine->LogD("Creating new texture from file succeeded!");

	return S_SUCCESS;
}

// -----------------------------------------------------------------------------------
size_t DirectX11Texture::BitsPerPixel(REFGUID targetGuid, IWICImagingFactory* pWIC)
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

// -----------------------------------------------------------------------------------
S_API SResult DirectX11Texture::CreateEmpty(int w, int h, int mipLevels, ETextureType type, SColor clearcolor)
{	
	m_Type = type;
	DXGI_FORMAT newTextureFmt;
	switch (type)
	{	
	case eTEXTURE_D32_FLOAT:
		newTextureFmt = DXGI_FORMAT_D32_FLOAT; break;
	case eTEXTURE_R8G8B8A8_UNORM:
	default:	
		newTextureFmt = DXGI_FORMAT_R8G8B8A8_UNORM; break;
	}	

	UINT fmtSupport = 0;
	HRESULT hRes = m_pDXRenderer->GetD3D11Device()->CheckFormatSupport(newTextureFmt, &fmtSupport);
	bool bMipAutoGenSupported = Success(hRes) && (fmtSupport & D3D11_FORMAT_SUPPORT_MIP_AUTOGEN);

	// Now create the directx texture
	D3D11_TEXTURE2D_DESC& textureDesc = m_DXTextureDesc;
	textureDesc.ArraySize = 1;
	textureDesc.BindFlags = (bMipAutoGenSupported) ? (D3D11_BIND_SHADER_RESOURCE | D3D11_BIND_RENDER_TARGET) : (D3D11_BIND_SHADER_RESOURCE);
	textureDesc.CPUAccessFlags = (m_bDynamic) ? D3D11_CPU_ACCESS_WRITE /* | D3D11_CPU_ACCESS_READ */ : 0;
	textureDesc.Format = newTextureFmt;
	textureDesc.Width = w;
	textureDesc.Height = h;
	textureDesc.MipLevels = (bMipAutoGenSupported) ? 0 : 1;
	textureDesc.MiscFlags = (bMipAutoGenSupported) ? D3D11_RESOURCE_MISC_GENERATE_MIPS : 0;
	textureDesc.SampleDesc.Count = 1;
	textureDesc.SampleDesc.Quality = 0; // No MS for now!
	textureDesc.Usage = (m_bDynamic) ? D3D11_USAGE_DYNAMIC : D3D11_USAGE_DEFAULT;

	unsigned int nPixels = w * h;

	D3D11_SUBRESOURCE_DATA initData;		
	switch (type)
	{
	case eTEXTURE_R8G8B8A8_UNORM:
	{
		char* pEmptyPixels = new char[nPixels * 4];
		for (unsigned int iPxl = 0; iPxl < nPixels * 4; iPxl += 4)
		{
			pEmptyPixels[iPxl]	= (char)(clearcolor.r * 255.0f);
			pEmptyPixels[iPxl + 1]	= (char)(clearcolor.g * 255.0f);
			pEmptyPixels[iPxl + 2]	= (char)(clearcolor.b * 255.0f);
			pEmptyPixels[iPxl + 3]	= (char)(clearcolor.a * 255.0f);
		}

		initData.pSysMem = pEmptyPixels;
		initData.SysMemPitch = w * 4;
		initData.SysMemSlicePitch = nPixels * 4;

		break;
	}
	case eTEXTURE_D32_FLOAT:
	{
		float* pEmptyPixels = new float[nPixels];
		for (unsigned int iPxl = 0; iPxl < nPixels; ++iPxl)
			pEmptyPixels[iPxl] = clearcolor.r;		

		initData.pSysMem = pEmptyPixels;
		initData.SysMemPitch = w * sizeof(float);
		initData.SysMemSlicePitch = nPixels * sizeof(float);

		break;
	}			
	}	

	m_pDXTexture = nullptr;
	hRes = m_pDXRenderer->GetD3D11Device()->CreateTexture2D(&textureDesc, (bMipAutoGenSupported) ? nullptr : &initData, &m_pDXTexture);
	if (Failure(hRes) || m_pDXTexture == nullptr)
		return m_pEngine->LogE("Failed to create empty DirectX11 Texture (CreateTexture2D failed)!");

	D3D11_SHADER_RESOURCE_VIEW_DESC srvDesc;
	memset(&srvDesc, 0, sizeof(srvDesc));
	srvDesc.Format = newTextureFmt;
	srvDesc.ViewDimension = D3D11_SRV_DIMENSION_TEXTURE2D;
	srvDesc.Texture2D.MipLevels = (bMipAutoGenSupported) ? -1 : 1;
	if (Failure(m_pDXRenderer->GetD3D11Device()->CreateShaderResourceView(m_pDXTexture, &srvDesc, &m_pDXSRV)))
	{
		m_pDXTexture->Release();
		return m_pEngine->LogE("Failed create shader resource view for empty texture!");
	}

	if (bMipAutoGenSupported)
	{
		ID3D11DeviceContext* pDXDevCon = m_pDXRenderer->GetD3D11DeviceContext();
		pDXDevCon->UpdateSubresource(m_pDXTexture, 0, nullptr, initData.pSysMem, initData.SysMemPitch, initData.SysMemSlicePitch);
		pDXDevCon->GenerateMips(m_pDXSRV);
	}

	delete[] initData.pSysMem;

	m_pEngine->LogD("Creating new empty texture succeeded!");

	return S_SUCCESS;
}

// -----------------------------------------------------------------------------------
S_API SString DirectX11Texture::GetSpecification(void)
{
	return m_Specification;
}

// -----------------------------------------------------------------------------------
S_API ETextureType DirectX11Texture::GetType(void)
{
	return m_Type;
}

// -----------------------------------------------------------------------------------
S_API SResult DirectX11Texture::GetSize(unsigned int* pW, unsigned int* pH)
{
	if (!IS_VALID_PTR(m_pDXTexture))
		return S_NOTINIT;

	if (IS_VALID_PTR(pW))
		*pW = m_DXTextureDesc.Width;

	if (IS_VALID_PTR(pH))
		*pH = m_DXTextureDesc.Height;

	return S_SUCCESS;
}

// -----------------------------------------------------------------------------------
S_API SResult DirectX11Texture::Lock(void **pPixels, unsigned int* pnPixels)
{
	if (!IS_VALID_PTR(pPixels) || !IS_VALID_PTR(pnPixels))
		return S_INVALIDPARAM;

	if (!IS_VALID_PTR(m_pDXTexture) || !IS_VALID_PTR(m_pDXRenderer))
		return S_NOTINIT;

	ID3D11DeviceContext* pDXDevCon = m_pDXRenderer->GetD3D11DeviceContext();
	if (!IS_VALID_PTR(pDXDevCon))
		return S_NOTINIT;

	D3D11_MAPPED_SUBRESOURCE mappedSubresource;
	memset(&mappedSubresource, 0, sizeof(D3D11_MAPPED_SUBRESOURCE));
	if (FAILED(pDXDevCon->Map(m_pDXTexture, 0, D3D11_MAP_READ_WRITE, 0, &mappedSubresource)))
	{
		m_pDXRenderer->FrameDump("Failed map texture (" + m_Specification + ") for Lock!");
		return S_ERROR;
	}

	*pPixels = mappedSubresource.pData;
	m_pLockedData = mappedSubresource.pData;
	
	*pnPixels = m_DXTextureDesc.Width * m_DXTextureDesc.Height;
	switch (m_Type)
	{
	case eTEXTURE_R8G8B8A8_UNORM:
		m_nLockedBytes = (*pnPixels) * 4;
		break;
	case eTEXTURE_D32_FLOAT:
		m_nLockedBytes = (*pnPixels) * sizeof(float);
		break;
	}

	return S_SUCCESS;
}

// -----------------------------------------------------------------------------------
S_API SResult DirectX11Texture::Unlock()
{
	if (!IS_VALID_PTR(m_pLockedData))
		return S_ERROR;

	if (!IS_VALID_PTR(m_pDXRenderer) || !IS_VALID_PTR(m_pDXTexture))
		return S_NOTINIT;

	ID3D11DeviceContext* pDXDevCon = m_pDXRenderer->GetD3D11DeviceContext();
	if (!IS_VALID_PTR(pDXDevCon))
		return S_NOTINIT;

	pDXDevCon->Unmap(m_pDXTexture, 0);

	m_pLockedData = 0;
	m_nLockedBytes = 0;

	return S_SUCCESS;
}

// -----------------------------------------------------------------------------------
S_API SResult DirectX11Texture::Clear(void)
{
	m_pEngine = 0;
	m_pDXRenderer = 0;

	SP_SAFE_RELEASE(m_pDXSRV);
	SP_SAFE_RELEASE(m_pDXTexture);

	m_pLockedData = 0;
	m_nLockedBytes = 0;

	return S_SUCCESS;
}




// -----------------------------------------------------------------------------------
SP_NMSPACE_END