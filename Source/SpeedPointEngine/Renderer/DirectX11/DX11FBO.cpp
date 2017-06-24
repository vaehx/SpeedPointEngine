//////////////////////////////////////////////////////////////////////////////////////////////////
//
//	SpeedPoint Game Engine
//	Copyright (c) 2011-2017 Pascal Rosenkranz, All rights reserved.
//
//////////////////////////////////////////////////////////////////////////////////////////////////

#include "DX11FBO.h"
#include "DX11Renderer.h"
#include "DX11Utilities.h"
#include "DX11ResourcePool.h"
#include "..\IRenderer.h"

SP_NMSPACE_BEG

inline DXGI_FORMAT GetDXGIFormatFromFBOType(EFBOType type)
{
	switch (type)
	{
	case eFBO_F16:
		return DXGI_FORMAT_R16_UNORM;
	case eFBO_F32:
		return DXGI_FORMAT_R32_TYPELESS;
	case eFBO_R16G16F:
		return DXGI_FORMAT_R16G16_FLOAT;
	case eFBO_R16G16B16A16F:
		return DXGI_FORMAT_R16G16B16A16_FLOAT;
	case eFBO_R8G8B8A8:
	default:
		return DXGI_FORMAT_R8G8B8A8_UNORM;
	}
}


// -----------------------------------------------------------------------------------------------
S_API DX11FBO::DX11FBO(DX11Renderer* pRenderer)
	: m_pDXRenderer(pRenderer),
	m_pFrameBuffer(0),
	m_pRTV(0),
	m_pFrameBufferTexture(0),
	m_pDepthBuffer(0),
	m_pDSV(0),
	m_pReadonlyDSV(0),
	m_pDepthBufferTexture(0)
{
}

// -----------------------------------------------------------------------------------------------
S_API DX11FBO::~DX11FBO()
{
	Clear();
}

// -----------------------------------------------------------------------------------------------
S_API SResult DX11FBO::Initialize(EFBOType type, unsigned int width, unsigned int height)
{
	HRESULT hRes;

	DX11Renderer* pRenderer = m_pDXRenderer;
	
	Clear();
	
	SP_ASSERTR(m_pDXRenderer = pRenderer, S_NOTINIT);

	m_FBOType = type;

	// Create frame buffer texture
	memset(&m_texDesc, 0, sizeof(m_texDesc));
	m_texDesc.Format = GetDXGIFormatFromFBOType(type);
	m_texDesc.ArraySize = 1;
	m_texDesc.Width = width;
	m_texDesc.Height = height;
	m_texDesc.MiscFlags = 0;
	m_texDesc.Usage = D3D11_USAGE_DEFAULT; // maybe make this specificable someday
	m_texDesc.SampleDesc = GetD3D11MSAADesc(
		m_texDesc.Format,
		m_pDXRenderer->GetD3D11Device(),
		m_pDXRenderer->GetParams().antiAliasingCount,
		m_pDXRenderer->GetParams().antiAliasingQuality);

	m_texDesc.BindFlags = D3D11_BIND_RENDER_TARGET
		| D3D11_BIND_SHADER_RESOURCE;	// assumes that all FBOs currently are used as textures as well - maybe we want to change this someday!

	if (Failure(hRes = m_pDXRenderer->GetD3D11Device()->CreateTexture2D(&m_texDesc, 0, &m_pFrameBuffer)))
		return CLog::Log(S_ERROR, "DX11FBO::Initialize(): Failed CreateTexture2D!");

	// Create RTV for frame buffer
	D3D11_RENDER_TARGET_VIEW_DESC rtvDesc;
	memset(&rtvDesc, 0, sizeof(rtvDesc));
	rtvDesc.Format = m_texDesc.Format;
	rtvDesc.Texture2D.MipSlice = 0;
	rtvDesc.ViewDimension = D3D11_RTV_DIMENSION_TEXTURE2D;

	if (Failure(hRes = m_pDXRenderer->GetD3D11Device()->CreateRenderTargetView(m_pFrameBuffer, &rtvDesc, &m_pRTV)))
		return CLog::Log(S_ERROR, "DX11FBO::Initialize(): Failed CreateRenderTargetView!");

	return S_SUCCESS;
}

