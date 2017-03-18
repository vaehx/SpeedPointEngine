//////////////////////////////////////////////////////////////////////////////////////////////////
//
//	SpeedPoint Game Engine
//	Copyright (c) 2011-2016 Pascal Rosenkranz, All rights reserved.
//
//////////////////////////////////////////////////////////////////////////////////////////////////

#include "DX11Viewport.h"
#include "DX11Renderer.h"
#include "DX11FBO.h"
#include "DX11Utilities.h"
#include <Abstract\IGameEngine.h>
#include <Abstract\IRenderer.h>
#include <Abstract\ISettings.h>
#include <Abstract\Matrix.h>

#ifdef _DEBUG
#include <dxgi1_3.h>
#endif

SP_NMSPACE_BEG

// -----------------------------------------------------------------------------------------------
S_API DX11Viewport::DX11Viewport()
: m_pEngine(0),
m_pRenderer(0),
m_pRenderTarget(0),	
m_bIsAdditional(false),
m_pDepthStencilBuffer(0),	
m_pDepthStencilView(0),
m_pSwapChain(0),
m_nBackBuffers(0)
{
	m_pFBO = new DX11FBO();
	m_pCamera = &m_OwnCamera;
}

// -----------------------------------------------------------------------------------------------
S_API DX11Viewport::~DX11Viewport()
{
	// make sure to clear
	Clear();
}

// -----------------------------------------------------------------------------------------------
S_API SResult DX11Viewport::Initialize(IGameEngine* pEngine, const SViewportDescription& desc, bool bIsAdditional)
{
	SP_ASSERTR(pEngine, S_INVALIDPARAM);
	SP_ASSERTR(desc.width > 640 && desc.height > 480, S_INVALIDPARAM);	
	SP_ASSERTR(desc.hWnd, S_INVALIDPARAM);

	Clear(); // make sure to clear before initializing again

	// make sure the FBO instance is instanciated, because we might have deleted it in Clear()
	if (!m_pFBO)
		m_pFBO = new DX11FBO();	

	m_pEngine = pEngine;
	SSettingsDesc& engineSet = m_pEngine->GetSettings()->Get();

	IRenderer* pRenderer = pEngine->GetRenderer();
	SP_ASSERTR(pRenderer->GetType() == S_DIRECTX11, S_INVALIDPARAM);

	m_pRenderer = dynamic_cast<DX11Renderer*>(pRenderer);
	SP_ASSERTR(IS_VALID_PTR(m_pRenderer), S_INVALIDPARAM);

	m_bIsAdditional = bIsAdditional;
	m_Desc = desc;
	m_nBackBuffers = 1;// only one buffer. render targets will be handled externally	

	m_DXViewportDesc.Width = (float)desc.width;
	m_DXViewportDesc.Height = (float)desc.height;
	m_DXViewportDesc.MinDepth = 0;
	m_DXViewportDesc.MaxDepth = 1.0f;
	m_DXViewportDesc.TopLeftX = 0.0f;
	m_DXViewportDesc.TopLeftY = 0.0f;


	// Setup swap chain description:

	DXGI_SWAP_CHAIN_DESC swapChainDesc;
	swapChainDesc.BufferCount = m_nBackBuffers;
	swapChainDesc.Windowed = engineSet.app.bWindowed;

	// The buffer desc returned by GetD3D11AutoSelectedDisplayModeDesc() is already created with
	// respect to windowed mode.
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
		engineSet.render.msaaCount,
		engineSet.render.msaaQuality);
	swapChainDesc.SwapEffect = DXGI_SWAP_EFFECT_DISCARD; // i saw no purpose to change this to custom val as we want to support MSAA	





	// Create the swapchain		
	if (Failure(m_pRenderer->D3D11_CreateSwapChain(&swapChainDesc, &m_pSwapChain)))
	{
		return CLog::Log(S_ERROR, "Failed create swap chain!");
	}
#ifdef _DEBUG
	const char nm[] = "DX11Viewport::SwapChain";
	m_pSwapChain->SetPrivateData(WKPDID_D3DDebugObjectName, sizeof(nm) - 1, nm);
#endif


	// Now create an RTV for this swapchain	
	ID3D11Resource* pBBResource;
	if (Failure(m_pSwapChain->GetBuffer(0, __uuidof(pBBResource), reinterpret_cast<void**>(&pBBResource))))
	{
		return CLog::Log(S_ERROR, "Failed retrieve BackBuffer resource of SwapChain in InitDefaultViewport!");
	}
	if (Failure(m_pRenderer->D3D11_CreateRTV(pBBResource, 0, &m_pRenderTarget)))
	{
		return CLog::Log(S_ERROR, "Failed create RTV for swapchain!");
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
	//m_pCamera = new SCamera();
	m_pCamera = &m_OwnCamera;





	// and finally initialize the projection matrix for this viewport	
	if (Failure(SetProjectionByDesc(m_Desc.projectionDesc)))
		return S_ERROR;





	return S_SUCCESS;
}

