// ********************************************************************************************

// SpeedPoint Engine

// ********************************************************************************************

#include <SpeedPoint.h>
#include <Abstract\SRenderPipeline.h>

//~~~~~~~~~
// TODO: Try to eliminate implementations from the SCore Project
#include <Implementation\DirectX9\SDirectX9Renderer.h>
#include <Implementation\DirectX9\SDirectX9ResourcePool.h>
//~~~~~~~~~

#include <Lighting\SLightSystem.h>
#include <Abstract\SSolidSystem.h>

//~~~~~~~~~
// TODO: Try to eliminate implementations from the SCore Project
#include <Implementation\Geometry\SBasicSolidSystem.h>
//~~~~~~~~~~

//#include <Implementation\DirectX10\SDirectX10Renderer.h>
//#include <Implementation\DirectX11\SDirectX11Renderer.h>
//#include <Implementation\OpenGL\SOpenGLRenderer.h>

namespace SpeedPoint
{
	// ********************************************************************************************

	S_API SResult SpeedPointEngine::Start( const SSettings& settings )
	{
		if( bRunning ) return S_ABORTED;

		if( settings.hWnd == NULL ) return S_ABORTED;

		if( settings.nXResolution < 600 || settings.nYResolution < 400 ) return S_ABORTED;		

		sSettings = settings;

		// Initialize default logging stream
		if( !bCustomLoggingStream )
			pLoggingStream = new SLogStream();
	
		// Initialize the renderer and its resource pool
		if( settings.tyRendererType == S_DIRECTX9 )
		{
			pRenderer = (SRenderer*)new SDirectX9Renderer();
			if( Failure( pRenderer->Initialize( this, settings.hWnd, settings.nXResolution, settings.nYResolution, false ) ) )
			{
				return LogReport( S_ERROR, "Failed initialize renderer" );
			}

			pResourcePool = (SResourcePool*)new SDirectX9ResourcePool();
			if( Failure( pResourcePool->Initialize( this, pRenderer ) ) )
			{				
				return LogReport( S_ERROR, "Failed initialize Resource Pool" );
			}

			// Initialize the viewports array
			pViewports = (SViewport**)( malloc( sizeof( SViewport* ) * SP_MAX_VIEWPORTS ) );			
			if( pViewports == NULL )
			{
				return LogReport( S_ERROR, "Failed Initialize viewport storage" );
			}

			ZeroMemory( pViewports, sizeof( SViewport* ) * SP_MAX_VIEWPORTS );

			// Set the default viewport of the renderer as the zero-viewport
			pViewports[0] = pRenderer->GetDefaultViewport();
		}
		else
		{
			return LogReport( S_ABORTED, "This Renderer type is not supported yet" );
		}

		// Initialize the default viewport
		pViewports[0]->SetCamera( &camCamera );

		// Initialize the solid system
		pSolidSystem = (SSolidSystem*)new SBasicSolidSystem();
		pSolidSystem->Initialize( this );

		bRunning = true;
		return S_SUCCESS;
	}

	// ********************************************************************************************

	S_API SResult SpeedPointEngine::SetCustomLogStream( SLogStream* pLogStream )
	{
		if( pLogStream == NULL || pLoggingStream == pLogStream ) return S_ABORTED;

		// Destroy old Logging stream instance if default log stream
		if( !bCustomLoggingStream && pLoggingStream != NULL )
			delete pLoggingStream;					

		bCustomLoggingStream = true;

		pLoggingStream = pLogStream;
		LogReport( S_INFO, "Switched Logging Stream successfully" );

		return S_SUCCESS;
	}

	// ********************************************************************************************
	
	S_API SResult SpeedPointEngine::LogReport( SResult res, SString msg )
	{
		if( pLoggingStream == NULL ) return S_ABORTED;

		SString sActualMsg;

		char* cPrefix;
		switch( sSettings.tyRendererType )
		{
		case S_DIRECTX9: cPrefix = "[SpeedPoint:DX9]"; break;
		case S_DIRECTX10: cPrefix = "[SpeedPoint:DX10]"; break;
		case S_DIRECTX11: cPrefix = "[SpeedPoint:DX11]"; break;
		case S_OPENGL: cPrefix = "[SpeedPoint:GL]"; break;
		default: cPrefix = "[SpeedPoint:??]"; break;
		}
		
		sActualMsg.cString = new char[ msg.GetLength() + strlen( cPrefix ) + 2 ];
		ZeroMemory( sActualMsg.cString, msg.GetLength() + strlen( cPrefix ) + 2 );
		wsprintf( sActualMsg.cString, "%s %s", cPrefix, msg.cString );

		return pLoggingStream->Report( res, sActualMsg );
	}

