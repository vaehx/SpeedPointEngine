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
	m_pRTV(0)
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


	// Setup the texture description structure
	D3D11_TEXTURE2D_DESC texDesc;
	texDesc.ArraySize = 1;
	texDesc.BindFlags = D3D11_BIND_RENDER_TARGET | D3D11_BIND_SHADER_RESOURCE;		
	texDesc.Height = nH;
	texDesc.Width = nW;
	texDesc.MiscFlags = 0;
	texDesc.Usage = D3D11_USAGE_DEFAULT; // maybe make this specificable someday							
	texDesc.Format = DXGI_FORMAT_R8G8B8A8_UNORM_SRGB;
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
S_API bool DirectX11FBO::IsInitialized()
{
	return (m_pTexture && m_pDXRenderer && m_pEngine);
}

// -----------------------------------------------------------------------
S_API void DirectX11FBO::Clear(void)
{
	SP_SAFE_RELEASE(m_pRTV);
	SP_SAFE_RELEASE(m_pTexture);

	m_pRTV = 0;
	m_pTexture = 0;
	m_pDXRenderer = 0;
	m_pEngine = 0;
}



SP_NMSPACE_END