// -----------------------------------------------------------------------------------------------
S_API SResult DX11FBO::InitializeAsTexture(EFBOType type, unsigned int width, unsigned int height, const string& specification)
{
	if (specification.empty())
		return CLog::Log(S_ERROR, "Failed DX11FBO::InitializeAsTexture(): specification empty");

	SResult res;
	if (Failure(res = Initialize(type, width, height)))
		return res;

	m_pFrameBufferTexture = dynamic_cast<DX11Texture*>(m_pDXRenderer->GetResourcePool()->GetTexture(specification));
	if (!m_pFrameBufferTexture)
		return S_ERROR;

	if (Failure(m_pFrameBufferTexture->D3D11_InitializeFromExistingResource(m_pFrameBuffer)))
		return S_ERROR;

	m_pFrameBufferTexture->SetSpecification(specification);

	return S_SUCCESS;
}

// -----------------------------------------------------------------------------------------------
S_API SResult DX11FBO::D3D11_InitializeFromCustomResource(ID3D11Resource* pResource, unsigned int width, unsigned height,
	bool allowAsTexture /*= false*/, const string& specification /*= ""*/)
{
	DX11Renderer* pRenderer = m_pDXRenderer;
	Clear();

	SP_ASSERTR(m_pDXRenderer = pRenderer, S_NOTINIT);

	m_texDesc.Width = width;
	m_texDesc.Height = height;

	// Create RTV for frame buffer
	if (Failure(m_pDXRenderer->GetD3D11Device()->CreateRenderTargetView(pResource, 0, &m_pRTV)))
		return CLog::Log(S_ERROR, "Failed CreateRTV while attempting to create FBO!");

	// Create texture if necessary
	if (allowAsTexture)
	{
		m_pFrameBufferTexture = dynamic_cast<DX11Texture*>(pRenderer->GetResourcePool()->GetTexture(specification));
		if (!m_pFrameBufferTexture)
			return S_ERROR;

		if (Failure(m_pFrameBufferTexture->D3D11_InitializeFromExistingResource(m_pFrameBuffer)))
			return S_ERROR;
	}

	return S_SUCCESS;
}