	// ********************************************************************************************
	
	S_API SRenderer* SpeedPointEngine::GetRenderer( void )
	{
		return pRenderer;
	}

	// ********************************************************************************************

	S_API SSettings* SpeedPointEngine::GetSettings( void )
	{
		return &sSettings;
	}

	// ********************************************************************************************

	S_API SResourcePool* SpeedPointEngine::GetResourcePool( void )
	{
		return pResourcePool;
	}

	// ********************************************************************************************

	S_API SViewport* SpeedPointEngine::AddAdditionalViewport( void )
	{
		if( pViewports == NULL ) return NULL;

		// Check for a free viewport slot
		UINT iFreeSlot = 99999999;
		for( UINT i = 0; i < SP_MAX_VIEWPORTS; i++ )
		{
			if( pViewports[i] == NULL )
			{
				iFreeSlot = i;
				break;
			}
		}

		if( iFreeSlot > SP_MAX_VIEWPORTS )
		{
			LogReport( S_ERROR, "Cannot add additional Viewport: No free slot!" );
		}

		if( Failure( pRenderer->CreateAdditionalViewport( &pViewports[iFreeSlot] ) ) )
			LogReport( S_ERROR, "Failed add addition viewport!" );

		return pViewports[iFreeSlot];
	}

	// ********************************************************************************************

	S_API SViewport* SpeedPointEngine::GetViewport( UINT index )
	{
		if( pViewports == NULL ) return NULL;

		return pViewports[index];
	}

	// ********************************************************************************************

	S_API SP_ID SpeedPointEngine::AddSolid( void )
	{
		if( pSolidSystem == NULL ) return SP_ID();

		return pSolidSystem->AddSolid();
	}

	// ********************************************************************************************

	S_API SSolid* SpeedPointEngine::AddSolid( SP_ID* pUID )
	{
		if( pSolidSystem == NULL ) return NULL;

		SP_ID id = pSolidSystem->AddSolid();
		
		if( pUID ) *pUID = id;

		return pSolidSystem->GetSolid( id );
	}

	// ********************************************************************************************

	S_API SSolid* SpeedPointEngine::GetSolid( const SP_ID& id )
	{
		if( pSolidSystem == NULL ) return NULL;

		return pSolidSystem->GetSolid( id );
	}

	// ********************************************************************************************

	S_API SResult SpeedPointEngine::BeginFrame( void )
	{
		if( pRenderer == NULL || !bRunning ) return S_ABORTED;

/////// TODO: Add Commandbuffer management here
//////		This means we add a new task here to begin a new frame section inside the rendering pipeline
/////		But for now, the simple call should be enough			

		// BEGINFRAME = Begin the scene, clear backbuffer, switch state
		if( Failure( pRenderer->GetRenderPipeline()->BeginFrameSection() ) )
		{
			return S_ERROR;
		}

		return S_SUCCESS;
	}

	// ********************************************************************************************

	S_API SResult SpeedPointEngine::RenderSolid( SP_ID iSolid )
	{
		if( pRenderer == NULL || !bRunning ) return S_ABORTED;

//////// TODO: Add commandbuffer management here
////////	This means we add a new task here to render the solid.
///////		pay attention when giving the pointer: the position of the data might change
/////		For now, a simple call to the render pipeline should be enough

		SSolid* pSolid = pSolidSystem->GetSolid( iSolid );

		if (pSolid == NULL)
			return LogReport(S_ERROR, "(eng) Tried to render solid of invalid index!");

		return pRenderer->GetRenderPipeline()->RenderSolidGeometry( pSolid );
	}

	// ********************************************************************************************

