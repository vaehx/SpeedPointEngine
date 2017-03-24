//////////////////////////////////////////////////////////////////////////////////////////////////
//
//	SpeedPoint Game Engine
//	Copyright (c) 2011-2016 Pascal Rosenkranz, All rights reserved.
//
//////////////////////////////////////////////////////////////////////////////////////////////////

#pragma once

#include "..\IViewport.h"
#include <Common\Matrix.h>
#include <Common\SPrerequisites.h>
#include "DX11.h"

SP_NMSPACE_BEG

// forward declarations
class S_API DX11Renderer;
class S_API DX11FBO;

// DirectX11 implementation of the viewport interface
class S_API DX11Viewport : public IViewport
{
private:
	DX11Renderer* m_pRenderer;	// the handling rendering
	
	DX11FBO* m_pFBO;

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
	DX11Viewport();
	~DX11Viewport();

	// DirectX11 Specific:

	IDXGISwapChain** GetSwapChainPtr() { return &m_pSwapChain; }
	ID3D11RenderTargetView** GetRTVPtr() { return &m_pRenderTarget; }
	D3D11_VIEWPORT* GetViewportDescPtr() { return &m_DXViewportDesc; }

	// With DirectX11 we have to create our own Depth Buffer
	SResult InitializeDepthStencilBuffer();

	// The Render Pipeline might want to access this Depth Stencil Buffer!	
	ID3D11Texture2D* GetDepthStencilBuffer() { return m_pDepthStencilBuffer; }

	// Derived:

	virtual SResult Initialize(IRenderer* pRenderer, const SViewportDescription& desc, bool bIsAdditional = true);		
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
