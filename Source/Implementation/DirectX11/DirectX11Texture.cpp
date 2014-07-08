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
#include <SSpeedPointEngine.h>
#include <Abstract\IRenderer.h>

#include <wincodec.h>

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
S_API SResult DirectX11Texture::Initialize(SpeedPointEngine* pEngine, const SString& spec)
{	
	return Initialize(pEngine, spec, false);
}

// -----------------------------------------------------------------------------------
S_API SResult DirectX11Texture::Initialize(SpeedPointEngine* pEngine, const SString& spec, bool bDynamic)
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
S_API SResult DirectX11Texture::LoadFromFile(int w, int h, int mipLevels, char* cFileName)
{	
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

	// TODO: for animated texture, loop through all frames and call pImgFactory->GetFrameCount() first.

	IWICBitmapFrameDecode* pBmpFrameDecode;
	pImgDecoder->GetFrame(0, )


	// TODO!

	return S_SUCCESS;
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

	return S_SUCCESS;
}




// -----------------------------------------------------------------------------------
SP_NMSPACE_END