	S_API SResult SpeedPointEngine::FlushViewport( SViewport* pViewport )
	{
		if( pRenderer == NULL || pViewport == NULL || !bRunning ) return S_ABORTED;

//////// TODO: Add a task to the gpu commandbuffer to present the viewport (S_RENDER_PRESENT)
//////		This will only be possible if the latest task equals S_RENDER_POST
///////		Be careful, when giving the pointer to the viewport.
///////		By now, we simply call the renderpipeline to present if in POST state

		return Flush();
	}

	// ********************************************************************************************

	S_API SResult SpeedPointEngine::Flush( void )
	{
		if( pRenderer == NULL || !bRunning ) return S_ABORTED;

///////// TODO: Add a task to the gpu commandbuffer to present the viewport that is currently the
////////	target Viewport of the Render Pipeline

///////		for now, we simply call the render pipeline to present everything	

		return pRenderer->GetRenderPipeline()->Present();
	}

	// ********************************************************************************************

	S_API SResult SpeedPointEngine::EndFrame( float* fFrameDelay )
	{	
		if( pRenderer == NULL || !bRunning ) return S_ABORTED;

///////// TODO: Add a task to the gpu commandbuffer to stop geometry rendering, render lighting and post shader
///		 and finally end frame section which includes rendering the final OutputPlane

/////		For now we simply call the methods without any parameters
		if( Failure( pRenderer->GetRenderPipeline()->StopGeometryRendering() ) )
		{
			return S_ERROR;
		}

		if( Failure( pRenderer->GetRenderPipeline()->RenderLighting( (SLight*)NULL ) ) )
		{
			return S_ERROR;
		}

		if( Failure( pRenderer->GetRenderPipeline()->RenderPostEffects( NULL ) ) )
		{
			return S_ERROR;
		}

		// EndFrameSection() also renders the Output Plane
		if( Failure( pRenderer->GetRenderPipeline()->EndFrameSection() ) )
		{
			return S_ERROR;
		}

/////// END OF TODO SECTION

		if( Failure( Flush() ) )
		{
			return S_ERROR;
		}

/////// TODO: Calculate delay between last frame till now and return the result into fFrameDelay parameter

		return S_SUCCESS;
	}

	// ********************************************************************************************

	S_API SResult SpeedPointEngine::Shutdown( void )
	{
		bRunning = false;

		// Clear Resources
		if( pResourcePool )
		{
			if( Failure( pResourcePool->ClearAll() ) ) return S_ERROR;
			delete pResourcePool;
			pResourcePool = NULL;
		}

		// Clear Viewports
		pDefaultViewport = NULL;
		for( UINT iViewport = 1; iViewport < SP_MAX_VIEWPORTS; iViewport++ )
		{
			if( pViewports[iViewport] != NULL )
			{
				pViewports[iViewport]->Clear();
				delete pViewports[iViewport];				
			}
		}
				
		memset( (void*)pViewports, 0, sizeof( SViewport* ) * SP_MAX_VIEWPORTS );
		free( pViewports );

		// Clear Solid System
		if( pSolidSystem )
		{
			pSolidSystem->Clear();
			delete pSolidSystem;
			pSolidSystem = NULL;
		}
				
		// Clear Lighting System
		if( pLightSystem )
		{
			//if( Failure( pLightSystem->Clear() ) ) return S_ERROR;
			delete pLightSystem;
			pLightSystem = NULL;
		}		

		// Clear Renderer
		if( pRenderer )
		{
			if( Failure( pRenderer->Shutdown() ) ) return S_ERROR;
			delete pRenderer;
			pRenderer = NULL;
		}

		// Clear logging stream
		if( !bCustomLoggingStream && pLoggingStream )					
			delete pLoggingStream;					

		pLoggingStream = NULL;
		bCustomLoggingStream = false;

		// Reset settings
		dwBeforeFrame = 0;
		dwAfterFrame = 0;
		fLastFrameDelay = 0;
		camCamera = SCamera();
		sSettings = SSettings();

		return S_SUCCESS;
	}

	// ********************************************************************************************

	S_API SResult SpeedPointEngine::HookLightingSystem( SLightSystem* pSystem )
	{
		return S_ABORTED;
	}
}