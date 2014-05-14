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
#include <Util\SMatrix.h>
#include <SSpeedPointEngine.h>
#include <Abstract\IRenderer.h>

SP_NMSPACE_BEG

// -------------------------------------------------------------------
S_API DirectX11Viewport::DirectX11Viewport()
	: m_pEngine(0),
	m_pRenderer(0),
	m_pRenderTarget(0),
	m_hWnd(0),
	m_bIsAdditional(false),
	m_pDepthStencilBuffer(0),
	m_pDepthStencilState(0),
	m_pDepthStencilView(0),
	m_pSwapChain(0)
{
}

// -------------------------------------------------------------------
S_API DirectX11Viewport::~DirectX11Viewport()
{
	// make sure to clear
	Clear();
}

// -------------------------------------------------------------------
S_API SResult DirectX11Viewport::Initialize(SpeedPointEngine* pEngine, bool bIsAdditional)
{
	SP_ASSERTR(pEngine, S_INVALIDPARAM);
	
	Clear(); // make sure to clear before initializing again

	m_pEngine = pEngine;

	IRenderer* pRenderer = pEngine->GetRenderer();
	SP_ASSERTR(pRenderer->GetType() == S_DIRECTX11, S_INVALIDPARAM);

	m_pRenderer = (DirectX11Renderer*)pRenderer;
	m_bIsAdditional = bIsAdditional;

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


	// Create the Buffer texture

	D3D11_TEXTURE2D_DESC dsTexDesc;	
	memset((void*)&dsTexDesc, 0, sizeof(D3D11_TEXTURE2D_DESC));
	dsTexDesc.ArraySize = 1; // one depth buffer is probably enough
	dsTexDesc.BindFlags = D3D11_BIND_DEPTH_STENCIL;
	dsTexDesc.CPUAccessFlags = 0;
	dsTexDesc.Format = DXGI_FORMAT_D24_UNORM_S8_UINT;	// you might want to change this format someday
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


	// Setup the proper depth testing

	D3D11_DEPTH_STENCIL_DESC depthStencilDesc;
	depthStencilDesc.DepthEnable = true; // we probably won't change this, so this if fixed
	depthStencilDesc.DepthFunc = D3D11_COMPARISON_LESS;
	depthStencilDesc.DepthWriteMask = D3D11_DEPTH_WRITE_MASK_ALL;
	
	depthStencilDesc.StencilEnable = true;
	depthStencilDesc.StencilReadMask = 0xff;
	depthStencilDesc.StencilWriteMask = 0xff;
	
	depthStencilDesc.FrontFace.StencilFailOp = D3D11_STENCIL_OP_KEEP;
	depthStencilDesc.FrontFace.StencilDepthFailOp = D3D11_STENCIL_OP_INCR;
	depthStencilDesc.FrontFace.StencilPassOp = D3D11_STENCIL_OP_KEEP;
	depthStencilDesc.FrontFace.StencilFunc = D3D11_COMPARISON_ALWAYS;
	
	depthStencilDesc.BackFace.StencilFailOp = D3D11_STENCIL_OP_KEEP;
	depthStencilDesc.BackFace.StencilDepthFailOp = D3D11_STENCIL_OP_DECR;
	depthStencilDesc.BackFace.StencilPassOp = D3D11_STENCIL_OP_KEEP;
	depthStencilDesc.BackFace.StencilFunc = D3D11_COMPARISON_ALWAYS;

	if (Failure(pD3DDevice->CreateDepthStencilState(&depthStencilDesc, &m_pDepthStencilState)))
	{
		return m_pEngine->LogE("Failed create depth stencil state!");
	}
	pD3DDeviceContext->OMSetDepthStencilState(m_pDepthStencilState, 1);


	// 

	D3D11_DEPTH_STENCIL_VIEW_DESC depthStencilViewDesc;
}

// -------------------------------------------------------------------
S_API SResult DirectX11Viewport::Clear(void)
{	
	SP_SAFE_RELEASE(m_pDepthStencilState);
	SP_SAFE_RELEASE(m_pDepthStencilView);
	SP_SAFE_RELEASE(m_pDepthStencilBuffer);	

	SP_SAFE_RELEASE(m_pRenderTarget);	
	SP_SAFE_RELEASE(m_pSwapChain);

	m_pEngine = 0;
	m_pRenderer = 0;
	m_pRenderTarget = 0;
	m_pSwapChain = 0;
	m_hWnd = 0;
	m_pDepthStencilBuffer = 0;
	m_pDepthStencilState = 0;
	m_pDepthStencilView = 0;

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

}

// -------------------------------------------------------------------
S_API SResult DirectX11Viewport::SetSize(int nX, int nY)
{
}

// -------------------------------------------------------------------
S_API SVector2 DirectX11Viewport::GetOrthographicVolume(void)
{

}

// -------------------------------------------------------------------
S_API float DirectX11Viewport::GetPerspectiveFOV(void)
{

}

// -------------------------------------------------------------------
S_API SResult DirectX11Viewport::Set3DProjection(S_PROJECTION_TYPE type, float fPerspDegFOV, float fOrthoW, float fOrthoH)
{

}

// -------------------------------------------------------------------
S_API SMatrix4 DirectX11Viewport::GetProjectionMatrix()
{

}

// -------------------------------------------------------------------
S_API SResult DirectX11Viewport::RecalculateCameraViewMatrix(SCamera* tempCam)
{
}

// -------------------------------------------------------------------
S_API SMatrix4 DirectX11Viewport::GetCameraViewMatrix()
{

}

// -------------------------------------------------------------------
S_API IFBO* DirectX11Viewport::GetBackBuffer(void)
{
}

// -------------------------------------------------------------------
S_API SResult DirectX11Viewport::SetCamera(SCamera* pCamera)
{

}

// -------------------------------------------------------------------
S_API SCamera* DirectX11Viewport::GetCamera(void)
{

}


SP_NMSPACE_END