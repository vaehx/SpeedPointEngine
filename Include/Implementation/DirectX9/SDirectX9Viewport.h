// ******************************************************************************************

//	SpeedPoint DirectX9 Viewport

// ******************************************************************************************

#pragma once
#include <SPrerequisites.h>
#include <Abstract\SViewport.h>
#include <SVector2.h>
#include <d3d9.h>
#include <d3dx9.h>
#include <SMatrix.h>

namespace SpeedPoint
{
	// SpeedPoint DirectX9 Viewport
	class S_API SDirectX9Viewport : public SViewport
	{
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
			: fOrthoW( 20.0f ),
			fOrthoH( 20.0f ),
			fFOV( 100.0f ),
			pCamera( NULL ),
			pEngine( NULL ),
			hWnd( NULL ),
			pBackBuffer( NULL ),
			pSwapChain( NULL ) {};

		SDirectX9Viewport( const SDirectX9Viewport& o )
			: fOrthoW( o.fOrthoW ),
			fOrthoH( o.fOrthoH ),
			fFOV( o.fFOV ),
			pCamera( o.pCamera ),
			pEngine( o.pEngine ),
			hWnd( o.hWnd ),
			pBackBuffer( o.pBackBuffer ),
			pSwapChain( o.pSwapChain ) {};

		// Initialize this viewport with an Engine instance
		SResult Initialize( SpeedPointEngine* pEngine );

		// Get the size of this viewport
		SIZE GetSize( void );

		// Set the size of this viewport
		SResult SetSize( int nX, int nY );

		// Get the orthographic view volume size
		SVector2 GetOrthographicVolume( void );

		// Get the Field Of View of the perspective matrix
		float GetPerspectiveFOV( void );

		// Recalculate the projection matrix of this viewport
		SResult Set3DProjection( S_PROJECTION_TYPE type, float fPerspDegFOV, float fOrthoW, float fOrthoH );

		// Recalculate the view matrix of this viewport
		SResult RecalculateViewMatrix( SCamera* tempCam );

		/////// TODO: More viewport related settings

		// Get a pointer to the backbuffer framebuffer object
		SFrameBuffer* GetBackBuffer( void );

		// Set the pointer of the camera.
		SResult SetCamera( SCamera* pCamera );

		// Get the pointer to the camera
		SCamera* GetCamera( void );

		// Clearout the backbuffer + viewport
		SResult Clear( void );
	};
}