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
#include <Util\SMatrix.h>

SP_NMSPACE_BEG

// forward declarations
class S_API SSpeedPointEngine;
class S_API SpeedPointEngine;
class S_API DirectX11Renderer;
class S_API SCamera;	

// DirectX11 implementation of the viewport interface
class S_API DirectX11Viewport : public IViewport
{
private:
	SSpeedPointEngine* m_pEngine;
	DirectX11Renderer* m_pRenderer;	// the handling rendering
	
	IDXGISwapChain* m_pSwapChain;	// the corresponding swap chain		
	ID3D11RenderTargetView* m_pRenderTarget;	

	ID3D11DepthStencilState* m_pDepthStencilState;

	ID3D11Texture2D* m_pDepthStencilBuffer;
	ID3D11DepthStencilState* m_pDepthStencilState;
	ID3D11DepthStencilView* m_pDepthStencilView;	

	HWND m_hWnd;
	bool m_bIsAdditional;

public:		
	DirectX11Viewport();
	~DirectX11Viewport();

	// DirectX11 Specific:

	IDXGISwapChain** GetSwapChainPtr() { return &m_pSwapChain; }
	ID3D11RenderTargetView** GetRTVPtr() { return &m_pRenderTarget; }

	// With DirectX11 we have to create our own Depth Buffer
	SResult InitializeDepthStencilBuffer();

	// The Render Pipeline might want to access this Depth Stencil Buffer!	
	ID3D11Texture2D* GetDepthStencilBuffer() { return m_pDepthStencilBuffer; }

	// Derived:

	virtual SResult Initialize(SpeedPointEngine* pEngine, bool bIsAdditional = true);		
	virtual SResult Clear(void);

	virtual bool IsAdditional();
		
	virtual SIZE GetSize(void);		
	virtual SResult SetSize(int nX, int nY);
		
	virtual SVector2 GetOrthographicVolume(void);
		
	virtual float GetPerspectiveFOV(void);
		
	virtual SResult Set3DProjection(S_PROJECTION_TYPE type, float fPerspDegFOV, float fOrthoW, float fOrthoH);		
	virtual SMatrix4 GetProjectionMatrix();
		
	virtual SResult RecalculateCameraViewMatrix(SCamera* tempCam);		
	virtual SMatrix4 GetCameraViewMatrix();

	virtual HWND GetWindow() { return m_hWnd;  }
	virtual void SetWindow(HWND hWnd) // does NOT update swapchain hwnd!
	{
		m_hWnd = hWnd;
	}
				
	virtual IFBO* GetBackBuffer(void);
		
	virtual SResult SetCamera(SCamera* pCamera);		
	virtual SCamera* GetCamera(void);				
};


SP_NMSPACE_END