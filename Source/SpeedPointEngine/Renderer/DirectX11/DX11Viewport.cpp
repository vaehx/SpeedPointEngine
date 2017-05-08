//////////////////////////////////////////////////////////////////////////////////////////////////
//
//	SpeedPoint Game Engine
//	Copyright (c) 2011-2017 Pascal Rosenkranz, All rights reserved.
//
//////////////////////////////////////////////////////////////////////////////////////////////////

#include "DX11Viewport.h"
#include "DX11Renderer.h"
#include "DX11FBO.h"
#include "DX11Utilities.h"
#include "..\IRenderer.h"
#include <Common\Matrix.h>

#ifdef _DEBUG
#include <dxgi1_3.h>
#endif

SP_NMSPACE_BEG

unsigned int DX11Viewport::m_SwapChainIdCtr = 0;

// -----------------------------------------------------------------------------------------------
S_API DX11Viewport::DX11Viewport()
: m_pRenderer(0),
m_pSwapChain(0)
{
	m_pCamera = &m_OwnCamera;
}

// -----------------------------------------------------------------------------------------------
S_API DX11Viewport::~DX11Viewport()
{
	// make sure to clear
	Clear();
}

// -----------------------------------------------------------------------------------------------
S_API SResult DX11Viewport::Initialize(IRenderer* pRenderer, const SViewportDescription& desc)
{
	SP_ASSERTR(IS_VALID_PTR(pRenderer), S_INVALIDPARAM);
	SP_ASSERTR(pRenderer->GetType() == S_DIRECTX11, S_INVALIDPARAM);
	SP_ASSERTR(desc.hWnd, S_INVALIDPARAM);

	Clear(); // make sure to clear before initializing again

	m_pRenderer = dynamic_cast<DX11Renderer*>(pRenderer);
	m_Desc = desc;


	// Setup viewport desc
	m_DXViewportDesc.Width = (float)desc.width;
	m_DXViewportDesc.Height = (float)desc.height;
	m_DXViewportDesc.MinDepth = 0;
	m_DXViewportDesc.MaxDepth = 1.0f;
	m_DXViewportDesc.TopLeftX = 0.0f;
	m_DXViewportDesc.TopLeftY = 0.0f;


	// Setup swap chain description
	DXGI_SWAP_CHAIN_DESC swapChainDesc;
	swapChainDesc.BufferCount = 1;
	swapChainDesc.Windowed = desc.windowed;

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
		desc.antiAliasingCount,
		desc.antiAliasingQuality);
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


	// Get buffer resource of swap chain and initialize FBO
	ID3D11Resource* pBBResource;
	if (Failure(m_pSwapChain->GetBuffer(0, __uuidof(pBBResource), reinterpret_cast<void**>(&pBBResource))))
		return CLog::Log(S_ERROR, "Failed retrieve BackBuffer resource of SwapChain in InitDefaultViewport!");

	string textureSpec = string("$swapchain_") + std::to_string(m_SwapChainIdCtr++);

	if (Failure(m_FBO.D3D11_InitializeFromCustomResource(pBBResource, m_pRenderer,
		swapChainDesc.BufferDesc.Width, swapChainDesc.BufferDesc.Height, m_Desc.allowAsTexture, textureSpec)))
	{
		return CLog::Log(S_ERROR, "Failed initialize FBO for viewport swapchain");
	}

	if (m_Desc.useDepthStencil)
	{
		SResult res;
		if (m_Desc.allowDepthAsTexture)
			res = m_FBO.InitializeDepthBufferAsTexture(textureSpec + "_depth");
		else
			res = m_FBO.InitializeDepthBuffer();

		if (Failure(res))
			return CLog::Log(S_ERROR, "Failed initialize depth buffer for viewport swapchain");
	}

	pBBResource->Release();


	// Setup camera
	m_pCamera = &m_OwnCamera;


	// Setup projection
	if (Failure(SetProjectionByDesc(m_Desc.projectionDesc)))
		return S_ERROR;


	return S_SUCCESS;
}

// -----------------------------------------------------------------------------------------------
S_API SResult DX11Viewport::Clear(void)
{		
	m_FBO.Clear();

	SP_SAFE_RELEASE(m_pSwapChain);

	m_pRenderer = 0;
	m_Desc.hWnd = 0;
	m_pCamera = 0;

	return S_SUCCESS;
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
S_API SResult DX11Viewport::SetSize(unsigned int width, unsigned int height)
{
	SP_ASSERTR(m_pSwapChain, S_NOTINIT);

	if (m_Desc.width == width && m_Desc.height == height)
		return S_SUCCESS;

	m_DXViewportDesc.Width = (float)width;
	m_DXViewportDesc.Height = (float)height;

	m_Desc.width = width;
	m_Desc.height = height;

	// Resize swap chain backbuffer
	if (Failure(m_pSwapChain->ResizeBuffers(1, m_Desc.width, m_Desc.height, DXGI_FORMAT_UNKNOWN, 0)))
		return CLog::Log(S_ERROR, "Failed Resize Backbuffers of Viewport!");

	// Recreate FBO
	m_FBO.Clear();

	ID3D11Resource* pBBResource;
	if (Failure(m_pSwapChain->GetBuffer(0, __uuidof(pBBResource), reinterpret_cast<void**>(&pBBResource))))
		return CLog::Log(S_ERROR, "Failed retrieve BackBuffer resource of SwapChain in InitDefaultViewport!");

	string textureSpec = string("$swapchain_") + std::to_string(m_SwapChainIdCtr++);

	if (Failure(m_FBO.D3D11_InitializeFromCustomResource(pBBResource, m_pRenderer, width, height, m_Desc.allowAsTexture, textureSpec)))
		return CLog::Log(S_ERROR, "Failed initialize FBO for viewport swapchain");

	if (m_Desc.useDepthStencil)
	{
		SResult res;
		if (m_Desc.allowDepthAsTexture)
			res = m_FBO.InitializeDepthBufferAsTexture(textureSpec + "_depth");
		else
			res = m_FBO.InitializeDepthBuffer();

		if (Failure(res))
			return CLog::Log(S_ERROR, "Failed initialize depth buffer for viewport swapchain");
	}

	pBBResource->Release();


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
	float	fNearZ = desc.nearZ,
			fFarZ = desc.farZ;

	switch (desc.projectionType)
	{
	case S_PROJECTION_PERSPECTIVE:
		SPMatrixPerspectiveFovRH(
			&m_ProjectionMtx,
			SP_DEG_TO_RAD(desc.fov),
			m_DXViewportDesc.Width / m_DXViewportDesc.Height,
			fNearZ, fFarZ);		

		/*CLog::Log(S_DEBUG, m_pEngine, "Set up perspective projection: fov=%f, w=%f, h=%f, n=%f, f=%f",
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
	return &m_FBO;
}


SP_NMSPACE_END
