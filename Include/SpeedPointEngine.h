// ******************************************************************************************

//	SpeedPoint Engine Class

// ******************************************************************************************

#pragma once

#include "Abstract\SResourcePool.h"
#include "SPrerequisites.h"
#include "SSettings.h"
#include "SCamera.h"
#include "Util\SLogStream.h"

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

		// Basic Solid System to store solids
		SSolidSystem*	pSolidSystem;		

		// Frame delay counter
		DWORD		dwBeforeFrame;
		DWORD		dwAfterFrame;
		float		fLastFrameDelay;		

		// Basic Logging Stream
		bool		bCustomLoggingStream;
		SLogStream*	pLoggingStream;		

	public:	

		// Default constructor
		SpeedPointEngine()
			: bRunning( false ),
			pRenderer( NULL ),			
			pSolidSystem( NULL ),
			pLightSystem( NULL ),
			pViewports( NULL ),
			pDefaultViewport( NULL ),			
			pResourcePool( NULL ),
			dwBeforeFrame( 0 ),
			dwAfterFrame( 0 ),
			fLastFrameDelay( 0 ),
			bCustomLoggingStream( false ),
			pLoggingStream( NULL )
		{
		};

		// --

		// Startup the whole thing
		SResult Start( const SSettings& settings );

		// Set custom logging stream
		SResult SetCustomLogStream( SLogStream* pLogStream );

		// Report a log entry and return the result given as parameter
		SResult LogReport( SResult res, SString msg );

		// Get the Renderer
		SRenderer* GetRenderer( void );

		// Get the settings
		SSettings* GetSettings( void );

		// Get the Renderer Resourcepool
		SResourcePool* GetResourcePool( void );

		// Create a new solid
		SP_ID AddSolid( void );

		// Create a new solid and immediately get the solid pointer
		SSolid* AddSolid( SP_ID* pUID );

		// Get a solid by its id
		SSolid* GetSolid( const SP_ID& id );

		// Begin a frame. Sets first position of frame delay counter
		SResult BeginFrame( void );

		// Render a solid
		SResult RenderSolid( SP_ID iSolid );

		// Add a new Viewport and return static pointer to it
		SViewport* AddAdditionalViewport( void );

		// Get a viewport by its id
		SViewport* GetViewport( UINT index );

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