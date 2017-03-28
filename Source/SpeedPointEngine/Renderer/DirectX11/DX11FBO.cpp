//////////////////////////////////////////////////////////////////////////////////////////////////
//
//	SpeedPoint Game Engine
//	Copyright (c) 2011-2016 Pascal Rosenkranz, All rights reserved.
//
//////////////////////////////////////////////////////////////////////////////////////////////////

#include "DX11FBO.h"
#include "DX11Renderer.h"
#include "DX11Utilities.h"
#include "..\IRenderer.h"

SP_NMSPACE_BEG

// -----------------------------------------------------------------------------------------------
S_API DX11FBO::DX11FBO()
	: m_pDXRenderer(0),
	m_pTexture(0),
	m_pRTV(0),
	m_nBufferWidth(0),
	m_nBufferHeight(0),
	m_bSwapChainFBO(false)
{
}

// -----------------------------------------------------------------------------------------------
S_API DX11FBO::~DX11FBO()
{
	Clear();
}

// -----------------------------------------------------------------------------------------------
S_API SResult DX11FBO::Initialize(EFBOType type, IRenderer* pRenderer, unsigned int nW, unsigned int nH)
{
	Clear(); // make sure to clear before initialize again

	if (!IS_VALID_PTR(pRenderer) || pRenderer->GetType() != S_DIRECTX11)
		return S_INVALIDPARAM;

	m_pDXRenderer = dynamic_cast<DX11Renderer*>(pRenderer);
	m_FBOType = type;
	m_nBufferWidth = nW;
	m_nBufferHeight = nH;

	// By default we use sRGB 32bit unsigned normalized buffer
	DXGI_FORMAT texFmt;
	switch (type)
	{
	case eFBO_F32:
		texFmt = DXGI_FORMAT_R32_FLOAT;
		break;
	case eFBO_R8G8B8A8:
	default:
		texFmt = DXGI_FORMAT_R8G8B8A8_UNORM;
	}

	// Setup the texture description structure
	m_texDesc.ArraySize = 1;


	m_texDesc.BindFlags = D3D11_BIND_RENDER_TARGET
		| D3D11_BIND_SHADER_RESOURCE;	// assumes that all FBOs currently are used as textures as well - maybe we want to change this someday!



	m_texDesc.Height = nH;
	m_texDesc.Width = nW;
	m_texDesc.MiscFlags = 0;
	m_texDesc.Usage = D3D11_USAGE_DEFAULT; // maybe make this specificable someday
	m_texDesc.Format = texFmt;
	m_texDesc.SampleDesc = GetD3D11MSAADesc(
		m_texDesc.Format,
		m_pDXRenderer->GetD3D11Device(),
		m_pDXRenderer->GetParams().antiAliasingCount,
		m_pDXRenderer->GetParams().antiAliasingQuality);

	// now finally create the texture
	HRESULT hRes;
	if (Failure(hRes = m_pDXRenderer->GetD3D11Device()->CreateTexture2D(&m_texDesc, 0, &m_pTexture)))
	{
		return CLog::Log(S_ERROR, "Failed CreateTexture2D while attempting to create FBO!");
	}


	// setup description of rtv
	D3D11_RENDER_TARGET_VIEW_DESC rtvDesc;
	rtvDesc.Format = m_texDesc.Format;
	rtvDesc.Texture2D.MipSlice = 0;
	rtvDesc.ViewDimension = D3D11_RTV_DIMENSION_TEXTURE2D;

	// create the rtv
	if (Failure(hRes = m_pDXRenderer->GetD3D11Device()->CreateRenderTargetView(m_pTexture, &rtvDesc, &m_pRTV)))
	{
		return CLog::Log(S_ERROR, "Failed CreateRTV while attempting to create FBO!");
	}


	return S_SUCCESS;
}

// -----------------------------------------------------------------------------------------------
S_API SResult DX11FBO::InitializeDSV()
{
	SP_ASSERTR(IsInitialized(), S_NOTINIT);

	ID3D11Device* pD3DDevice = m_pDXRenderer->GetD3D11Device();
	SP_ASSERTR(pD3DDevice, S_NOTINIT);


	// Setup the depth Buffer
	DXGI_FORMAT depthStencilBufferFormat = DXGI_FORMAT_D32_FLOAT; // you might want to change this format someday
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
		return CLog::Log(S_ERROR, "Failed Create Depth Stencil Buffer Texture!");
	}


	// Create the Depth-Stencil view for this viewport
	D3D11_DEPTH_STENCIL_VIEW_DESC depthStencilViewDesc;
	memset(&depthStencilViewDesc, 0, sizeof(D3D11_DEPTH_STENCIL_VIEW_DESC));
	depthStencilViewDesc.Flags = 0;
	depthStencilViewDesc.Format = depthStencilBufferFormat;
	depthStencilViewDesc.Texture2D.MipSlice = 0;	// ???

	depthStencilViewDesc.ViewDimension = D3D11_DSV_DIMENSION_TEXTURE2D;	// Maybe someday use MS?

	if (Failure(pD3DDevice->CreateDepthStencilView(m_pDepthStencilBuffer, &depthStencilViewDesc, &m_pDepthStencilView)))
	{
		return CLog::Log(S_ERROR, "Failed Create Depth Stencil View!");
	}


	// okay. done.
	return S_SUCCESS;
}

// -----------------------------------------------------------------------------------------------
S_API SResult DX11FBO::InitializeSRV()
{
	SP_ASSERTR(m_pTexture, S_NOTINIT);

	D3D11_SHADER_RESOURCE_VIEW_DESC srvDesc;
	memset(&srvDesc, 0, sizeof(srvDesc));
	srvDesc.Format = m_texDesc.Format;
	srvDesc.ViewDimension = D3D11_SRV_DIMENSION_TEXTURE2D;		// maybe someday use MS?
	srvDesc.Texture2D.MipLevels = 1;

	if (Failure(m_pDXRenderer->GetD3D11Device()->CreateShaderResourceView(m_pTexture, &srvDesc, &m_pSRV)))
		return CLog::Log(S_ERROR, "Failed create shader resource view for FBO!");

	return S_SUCCESS;
}

// -----------------------------------------------------------------------------------------------
S_API bool DX11FBO::IsInitialized()
{
	if (m_bSwapChainFBO)
		return m_pRTV != nullptr; // note: DSV does not need to be initialized
	else
		return (m_pTexture && m_pDXRenderer && m_nBufferWidth > 0 && m_nBufferHeight > 0);
}

// -----------------------------------------------------------------------------------------------
S_API void DX11FBO::Clear(void)
{
	if (!m_bSwapChainFBO)
	{
		SP_SAFE_RELEASE(m_pRTV);
		SP_SAFE_RELEASE(m_pDepthStencilView);
	}

	SP_SAFE_RELEASE(m_pTexture);
	SP_SAFE_RELEASE(m_pDepthStencilBuffer);
	SP_SAFE_RELEASE(m_pSRV);

	m_pRTV = 0;
	m_pTexture = 0;
	m_pDepthStencilView = 0;
	m_pDepthStencilBuffer = 0;
	m_pDXRenderer = 0;
}



SP_NMSPACE_END
