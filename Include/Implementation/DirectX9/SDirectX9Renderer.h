// ******************************************************************************************

//	SpeedPoint DirectX 9 Renderer

// ******************************************************************************************

#pragma once
#include <SPrerequisites.h>
#include <Abstract\SRenderer.h>
#include "SDirectX9Settings.h"
#include "SDirectX9Viewport.h"
#include <d3d9.h>

namespace SpeedPoint
{




/// TODO: VIRTUALS ??????





	// SpeedPoint DirectX 9 Renderer
	class S_API SDirectX9Renderer : public SRenderer
	{
	public:
		SpeedPointEngine*	pEngine;
		SDirectX9Settings	setSettings;
		LPDIRECT3D9		pDirect3D;
		LPDIRECT3DDEVICE9	pd3dDevice;
		D3DCAPS9		cpsDeviceCaps;
		D3DDISPLAYMODE		m_AutoSelectedDisplayMode;	// automatically selected in AutoSelectAdapter()
		SRenderPipeline*	pRenderPipeline;
		SDirectX9Viewport	vpViewport;

		// Default constructor
		SDirectX9Renderer() :
			pEngine( NULL ),
			pDirect3D( NULL ),
			pd3dDevice( NULL ),
			pRenderPipeline( NULL ) {};

		// --

		SResult AutoSelectAdapter( int nW, int nH );

		SResult InitDefaultViewport( HWND hWnd, int nW, int nH, DWORD * dwBeheaviourFlags );
		
		SResult CreateDX9Device( DWORD * dwBeheaviourFlags );
		
		SResult CollectDeviceCaps( void );
		
		SResult SetRenderStateDefaults( void );


		// Derived:

		S_RENDERER_TYPE GetType( void );
		
		SResult Initialize( SpeedPointEngine* pEngine, HWND hWnd, int nW, int nH, bool bIgnoreAdapter );
		
		SResult CreateAdditionalViewport( SViewport** pViewport );
		
		SResult Shutdown( void );
		
		bool IsInited( void );
		
		SRenderPipeline* GetRenderPipeline( void );
		
		SResult BeginScene( void );
		
		SResult EndScene( void );
		
		SResult SetTargetViewport( SViewport* pViewport );
		
		SViewport* GetTargetViewport( void );
		
		SViewport* GetDefaultViewport( void );
		
		SResult UpdateViewportMatrices( SViewport* pViewport );
		
		SResult RenderSolid(SSolid* pSolid, bool bTextured);
	};
}