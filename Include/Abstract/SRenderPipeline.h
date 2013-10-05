// ********************************************************************************************

//	SpeedPoint Render Pipeline

//	This is a virtual class. You cannot instantiate it.
//	Please use Specific Implementation to instantiate.

// ********************************************************************************************

#pragma once

#include <SPrerequisites.h>
#include <SPool.h>

namespace SpeedPoint
{
	// SpeedPoint Render Pipeline (abstract)
	class S_API SRenderPipeline
	{
	public:
		// Initialize this render pipeline by given SpeedPoint Engine pointer and renderer
		virtual SResult Initialize( SpeedPointEngine* pEngine, SRenderer* pRenderer ) = 0;

		// Get the current state the pipeline is running at
		virtual S_RENDER_STATE GetState( void ) = 0;

		// Switch the current state to the next state
		virtual S_RENDER_STATE NextState( void ) = 0;

		// Get the current target buffer. By default this is the backbuffer of this pipeline
		virtual SViewport* GetTargetViewport( void ) = 0;

		// Set the current target buffer. By default this is the backbuffer of this pipeline
		virtual SResult SetTargetViewport( SViewport* pVP ) = 0;

		// Clearout the renderpipeline
		virtual SResult Clear( void ) = 0;

		// -- BEGIN FRAME STATE --

		// Begin a new frame
		virtual SResult BeginFrameSection( void ) = 0;		

		// -- GEOMETRY STATE --

		// Render the geometry of a Primitive of a solid to the current G-Buffer
		// Will abort if not in geometry state
		virtual SResult RenderSolidGeometry( SSolid* pSolid ) = 0;

		// Invokes the end of GBuffer creation
		// This function is normally called by the CommandBuffer Playback when recognizing a
		// S_RENDER_LIGHTING command after a S_RENDER_GEOMETRY command
		virtual SResult StopGeometryRendering( void ) = 0;

		// -- LIGHTING STATE --
		
		// Render the current corresponding Light Buffer to the current G-Buffer by given light pool
		// Will abort if not in lighting state
		virtual SResult RenderLighting( SPool<SLight>* pLightPool ) = 0;

		// Render the current corresponsing Light Buffer to the current G-Buffer by given light array
		// Will abort if not in lighting state
		virtual SResult RenderLighting( SLight* pLightArray ) = 0;	

		// -- POST --

		// Now merge and apply custom shaders (if given) and render to current target buffer
		// Will abort if not in Post state
		virtual SResult RenderPostEffects( SP_UNIQUE* pCustomPostShaders ) = 0;

		// -- ENDFRAME --

		// Ends a frame section
		virtual SResult EndFrameSection( void ) = 0;

		// -- PRESENT --

		// Present the recently created frame section to the current target viewport
		virtual SResult Present( void ) = 0;		
	};
}