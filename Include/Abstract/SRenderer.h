// ********************************************************************************************

//	SpeedPoint Renderer

//	This is a virtual class. You cannot instantiate it.
//	Please use Specific Implementation to instantiate.

//	Note: Each Renderer instance has its own render pipeline.
//		And this render pipeline cannot be changed!

// ********************************************************************************************

#pragma once

#include <SPrerequisites.h>

namespace SpeedPoint
{
	// SpeedPoint Renderer (abstract)
	class S_API SRenderer
	{
	public:
		// Get the type of this renderer
		virtual S_RENDERER_TYPE GetType( void ) = 0;

		// Intialize the renderer
		virtual SResult Initialize( SpeedPointEngine* pEngine, HWND hWnd, int nW, int nH, bool bIgnoreAdapter ) = 0;

		// Set the current target viewport
		virtual SResult SetTargetViewport( SViewport* pViewport ) = 0;

		// Get the current target viewport
		virtual SViewport* GetTargetViewport( void ) = 0;

		// Get the default viewport
		virtual SViewport* GetDefaultViewport( void ) = 0;

		// Update projection and view matrix to those of the given viewport
		virtual SResult UpdateViewportMatrices( SViewport* pViewport ) = 0;

		// Create an an addition viewport
		virtual SResult CreateAdditionalViewport( SViewport** pViewport ) = 0;

		// Clearout everything (viewports, buffers, stop render Pipeline thread and task buffer)
		virtual SResult Shutdown( void ) = 0;

		// Check if this Renderer is initialized properly
		virtual bool IsInited( void ) = 0;

		// Get a pointer to the instance of the render pipeline
		virtual SRenderPipeline* GetRenderPipeline( void ) = 0;	

		// Begin a new Scene
		virtual SResult BeginScene( void ) = 0;

		// End the current Scene
		virtual SResult EndScene( void ) = 0;	
	};
}