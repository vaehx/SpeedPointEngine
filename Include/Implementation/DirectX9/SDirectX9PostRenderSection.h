// ********************************************************************************************

//	This file is part of the SpeedPoint Game Engine

//	(c) 2011-2014 Pascal R. aka iSmokiieZz
//	All rights reserved.

// ********************************************************************************************

#pragma once

#include <SPrerequisites.h>
#include <Abstract\Pipelines\SRenderPipelineSection.h>
#include <Implementation\DirectX9\SDirectX9Shader.h>
#include <Implementation\DirectX9\SDirectX9OutputPlane.h>

namespace SpeedPoint
{
	// DirectX9 Implementation of SRenderPipelineSection for Post Effects
	class S_API SDirectX9PostRenderSection : public SRenderPipelineSection
	{
	private:
		SpeedPointEngine*		m_pEngine;		// Ptr to the SpeedPoint Game Engine instance
		SDirectX9RenderPipeline*	m_pDX9RenderPipeline;	// Ptr to the DirectX9 Render Pipeline instance

		SDirectX9Shader			m_PostShader;	// the post effects shader

		SDirectX9OutputPlane		m_OutputPlane;	// the output plane

	// Initialization / Destruction
	public:
		~SDirectX9PostRenderSection();

		// Initialize the Render Pipeline State
		virtual SResult Initialize(SpeedPointEngine* pEngine, SRenderPipeline* pRenderPipeline);

		// Clear the Render Pipeline state
		virtual SResult Clear(void);

	// Section specific methods
	public:
		// Render the output plane to the backbuffer with the output of the given Lighting render section
		SResult RenderOutputPlane(SDirectX9LightingRenderSection* pLightingSection);

	// Getter / setter
	public:
		SDirectX9OutputPlane* GetOutputPlane() { return &m_OutputPlane; }
	};
}