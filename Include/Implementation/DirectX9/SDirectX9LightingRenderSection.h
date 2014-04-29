// ********************************************************************************************

//	This file is part of the SpeedPoint Game Engine

// ********************************************************************************************

#pragma once

#include <SPrerequisites.h>
#include <Abstract\Pipelines\IRenderPipelineSection.h>
#include "SDirectX9FrameBuffer.h"
#include "SDirectX9Shader.h"
#include <Util\SPool.h>

namespace SpeedPoint
{
	// DirectX9 Implementation of IRenderPipelineSection for Lighting and Shading
	class S_API SDirectX9LightingRenderSection : public IRenderPipelineSection
	{
	private:
		SpeedPointEngine*		m_pEngine;		// Ptr to the SpeedPoint Game Engine instance
		SDirectX9RenderPipeline*	m_pDX9RenderPipeline;	// Ptr to the DirectX9 Render Pipeline instance		
		
		SDirectX9Shader			m_LightingShader;	// the lighting shader
		SDirectX9FrameBuffer		m_LightingBuffer;	// The output of the lighting shader

		SPool<SLight*>*			m_pLightSources;	// Pool of ptrs to the light sources to be rendered			

	// Initialization
	public:
		// Default constructor
		SDirectX9LightingRenderSection();

		// Default destructor
		~SDirectX9LightingRenderSection();

		// Initialize the Render Pipeline State
		virtual SResult Initialize(SpeedPointEngine* pEngine, IRenderPipeline* pRenderPipeline);

		// Clear the Render Pipeline state
		virtual SResult Clear(void);

		// Prepare render targets for the lighting section
		// usally called by RenderPipeline implementation after finishing geometry section
		SResult PrepareSection();

		// Reset the render target of the lighting section
		// usally called by RenderPipeline implementation after FreeLightSources()
		SResult EndSection();

	// Light buffer assignment and render call
	public:
		// Generate lighting buffer by lighting system stored in SpeedPointEngine, given by Frame-Buffer.
		// This is the default function. If client wants to select lights on its own, change
		// the SSettings::m_bCustomLighting setting to true, catch S_E_RENDER_LIGHTING_PREP event and
		// modify 'lights'-EventParameter (ptr to lights buffer)
		// Will abort if not in lighting stage
		SResult PrepareLightSources(SPool<SLight*>** pLightSources);

		// Free the light source buffer created by PrepareLightSources()
		// usally called by RenderPipeline implementation after RenderLighting()
		SResult FreeLightSources();

		// Generate lighting buffer by previously prepared light-sources-buffer and given geometry stage output
		// Renders onto given output plane. This usually is the one of the post-render section.
		// Will abort if not in lighting stage
		SResult RenderLighting(SDirectX9GeometryRenderSection* pDXGeometryStage, SDirectX9OutputPlane* pOutputPlane);

	// Getter / setter
	public:
		SDirectX9FrameBuffer* GetLightingBuffer() { return &m_LightingBuffer; }
	};
}