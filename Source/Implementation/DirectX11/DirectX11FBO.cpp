//////////////////////////////////////////////////////////////////////////////////////////////////
//
//	This file is part of the SpeedPoint Game Engine
//
//	written by Pascal R. aka iSmokiieZz
//	(c) 2011-2014, All rights reserved.
//
//////////////////////////////////////////////////////////////////////////////////////////////////

#include <Implementation\DirectX11\DirectX11FBO.h>
#include <Implementation\DirectX11\DirectX11Renderer.h>
#include <Abstract\IRenderer.h>
#include <SSpeedPointEngine.h>
#include <Implementation\DirectX11\DirectX11Utilities.h>

SP_NMSPACE_BEG

// -----------------------------------------------------------------------
S_API DirectX11FBO::DirectX11FBO()
	: m_pDXRenderer(0),
	m_pEngine(0),
	m_pTexture(0),
	m_pRTV(0),
	m_nBufferWidth(0),
	m_nBufferHeight(0)
{
}

// -----------------------------------------------------------------------
S_API DirectX11FBO::~DirectX11FBO()
{
	Clear();
}

// -----------------------------------------------------------------------
S_API SResult DirectX11FBO::Initialize(EFBOType type, SSpeedPointEngine* pEngine, IRenderer* pRenderer, unsigned int nW, unsigned int nH)
{
	Clear(); // make sure to clear before initialize again

	SP_ASSERTR((m_pEngine = pEngine), S_INVALIDPARAM);
	SP_ASSERTXR(pRenderer && pRenderer->GetType() == S_DIRECTX11, S_INVALIDPARAM, m_pEngine);		

	m_pDXRenderer = (DirectX11Renderer*)pRenderer;
	m_FBOType = type;

	// By default we use sRGB 32bit unsigned normalized buffer
	DXGI_FORMAT texFmt;	
	switch (type)
	{
	default:
		texFmt = DXGI_FORMAT_R8G8B8A8_UNORM_SRGB;
	}

	// Setup the texture description structure
	D3D11_TEXTURE2D_DESC texDesc;
	texDesc.ArraySize = 1;
	texDesc.BindFlags = D3D11_BIND_RENDER_TARGET | D3D11_BIND_SHADER_RESOURCE;		
	texDesc.Height = nH;
	texDesc.Width = nW;
	texDesc.MiscFlags = 0;
	texDesc.Usage = D3D11_USAGE_DEFAULT; // maybe make this specificable someday							
	texDesc.Format = texFmt;
	texDesc.SampleDesc = GetD3D11MSAADesc(
		texDesc.Format,
		m_pDXRenderer->GetD3D11Device(),
		m_pDXRenderer->GetSettings()->GetMSAACount(),
		m_pDXRenderer->GetSettings()->GetMSAAQuality());

	// now finally create the texture
	HRESULT hRes;
	if (Failure(hRes = m_pDXRenderer->GetD3D11Device()->CreateTexture2D(&texDesc, 0, &m_pTexture)))
	{
		return m_pEngine->LogE("Failed CreateTexture2D while attempting to create FBO!");
	}


	// setup description of rtv
	D3D11_RENDER_TARGET_VIEW_DESC rtvDesc;
	rtvDesc.Format = texDesc.Format;
	rtvDesc.Texture2D.MipSlice = 0;
	rtvDesc.ViewDimension = D3D11_RTV_DIMENSION_TEXTURE2D;

	// create the rtv
	if (Failure(hRes = m_pDXRenderer->GetD3D11Device()->CreateRenderTargetView(m_pTexture, &rtvDesc, &m_pRTV)))
	{
		return m_pEngine->LogE("Failed CreateRTV while attempting to create FBO!");
	}


	return S_SUCCESS;
}

// -----------------------------------------------------------------------
S_API SResult DirectX11FBO::InitializeDSV()
{
	SP_ASSERTR(IsInitialized(), S_NOTINIT);

	ID3D11Device* pD3DDevice = m_pDXRenderer->GetD3D11Device();
	SP_ASSERTR(pD3DDevice, S_NOTINIT);


	// Setup the depth Buffer
	DXGI_FORMAT depthStencilBufferFormat = DXGI_FORMAT_D24_UNORM_S8_UINT; // you might want to change this format someday		
	usint32 nDepthStencilWidth = m_nBufferWidth;
	usint32 nDepthStencilHeight = m_nBufferHeight;


	// Create the Buffer texture
	D3D11_TEXTURE2D_DESC dsTexDesc;
	memset((void*)&dsTexDesc, 0, sizeof(D3D11_TEXTURE2D_DESC));
	dsTexDesc.ArraySize = 1; // one depth buffer is probably enough
	dsTexDesc.BindFlags = D3D11_BIND_DEPTH_STENCIL;
	dsTexDesc.CPUAccessFlags = 0;
	dsTexDesc.Format = depthStencilBufferFormat;
	// WARNING: If you change them make sure that stencil is supported.
	// From msdn: "DXGI_FORMAT_D24_UNORM_S8_UINT and DXGI_FORMAT_D32_FLOAT_S8X24_UINT are the only formats to support stencil!"
	dsTexDesc.Width = nDepthStencilWidth;
	dsTexDesc.Height = nDepthStencilHeight;
	dsTexDesc.MipLevels = 1;	// don't support mipmapping for depth stencil buffer
	dsTexDesc.MiscFlags = 0;
	dsTexDesc.SampleDesc.Count = 1;
	dsTexDesc.SampleDesc.Quality = 0; // don't support Depth MSAA
	dsTexDesc.Usage = D3D11_USAGE_DEFAULT;

	if (Failure(pD3DDevice->CreateTexture2D(&dsTexDesc, 0, &m_pDepthStencilBuffer)))
	{
		return m_pEngine->LogE("Failed Create Depth Stencil Buffer Texture!");
	}


	// Create the Depth-Stencil view for this viewport
	D3D11_DEPTH_STENCIL_VIEW_DESC depthStencilViewDesc;
	memset(&depthStencilViewDesc, 0, sizeof(D3D11_DEPTH_STENCIL_VIEW_DESC));
	depthStencilViewDesc.Flags = 0;
	depthStencilViewDesc.Format = depthStencilBufferFormat;
	depthStencilViewDesc.Texture2D.MipSlice = 0;	// ???
	depthStencilViewDesc.ViewDimension = D3D11_DSV_DIMENSION_TEXTURE2D;

	if (Failure(pD3DDevice->CreateDepthStencilView(m_pDepthStencilBuffer, &depthStencilViewDesc, &m_pDepthStencilView)))
	{
		return m_pEngine->LogE("Failed Create Depth Stencil View!");
	}

	
	// okay. done.
	return S_SUCCESS;
}

// -----------------------------------------------------------------------
S_API bool DirectX11FBO::IsInitialized()
{
	return (m_pTexture && m_pDXRenderer && m_pEngine
		&& m_nBufferWidth > 0 && m_nBufferHeight > 0);
}

// -----------------------------------------------------------------------
S_API void DirectX11FBO::Clear(void)
{
	SP_SAFE_RELEASE(m_pRTV);
	SP_SAFE_RELEASE(m_pTexture);
	SP_SAFE_RELEASE(m_pDepthStencilView);
	SP_SAFE_RELEASE(m_pDepthStencilBuffer);

	m_pRTV = 0;
	m_pTexture = 0;
	m_pDepthStencilView = 0;
	m_pDepthStencilBuffer = 0;
	m_pDXRenderer = 0;
	m_pEngine = 0;
}



SP_NMSPACE_END