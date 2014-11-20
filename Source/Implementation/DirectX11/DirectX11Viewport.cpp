//////////////////////////////////////////////////////////////////////////////////////////////////
//
//	This file is part of the SpeedPoint Game Engine
//
//	written by Pascal R. aka iSmokiieZz
//	(c) 2011-2014, All rights reserved.
//
//////////////////////////////////////////////////////////////////////////////////////////////////

#include <Implementation\DirectX11\DirectX11Viewport.h>
#include <Implementation\DirectX11\DirectX11Renderer.h>
#include <Implementation\DirectX11\DirectX11FBO.h>
#include <Implementation\DirectX11\DirectX11Utilities.h>
#include <Util\SMatrix.h>
#include <Abstract\IGameEngine.h>
#include <Abstract\IRenderer.h>

#ifdef _DEBUG
#include <dxgi1_3.h>
#endif

SP_NMSPACE_BEG

// -------------------------------------------------------------------
S_API DirectX11Viewport::DirectX11Viewport()
: m_pEngine(0),
m_pRenderer(0),
m_pRenderTarget(0),	
m_bIsAdditional(false),
m_pDepthStencilBuffer(0),	
m_pDepthStencilView(0),
m_pSwapChain(0),
m_nBackBuffers(0)
{
	m_pFBO = new DirectX11FBO();
	m_pCamera = &m_OwnCamera;
}

// -------------------------------------------------------------------
S_API DirectX11Viewport::~DirectX11Viewport()
{
	// make sure to clear
	Clear();
}

// -------------------------------------------------------------------
S_API SResult DirectX11Viewport::Initialize(IGameEngine* pEngine, const SViewportDescription& desc, bool bIsAdditional)
{
	SP_ASSERTR(pEngine, S_INVALIDPARAM);
	SP_ASSERTR(desc.width > 640 && desc.height > 480, S_INVALIDPARAM);
	SP_ASSERTR(desc.fov > 0, S_INVALIDPARAM);
	SP_ASSERTR(desc.hWnd, S_INVALIDPARAM);

	Clear(); // make sure to clear before initializing again

	// make sure the FBO instance is instanciated, because we might have deleted it in Clear()
	if (!m_pFBO)
		m_pFBO = new DirectX11FBO();	

	m_pEngine = pEngine;
	SSettingsDesc& engineSet = m_pEngine->GetSettings()->Get();

	IRenderer* pRenderer = pEngine->GetRenderer();
	SP_ASSERTR(pRenderer->GetType() == S_DIRECTX11, S_INVALIDPARAM);

	m_pRenderer = (DirectX11Renderer*)pRenderer;
	DirectX11Settings* pRendererSet = (DirectX11Settings*)m_pRenderer->GetSettings();

	m_bIsAdditional = bIsAdditional;
	m_Desc = desc;
	m_nBackBuffers = 1;// only one buffer. render targets will be handled externally	

	m_DXViewportDesc.Width = (float)desc.width;
	m_DXViewportDesc.Height = (float)desc.height;
	m_DXViewportDesc.MinDepth = 0;
	m_DXViewportDesc.MaxDepth = 1.0f;
	m_DXViewportDesc.TopLeftX = 0.0f;
	m_DXViewportDesc.TopLeftY = 0.0f;


	// Setup swap chain description
	DXGI_SWAP_CHAIN_DESC swapChainDesc;
	swapChainDesc.BufferCount = m_nBackBuffers;
	swapChainDesc.BufferDesc = m_pRenderer->GetD3D11AutoSelectedDisplayModeDesc();
	/*if (!engineSet.render.bEnableVSync)
	{
		swapChainDesc.BufferDesc.RefreshRate.Numerator = 0;
		swapChainDesc.BufferDesc.RefreshRate.Denominator = 1;
	}
	*/
	swapChainDesc.BufferUsage = DXGI_USAGE_RENDER_TARGET_OUTPUT;
	swapChainDesc.Flags = 0;
	swapChainDesc.OutputWindow = desc.hWnd;
	swapChainDesc.SampleDesc = GetD3D11MSAADesc(
		swapChainDesc.BufferDesc.Format,
		m_pRenderer->GetD3D11Device(),
		pRendererSet->GetMSAACount(),
		pRendererSet->GetMSAAQuality());
	swapChainDesc.SwapEffect = DXGI_SWAP_EFFECT_DISCARD; // i saw no purpose to change this to custom val as we want to support MSAA
	swapChainDesc.Windowed = engineSet.app.bWindowed;


	// Create the swapchain		
	if (Failure(m_pRenderer->D3D11_CreateSwapChain(&swapChainDesc, &m_pSwapChain)))
	{
		return m_pEngine->LogE("Failed create swap chain!");
	}
#ifdef _DEBUG
	const char nm[] = "DirectX11Viewport::SwapChain";
	m_pSwapChain->SetPrivateData(WKPDID_D3DDebugObjectName, sizeof(nm) - 1, nm);
#endif


	// Now create an RTV for this swapchain	
	ID3D11Resource* pBBResource;
	if (Failure(m_pSwapChain->GetBuffer(0, __uuidof(pBBResource), reinterpret_cast<void**>(&pBBResource))))
	{
		return m_pEngine->LogE("Failed retrieve BackBuffer resource of SwapChain in InitDefaultViewport!");
	}
	if (Failure(m_pRenderer->D3D11_CreateRTV(pBBResource, 0, &m_pRenderTarget)))
	{
		return m_pEngine->LogE("Failed create RTV for swapchain!");
	}

	pBBResource->Release();

	if (desc.useDepthStencil)
	{
		if (Failure(InitializeDepthStencilBuffer()))
			return S_ERROR;
	}

	// Setup the RTV of the FBO which is used by the renderer to bind the swapchain backbuffer as rendertarget	
	m_pFBO->FlagSwapchainFBO(m_pRenderTarget, m_pDepthStencilView);	


	// Instanciate the camera
	m_pCamera = new SCamera();





	// and finally initialize the projection matrix for this viewport
	if (Failure(Set3DProjection(desc.projectionType, desc.fov, desc.orthoW, desc.orthoH)))
		return S_ERROR;





	return S_SUCCESS;
}

