// ******************************************************************************************

//	This is the main header of the SpeedPoint Engine

//	(c) 2013, Pascal Rosenkranz -- All rights reserved.
//	- Current Dev Version: 2.0
//	- Current Beta Version: 0.2
//	- Current Release Version: 0.0.2

// ******************************************************************************************

#pragma once

#include "Abstract\SResourcePool.h"
#include "SPrerequisites.h"
#include "SSettings.h"
#include "SCamera.h"

#define SP_MAX_VIEWPORTS 10

namespace SpeedPoint
{
	// The SpeedPoint Engine
	class S_API SpeedPointEngine
	{
	private:
		bool		bRunning;

		SSettings	sSettings;		
		SRenderer*	pRenderer;		
		SCamera		camCamera;
		SViewport**	pViewports;
		SViewport*	pDefaultViewport;		

		// Resource pool for renderer specific instances
		SResourcePool*	pResourcePool;

		// Lighting System
		SLightSystem*	pLightSystem;

		// Frame delay counter
		DWORD		dwBeforeFrame;
		DWORD		dwAfterFrame;
		float		fLastFrameDelay;		

	public:
		// Default constructor
		SpeedPointEngine()
			: pRenderer( NULL ),
			pViewports( NULL ),
			pDefaultViewport( NULL ),			
			pResourcePool( NULL ),
			dwBeforeFrame( 0 ),
			dwAfterFrame( 0 ),
			fLastFrameDelay( 0 ) {};

		// --

		// Startup the whole thing
		SResult Start( const SSettings& settings );		

		// Get the Renderer
		SRenderer* GetRenderer( void );

		// Get the settings
		SSettings* GetSettings( void );

		// Get the resourcepool
		SResourcePool* GetResourcePool( void );

		// Begin a frame. Sets first position of frame delay counter
		SResult BeginFrame( void );

		// Render a solid
		SResult RenderSolid( SSolid* pSolid );

		// Present the backbuffer of the given viewport
		SResult FlushViewport( SViewport* pViewport );

		// Present the current targetviewport. By default this is the default viewport
		SResult Flush( void );

		// End a frame
		SResult EndFrame( float* fFrameDelay );

		// Stop and clearout everything this engine instance stores
		SResult Shutdown( void );

		// Hook a lighting System
		SResult HookLightingSystem( SLightSystem* pSystem );
	};


}