// -----------------------------------------------------------------------------------------------
S_API SResult DX11Viewport::InitializeDepthStencilBuffer()
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
		return CLog::Log(S_ERROR, "Failed Create Depth Stencil Buffer Texture!");
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
		return CLog::Log(S_ERROR, "Failed Create Depth Stencil View!");
	}


	// okay done.
	return S_SUCCESS;
}

// -----------------------------------------------------------------------------------------------
S_API SResult DX11Viewport::Clear(void)
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

// -----------------------------------------------------------------------------------------------
S_API bool DX11Viewport::IsAdditional()
{
	return m_bIsAdditional;
}

// -----------------------------------------------------------------------------------------------
S_API SIZE DX11Viewport::GetSize(void)
{
	SIZE size;
	size.cx = (LONG)m_DXViewportDesc.Width;
	size.cy = (LONG)m_DXViewportDesc.Height;

	return size;
}

// -----------------------------------------------------------------------------------------------
S_API SResult DX11Viewport::SetSize(int nX, int nY)
{
	SP_ASSERTR(m_pSwapChain && m_pEngine && m_nBackBuffers > 0, S_NOTINIT);
	SP_ASSERTR(nX > 640 && nY > 480, S_INVALIDPARAM);

	m_Desc.width = nX;
	m_Desc.height = nY;

	if (Failure(m_pSwapChain->ResizeBuffers(m_nBackBuffers, m_Desc.width, m_Desc.height, DXGI_FORMAT_UNKNOWN, 0)))
	{		
		return CLog::Log(S_ERROR, "Failed Resize Backbuffers of Viewport!");
	}

	return S_SUCCESS;
}

// -----------------------------------------------------------------------------------------------
S_API SResult DX11Viewport::EnableVSync(bool enable)
{
	return S_SUCCESS;
}

// -----------------------------------------------------------------------------------------------
S_API SVector2 DX11Viewport::GetOrthographicVolume(void)
{



	// TODO


	return SVector2();




}

// -----------------------------------------------------------------------------------------------
S_API unsigned int DX11Viewport::GetPerspectiveFOV(void)
{
	return m_Desc.projectionDesc.fov;
}

// -----------------------------------------------------------------------------------------------
S_API SResult DX11Viewport::SetProjectionByDesc(const SProjectionDesc& desc)
{
	if (!IS_VALID_PTR(m_pEngine))
		return S_NOTINIT;

	float fNearZ = desc.nearZ, fFarZ = desc.farZ;
	if (desc.bUseEngineZPlanes)
	{
		SSettingsDesc& engineSettings = m_pEngine->GetSettings()->Get();
		fNearZ = engineSettings.render.fClipNear;
		fFarZ = engineSettings.render.fClipFar;
	}

	switch (desc.projectionType)
	{
	case S_PROJECTION_PERSPECTIVE:
		SPMatrixPerspectiveFovRH(
			&m_ProjectionMtx,
			SP_DEG_TO_RAD(desc.fov),
			m_DXViewportDesc.Width / m_DXViewportDesc.Height,
			fNearZ, fFarZ);		

		/*EngLog(S_DEBUG, m_pEngine, "Set up perspective projection: fov=%f, w=%f, h=%f, n=%f, f=%f",
			SP_DEG_TO_RAD(desc.fov), m_DXViewportDesc.Width, m_DXViewportDesc.Height, fNearZ, fFarZ);*/

		break;

	case S_PROJECTION_ORTHOGRAPHIC:
		SPMatrixOrthoRH(&m_ProjectionMtx,
			desc.orthoW, desc.orthoH,
			fNearZ, fFarZ);
		break;

	default:
		CLog::Log(S_ERROR, "Invalid projection type (value = %d) given in DX11Viewport::SetProjectionByDesc()", desc.projectionType);
		return S_INVALIDPARAM;
	}

	m_Desc.projectionDesc = desc;
	m_Desc.projectionDesc.nearZ = fNearZ;
	m_Desc.projectionDesc.farZ = fFarZ;	

	return S_SUCCESS;
}


// -----------------------------------------------------------------------------------------------
S_API SProjectionDesc DX11Viewport::GetProjectionDesc() const
{
	return m_Desc.projectionDesc;
}

// -----------------------------------------------------------------------------------------------
S_API const SMatrix4& DX11Viewport::GetProjectionMatrix() const
{
	return m_ProjectionMtx;
}

// -----------------------------------------------------------------------------------------------
S_API SMatrix& DX11Viewport::GetCameraViewMatrix()
{
	return m_pCamera->viewMatrix;
}

// -----------------------------------------------------------------------------------------------
S_API SResult DX11Viewport::RecalculateCameraViewMatrix()
{
	m_pCamera->RecalculateViewMatrix();
	return S_SUCCESS;
}

// -----------------------------------------------------------------------------------------------
S_API IFBO* DX11Viewport::GetBackBuffer(void)
{
	return (IFBO*)m_pFBO;
}


SP_NMSPACE_END