// -------------------------------------------------------------------
S_API SResult DirectX11Viewport::InitializeDepthStencilBuffer()
{
	SP_ASSERTR(m_pEngine && m_pRenderer && m_pSwapChain, S_NOTINIT);

	ID3D11Device* pD3DDevice = m_pRenderer->GetD3D11Device();
	ID3D11DeviceContext* pD3DDeviceContext = m_pRenderer->GetD3D11DeviceContext();
	SP_ASSERTR(pD3DDevice && pD3DDeviceContext, S_NOTINIT);		

	DXGI_SWAP_CHAIN_DESC chainDesc;
	m_pSwapChain->GetDesc(&chainDesc);	


	DXGI_FORMAT depthStencilBufferFormat = DXGI_FORMAT_D24_UNORM_S8_UINT; // you might want to change this format someday



	// Create the Depthstencil Buffer texture

	D3D11_TEXTURE2D_DESC dsTexDesc;	
	memset((void*)&dsTexDesc, 0, sizeof(D3D11_TEXTURE2D_DESC));
	dsTexDesc.ArraySize = 1; // one depth buffer is probably enough
	dsTexDesc.BindFlags = D3D11_BIND_DEPTH_STENCIL;
	dsTexDesc.CPUAccessFlags = 0;
	dsTexDesc.Format = depthStencilBufferFormat;
	// WARNING: If you change them make sure that stencil is supported.
	// From msdn: "DXGI_FORMAT_D24_UNORM_S8_UINT and DXGI_FORMAT_D32_FLOAT_S8X24_UINT are the only formats to support stencil!"
	dsTexDesc.Width = chainDesc.BufferDesc.Width;
	dsTexDesc.Height = chainDesc.BufferDesc.Height;
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


	// okay done.
	return S_SUCCESS;
}

