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
m_pDXTexture(0)
{
}

// -----------------------------------------------------------------------------------
DirectX11Texture::DirectX11Texture(const DirectX11Texture& o)
: m_pEngine(o.m_pEngine),
m_Type(o.m_Type),
m_bDynamic(o.m_bDynamic)
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

	// and get the bpp out of the pxl fmt guid


	// copy the pixels into a temporary buffer	
	UINT nLoadedWidth, nLoadedHeight;
	pBmpFrameDecode->GetSize(&nLoadedWidth, &nLoadedHeight);

	UINT nBPP = BitsPerPixel(pxlFmtGUID, pImgFactory);
	if (nBPP == 0)
		return m_pEngine->LogE("Could not retrieve bits per pixel for loaded texture!");

	size_t imageStride = (w * nBPP + 7) / 8;
	size_t imageSize = imageStride * nLoadedHeight;

	std::unique_ptr<uint8_t[]> temp(new uint8_t[imageSize]);

	// check whether we need to scale the loaded image
	if (nLoadedWidth == w && nLoadedHeight == h)
	{
		hRes = pBmpFrameDecode->CopyPixels(0, static_cast<UINT>(imageStride), static_cast<UINT>(imageSize), temp.get());
		if (Failure(hRes))
			return m_pEngine->LogE("Failed to buffer texture!");
	}
	else
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

		hRes = pScaler->CopyPixels(0, static_cast<UINT>(imageStride), static_cast<UINT>(imageSize), temp.get());
		if (Failure(hRes))
			return m_pEngine->LogE("Failed to buffer scaled texture!");
	}		


	// Check if autogeneration of mip levels is supported 	
	UINT fmtSupport = 0;
	hRes = m_pDXRenderer->GetD3D11Device()->CheckFormatSupport(loadedTextureFmt, &fmtSupport);
	bool bMipAutoGenSupported = Success(hRes) && (fmtSupport & D3D11_FORMAT_SUPPORT_MIP_AUTOGEN);


	// Now create the directx texture
	D3D11_TEXTURE2D_DESC textureDesc;
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
S_API SResult DirectX11Texture::CreateEmpty(int w, int h, int mipLevels, S_TEXTURE_TYPE type, SColor clearcolor)
{


	// TODO!

	return S_SUCCESS;

}

// -----------------------------------------------------------------------------------
S_API SString DirectX11Texture::GetSpecification(void)
{
	return m_Specification;
}

// -----------------------------------------------------------------------------------
S_API S_TEXTURE_TYPE DirectX11Texture::GetType(void)
{
	return m_Type;
}

// -----------------------------------------------------------------------------------
S_API SResult DirectX11Texture::GetSize(int* pW, int* pH)
{



	// TODO





	return S_SUCCESS;
}

// -----------------------------------------------------------------------------------
S_API SResult DirectX11Texture::Clear(void)
{
	m_pEngine = 0;
	m_pDXRenderer = 0;

	if (m_pDXSRV)
	{
		m_pDXSRV->Release();
		m_pDXSRV = nullptr;
	}

	if (m_pDXTexture)
	{
		m_pDXTexture->Release();
		m_pDXTexture = nullptr;
	}

	return S_SUCCESS;
}




// -----------------------------------------------------------------------------------
SP_NMSPACE_END