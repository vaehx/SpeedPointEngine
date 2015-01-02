//////////////////////////////////////////////////////////////////////////////////////////////////
//
//	This file is part of the SpeedPoint Game Engine
//
//	written by Pascal R. aka iSmokiieZz
//	(c) 2011-2014, All rights reserved.
//
//////////////////////////////////////////////////////////////////////////////////////////////////

#pragma once

#include <SPrerequisites.h>
#include <Abstract\IViewport.h>
#include "DirectX11.h"
#include <Abstract\Matrix.h>

SP_NMSPACE_BEG

// forward declarations
struct S_API IGameEngine;
class S_API DirectX11Renderer;
class S_API DirectX11FBO;

// DirectX11 implementation of the viewport interface
class S_API DirectX11Viewport : public IViewport
{
private:
	IGameEngine* m_pEngine;
	DirectX11Renderer* m_pRenderer;	// the handling rendering
	
	DirectX11FBO* m_pFBO;

	IDXGISwapChain* m_pSwapChain;	// the corresponding swap chain		
	ID3D11RenderTargetView* m_pRenderTarget;		

	ID3D11Texture2D* m_pDepthStencilBuffer;
	ID3D11DepthStencilView* m_pDepthStencilView;	

	D3D11_VIEWPORT m_DXViewportDesc;	
	bool m_bIsAdditional;

	SViewportDescription m_Desc;
	unsigned int m_nBackBuffers;

	SMatrix m_ProjectionMtx;

	SCamera m_OwnCamera;
	SCamera* m_pCamera;

public:		
	DirectX11Viewport();
	~DirectX11Viewport();

	// DirectX11 Specific:

	IDXGISwapChain** GetSwapChainPtr() { return &m_pSwapChain; }
	ID3D11RenderTargetView** GetRTVPtr() { return &m_pRenderTarget; }
	D3D11_VIEWPORT* GetViewportDescPtr() { return &m_DXViewportDesc; }

	// With DirectX11 we have to create our own Depth Buffer
	SResult InitializeDepthStencilBuffer();

	// The Render Pipeline might want to access this Depth Stencil Buffer!	
	ID3D11Texture2D* GetDepthStencilBuffer() { return m_pDepthStencilBuffer; }

	// Derived:

	virtual SResult Initialize(IGameEngine* pEngine, const SViewportDescription& desc, bool bIsAdditional = true);		
	virtual SResult Clear(void);

	virtual bool IsAdditional();
		
	virtual SIZE GetSize(void);		
	virtual SResult SetSize(int nX, int nY);

	virtual SResult EnableVSync(bool enable = true);
		
	virtual SVector2 GetOrthographicVolume(void);
		
	virtual unsigned int GetPerspectiveFOV(void);	
		
	virtual SResult SetProjectionByDesc(const SProjectionDesc& desc);
	virtual SProjectionDesc GetProjectionDesc() const;
	virtual const SMatrix4& GetProjectionMatrix() const;

	virtual SResult RecalculateCameraViewMatrix();		
	virtual SMatrix4& GetCameraViewMatrix();

	virtual HWND GetWindow() { return m_Desc.hWnd;  }
	virtual void SetWindow(HWND hWnd) // does NOT update swapchain hwnd!
	{
		m_Desc.hWnd = hWnd;
	}
				
	virtual IFBO* GetBackBuffer(void);
		
	virtual SResult SetCamera(SCamera* pCamera)
	{
		if (!IS_VALID_PTR(pCamera))
			return S_INVALIDPARAM;	
			
		m_pCamera = pCamera;
		return S_SUCCESS;
	}
	virtual SCamera* GetCamera(void)
	{
		return m_pCamera;
	}
	virtual SCamera* GetDefaultCamera()
	{
		return &m_OwnCamera;
	}
};


SP_NMSPACE_END