// -------------------------------------------------------------------
S_API SResult DirectX11Viewport::Clear(void)
{		
	if (IS_VALID_PTR(m_pDepthStencilView))
	{
		m_pDepthStencilView->Release();
		m_pDepthStencilView = nullptr;
	}
	
	if (IS_VALID_PTR(m_pDepthStencilBuffer))
	{
		m_pDepthStencilBuffer->Release();
		m_pDepthStencilBuffer = nullptr;
	}

	if (IS_VALID_PTR(m_pRenderTarget))
	{
		m_pRenderTarget->Release();
		m_pRenderTarget = nullptr;
	}
	if (m_pFBO)
		delete m_pFBO;

	if (IS_VALID_PTR(m_pSwapChain))
	{
		m_pSwapChain->Release();
		m_pSwapChain = nullptr;
	}

	m_pEngine = 0;
	m_pRenderer = 0;
	m_pRenderTarget = 0;	
	m_Desc.hWnd = 0;
	m_pDepthStencilBuffer = 0;	
	m_pDepthStencilView = 0;	
	m_pCamera = 0;
	m_pFBO = 0;
	m_pCamera = nullptr;

	return S_SUCCESS;
}

// -------------------------------------------------------------------
S_API bool DirectX11Viewport::IsAdditional()
{
	return m_bIsAdditional;
}

// -------------------------------------------------------------------
S_API SIZE DirectX11Viewport::GetSize(void)
{
	SIZE size;
	size.cx = (LONG)m_DXViewportDesc.Width;
	size.cy = (LONG)m_DXViewportDesc.Height;

	return size;
}

// -------------------------------------------------------------------
S_API SResult DirectX11Viewport::SetSize(int nX, int nY)
{
	SP_ASSERTR(m_pSwapChain && m_pEngine && m_nBackBuffers > 0, S_NOTINIT);
	SP_ASSERT(nX > 640 && nY > 480, S_INVALIDPARAM);

	m_Desc.width = nX;
	m_Desc.height = nY;

	if (Failure(m_pSwapChain->ResizeBuffers(m_nBackBuffers, m_Desc.width, m_Desc.height, DXGI_FORMAT_UNKNOWN, 0)))
	{		
		return m_pEngine->LogE("Failed Resize Backbuffers of Viewport!");
	}

	return S_SUCCESS;
}

// -------------------------------------------------------------------
S_API SResult DirectX11Viewport::EnableVSync(bool enable)
{
	return S_SUCCESS;
}

// -------------------------------------------------------------------
S_API SVector2 DirectX11Viewport::GetOrthographicVolume(void)
{



	// TODO


	return SVector2();




}

// -------------------------------------------------------------------
S_API unsigned int DirectX11Viewport::GetPerspectiveFOV(void)
{
	return m_Desc.fov;
}

// -------------------------------------------------------------------
S_API SResult DirectX11Viewport::Set3DProjection(S_PROJECTION_TYPE type, unsigned int fPerspDegFOV, float fOrthoW, float fOrthoH)
{
	SSettingsDesc& engineSettings = m_pEngine->GetSettings()->Get();

	switch (type)
	{
	case S_PROJECTION_PERSPECTIVE:		
		SPMatrixPerspectiveFovRH(
			&m_ProjectionMtx,
			SP_DEG_TO_RAD(fPerspDegFOV),
			m_DXViewportDesc.Width / m_DXViewportDesc.Height,
			engineSettings.render.fClipNear,
			engineSettings.render.fClipFar);
		break;

	default:
		return S_INVALIDPARAM;
	}

	m_Desc.fov = fPerspDegFOV;
	m_Desc.orthoW = fOrthoW;
	m_Desc.orthoH = fOrthoH;
	m_Desc.projectionType = type;	

	return S_SUCCESS;
}

// -------------------------------------------------------------------
S_API SMatrix4& DirectX11Viewport::GetProjectionMatrix()
{
	return m_ProjectionMtx;
}

// -------------------------------------------------------------------
S_API SMatrix& DirectX11Viewport::GetCameraViewMatrix()
{
	return m_pCamera->viewMatrix;
}

// -------------------------------------------------------------------
S_API SResult DirectX11Viewport::RecalculateCameraViewMatrix()
{
	m_pCamera->RecalculateViewMatrix();
	return S_SUCCESS;
}

// -------------------------------------------------------------------
S_API IFBO* DirectX11Viewport::GetBackBuffer(void)
{
	return (IFBO*)m_pFBO;
}


SP_NMSPACE_END