// -----------------------------------------------------------------------------------------------
S_API SResult DX11FBO::InitializeDepthBufferIntrnl(bool allowAsTexture, const string& specification, unsigned int resolution[2])
{
	SP_ASSERTR(m_pDXRenderer, S_NOTINIT);

	ID3D11Device* pD3DDevice = m_pDXRenderer->GetD3D11Device();
	SP_ASSERTR(pD3DDevice, S_NOTINIT);

	// Create the depth buffer texture
	memset((void*)&m_DepthBufferDesc, 0, sizeof(D3D11_TEXTURE2D_DESC));
	m_DepthBufferDesc.ArraySize = 1;
	m_DepthBufferDesc.BindFlags = D3D11_BIND_DEPTH_STENCIL;
	m_DepthBufferDesc.CPUAccessFlags = 0;
	m_DepthBufferDesc.MipLevels = 1;
	m_DepthBufferDesc.MiscFlags = 0;
	m_DepthBufferDesc.SampleDesc.Count = 1;
	m_DepthBufferDesc.SampleDesc.Quality = 0; // don't support Depth MSAA
	m_DepthBufferDesc.Usage = D3D11_USAGE_DEFAULT;

	m_DepthBufferDesc.Format = DXGI_FORMAT_D32_FLOAT;
	
	if (resolution)
	{
		m_DepthBufferDesc.Width = resolution[0];
		m_DepthBufferDesc.Height = resolution[1];
	}
	else if (m_pFrameBuffer || m_pRTV) // may be created from external resource
	{
		m_DepthBufferDesc.Width = m_texDesc.Width;
		m_DepthBufferDesc.Height = m_texDesc.Height;
	}
	else
	{
		return CLog::Log(S_ERROR, "DX11FBO: Failed create depth buffer: resolution is 0 and frame buffer not initialized.");
	}

	if (allowAsTexture)
	{
		// Buffer	-> DXGI_FORMAT_R32_TYPELESS
		// DSV		-> DXGI_FORMAT_D32_FLOAT
		// SRV		-> DXGI_FORMAT_R32_FLOAT
		m_DepthBufferDesc.Format = DXGI_FORMAT_R32_TYPELESS;
		m_DepthBufferDesc.BindFlags |= D3D11_BIND_SHADER_RESOURCE;
	}

	if (Failure(pD3DDevice->CreateTexture2D(&m_DepthBufferDesc, 0, &m_pDepthBuffer)))
		return CLog::Log(S_ERROR, "Failed Create Depth Stencil Buffer Texture!");

	// Create the Depth-Stencil view for this viewport
	D3D11_DEPTH_STENCIL_VIEW_DESC depthStencilViewDesc;
	memset(&depthStencilViewDesc, 0, sizeof(D3D11_DEPTH_STENCIL_VIEW_DESC));
	depthStencilViewDesc.Flags = 0;
	depthStencilViewDesc.Format = DXGI_FORMAT_D32_FLOAT;
	depthStencilViewDesc.ViewDimension = D3D11_DSV_DIMENSION_TEXTURE2D;
	depthStencilViewDesc.Texture2D.MipSlice = 0;

	if (Failure(pD3DDevice->CreateDepthStencilView(m_pDepthBuffer, &depthStencilViewDesc, &m_pDSV)))
		return CLog::Log(S_ERROR, "Failed Create Depth Stencil View!");


	if (allowAsTexture)
	{
		// Depth buffer read-only-DSV
		D3D11_DEPTH_STENCIL_VIEW_DESC readonlyDSV;
		memset(&readonlyDSV, 0, sizeof(readonlyDSV));
		readonlyDSV.Flags = D3D11_DSV_READ_ONLY_DEPTH;
		readonlyDSV.Format = depthStencilViewDesc.Format;
		readonlyDSV.ViewDimension = D3D11_DSV_DIMENSION_TEXTURE2D;
		readonlyDSV.Texture2D.MipSlice = 0;

		if (Failure(m_pDXRenderer->GetD3D11Device()->CreateDepthStencilView(m_pDepthBuffer, &readonlyDSV, &m_pReadonlyDSV)))
			return CLog::Log(S_ERROR, "Failed create readonly-dsv for fbo");

		// Create texture
		m_pDepthBufferTexture = dynamic_cast<DX11Texture*>(m_pDXRenderer->GetResourcePool()->GetTexture(specification));
		if (!m_pDepthBufferTexture)
			return S_ERROR;

		m_pDepthBufferTexture->SetSpecification(specification);
		if (Failure(m_pDepthBufferTexture->D3D11_InitializeFromExistingResource(m_pDepthBuffer, DXGI_FORMAT_R32_FLOAT)))
			return S_ERROR;
	}

	return S_SUCCESS;
}

S_API SResult DX11FBO::InitializeDepthBuffer(unsigned int resolution[2])
{
	return InitializeDepthBufferIntrnl(false, "", resolution);
}

S_API SResult DX11FBO::InitializeDepthBufferAsTexture(const string& specification, unsigned int resolution[2])
{
	return InitializeDepthBufferIntrnl(true, specification, resolution);
}

// -----------------------------------------------------------------------------------------------
S_API bool DX11FBO::IsInitialized() const
{
	return IS_VALID_PTR(m_pRTV);
}

// -----------------------------------------------------------------------------------------------
S_API ITexture* DX11FBO::GetTexture() const
{
	return m_pFrameBufferTexture;
}

S_API ID3D11ShaderResourceView* DX11FBO::GetSRV() const
{
	return (m_pFrameBufferTexture ? m_pFrameBufferTexture->D3D11_GetSRV() : 0);
}

// -----------------------------------------------------------------------------------------------
S_API ITexture* DX11FBO::GetDepthBufferTexture() const
{
	return m_pDepthBufferTexture;
}

S_API ID3D11ShaderResourceView* DX11FBO::GetDepthSRV() const
{
	return (m_pDepthBufferTexture ? m_pDepthBufferTexture->D3D11_GetSRV() : 0);
}

// -----------------------------------------------------------------------------------------------
S_API void DX11FBO::Clear(void)
{
	SP_SAFE_RELEASE(m_pFrameBufferTexture);
	SP_SAFE_RELEASE(m_pDepthBufferTexture);
	SP_SAFE_RELEASE(m_pRTV);
	SP_SAFE_RELEASE(m_pDSV);
	SP_SAFE_RELEASE(m_pReadonlyDSV);
	SP_SAFE_RELEASE(m_pDepthBuffer);
	SP_SAFE_RELEASE(m_pFrameBuffer);

	m_pDXRenderer = 0;
}



SP_NMSPACE_END
