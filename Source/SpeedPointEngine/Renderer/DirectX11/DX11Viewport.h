//////////////////////////////////////////////////////////////////////////////////////////////////
//
//	SpeedPoint Game Engine
//	Copyright (c) 2011-2016 Pascal Rosenkranz, All rights reserved.
//
//////////////////////////////////////////////////////////////////////////////////////////////////

#pragma once

#include "..\IViewport.h"
#include "DX11FBO.h"
#include <Common\Matrix.h>
#include <Common\SPrerequisites.h>
#include "DX11.h"

SP_NMSPACE_BEG

// forward declarations
class S_API DX11Renderer;

// DirectX11 implementation of the viewport interface
class S_API DX11Viewport : public IViewport
{
private:
	DX11Renderer* m_pRenderer;	// the handling rendering
	
	IDXGISwapChain* m_pSwapChain;	// the corresponding swap chain
	DX11FBO m_FBO;

	D3D11_VIEWPORT m_DXViewportDesc; // used in RSSetViewports

	SViewportDescription m_Desc;

	SMatrix m_ProjectionMtx;

	SCamera m_OwnCamera;
	SCamera* m_pCamera;

public:		
	DX11Viewport();
	~DX11Viewport();

	IDXGISwapChain** D3D11_GetSwapChainPtr() { return &m_pSwapChain; }
	D3D11_VIEWPORT* D3D11_GetViewportDescPtr() { return &m_DXViewportDesc; }

	// IViewport:
public:
	virtual SResult Initialize(IRenderer* pRenderer, const SViewportDescription& desc);
	virtual SResult Clear(void);

	virtual SIZE GetSize(void);		
	virtual SResult SetSize(unsigned int height, unsigned int width);

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
