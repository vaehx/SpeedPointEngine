// ********************************************************************************************

// SpeedPoint Engine

// ********************************************************************************************

#include <SpeedPoint.h>
#include <Abstract\SRenderPipeline.h>

#include <Implementation\DirectX9\SDirectX9Renderer.h>
#include <Implementation\DirectX9\SDirectX9ResourcePool.h>
#include <Lighting\SLightSystem.h>

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
	
		// Initialize the renderer and its resource pool
		if( settings.tyRendererType == S_DIRECTX9 )
		{
			pRenderer = (SRenderer*)new SDirectX9Renderer();
			if( Failure( pRenderer->Initialize( this, settings.hWnd, settings.nXResolution, settings.nYResolution, false ) ) )
			{
				return S_ERROR;	// failed to initialize renderer
			}

			pResourcePool = (SResourcePool*)new SDirectX9ResourcePool();
			if( Failure( pResourcePool->Initialize( this, pRenderer ) ) )
			{				
				return S_ERROR; // failed to initialize resource pool
			}

			// Initialize the viewports array
			pViewports = (SViewport**)( malloc( sizeof( SViewport* ) * SP_MAX_VIEWPORTS ) );
			if( pViewports == NULL )
			{
				return S_ERROR;
			}

			// Set the default viewport of the renderer as the zero-viewport
			pViewports[0] = pRenderer->GetDefaultViewport();
		}
		else
		{
			return S_ABORTED; // not supported yet
		}		

		bRunning = true;
		return S_SUCCESS;
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

	S_API SResult SpeedPointEngine::BeginFrame( void )
	{
		if( pRenderer == NULL || !bRunning ) return S_ABORTED;

/////// TODO: Add Commandbuffer management here
//////		This means we add a new task here to begin a new frame section inside the rendering pipeline
/////		But for now, the simple call should be enough
		
		S_RENDER_STATE iCurrentState = pRenderer->GetRenderPipeline()->NextState();		
		if( iCurrentState != S_RENDER_BEGINFRAME ) return S_ERROR;

		// BEGINFRAME = Begin the scene, clear backbuffer
		if( Failure( pRenderer->GetRenderPipeline()->BeginFrameSection() ) )
		{
			return S_ERROR;
		}

		return S_SUCCESS;
	}

	// ********************************************************************************************

	S_API SResult SpeedPointEngine::RenderSolid( SSolid* pSolid )
	{
		if( pRenderer == NULL || !bRunning ) return S_ABORTED;

//////// TODO: Add commandbuffer management here
////////	This means we add a new task here to render the solid.
///////		pay attention when giving the pointer: the position of the data might change
/////		For now, a simple call to the render pipeline should be enough
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

		if( pResourcePool )
		{
			if( Failure( pResourcePool->ClearAll() ) ) return S_ERROR;
			delete pResourcePool;
			pResourcePool = NULL;
		}

		free( pViewports );
		camCamera = SCamera();
		pDefaultViewport = NULL;

		if( pLightSystem )
		{
			if( Failure( pLightSystem->Clear() ) ) return S_ERROR;
			delete pLightSystem;
			pLightSystem = NULL;
		}

		dwBeforeFrame = 0;
		dwAfterFrame = 0;
		fLastFrameDelay = 0;

		if( pRenderer )
		{
			if( Failure( pRenderer->Shutdown() ) ) return S_ERROR;
			delete pRenderer;
			pRenderer = NULL;
		}

		return S_SUCCESS;
	}

	// ********************************************************************************************

	S_API SResult SpeedPointEngine::HookLightingSystem( SLightSystem* pSystem )
	{
		return S_ABORTED;
	}
}