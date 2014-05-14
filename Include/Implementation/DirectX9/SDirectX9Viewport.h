// ******************************************************************************************

//	This file is part of the SpeedPoint Game Engine

// ******************************************************************************************

#pragma once
#include <SPrerequisites.h>
#include <Abstract\IViewport.h>
#include <Util\SVector2.h>
#include <d3d9.h>
#include <d3dx9.h>
#include <Util\SMatrix.h>

namespace SpeedPoint
{		
	// DirectX9 specific implementation of the SpeedPoint Viewport (IViewport abstr)
	class S_API SDirectX9Viewport : public IViewport
	{
	private:
		bool			m_bIsAdditional;

	public:
		SCamera*		pCamera;
		SpeedPointEngine*	pEngine;
		D3DPRESENT_PARAMETERS	d3dPresentParameters;
		LPDIRECT3DSWAPCHAIN9	pSwapChain;			// In case this is an addition dx viewport
		HWND			hWnd;
		D3DDISPLAYMODE		d3dDisplayMode;
		LPDIRECT3DSURFACE9	pBackBuffer;
		SMatrix			mProjection, mView;
		int			nXResolution, nYResolution;
		float			fOrthoW, fOrthoH;
		float			fFOV;		

		// Default constructor
		SDirectX9Viewport()
			: fOrthoW(20.0f),
			fOrthoH(20.0f),
			fFOV(100.0f),
			pCamera(NULL),
			pEngine(NULL),
			hWnd(NULL),
			pBackBuffer(NULL),
			pSwapChain(NULL),
			m_bIsAdditional(false)
		{
		}

		// Copy constructor
		SDirectX9Viewport(const SDirectX9Viewport& o)
			: fOrthoW(o.fOrthoW),
			fOrthoH(o.fOrthoH),
			fFOV(o.fFOV),
			pCamera(o.pCamera),
			pEngine(o.pEngine),
			hWnd(o.hWnd),
			pBackBuffer(o.pBackBuffer),
			pSwapChain(o.pSwapChain),
			m_bIsAdditional(o.m_bIsAdditional)
		{
		}

		// Initialize this viewport with an Engine instance
		virtual SResult Initialize(SpeedPointEngine* pEngine, bool bIsAdditional = true);

		// check whether this viewport is an additional one
		virtual bool IsAddition();

		// Get the size of this viewport
		virtual SIZE GetSize( void );

		// Set the size of this viewport
		virtual SResult SetSize( int nX, int nY );

		// Get the orthographic view volume size
		virtual SVector2 GetOrthographicVolume( void );

		// Get the Field Of View of the perspective matrix
		virtual float GetPerspectiveFOV( void );

		// Recalculate the projection matrix of this viewport
		virtual SResult Set3DProjection( S_PROJECTION_TYPE type, float fPerspDegFOV, float fOrthoW, float fOrthoH );

		// Get the Projection matrix of this viewport
		virtual SMatrix4 GetProjectionMatrix();

		// Recalculate view matrix of the camera of this viewport
		virtual SResult RecalculateCameraViewMatrix(SCamera* tempCam);

		// Get view matrix of the camera of this viewport
		virtual SMatrix4 GetCameraViewMatrix();

		/////// TODO: More viewport related settings

		virtual HWND GetWindow() = 0;
		virtual void SetWindow(HWND hWnd) = 0;

		// Get a pointer to the backbuffer framebuffer object
		virtual IFrameBufferObject* GetBackBuffer( void );

		// Set the pointer of the camera.
		virtual SResult SetCamera( SCamera* pCamera );

		// Get the pointer to the camera
		virtual SCamera* GetCamera( void );

		// Clearout the backbuffer + viewport
		virtual SResult Clear( void );
	};
}