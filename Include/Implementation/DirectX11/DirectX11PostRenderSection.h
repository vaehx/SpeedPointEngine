//////////////////////////////////////////////////////////////////////////////////////////////////
//
//	This file is part of the SpeedPoint Game Engine
//
//	written by Pascal R. aka iSmokiieZz
//	(c) 2011-2014, All rights reserved.
//
//////////////////////////////////////////////////////////////////////////////////////////////////

#pragma once

#include <SPrerequisites.h>
#include <Abstract\Pipelines\IRenderPipelineSection.h>
#include "DirectX11Effect.h"
#include "DirectX11OutputPlane.h"

SP_NMSPACE_BEG

class DirectX11RenderPipeline;
struct IRenderPipeline;
class DirectX11LightingRenderSection;

// DirectX9 Implementation of IRenderPipelineSection for Post Effects
class S_API DirectX11PostRenderSection : public IRenderPipelineSection
{
private:
	SpeedPointEngine*		m_pEngine;		// Ptr to the SpeedPoint Game Engine instance
	DirectX11RenderPipeline*	m_pDXRenderPipeline;	// Ptr to the DirectX9 Render Pipeline instance

	DirectX11Effect			m_PostShader;	// the post effects shader

	DirectX11OutputPlane		m_OutputPlane;	// the output plane

	// Initialization / Destruction
public:
	~DirectX11PostRenderSection();

	// Initialize the Render Pipeline State
	virtual SResult Initialize(SpeedPointEngine* pEngine, IRenderPipeline* pRenderPipeline);

	// Clear the Render Pipeline state
	virtual SResult Clear(void);

	// Section specific methods
public:
	// Render the output plane to the backbuffer with the output of the given Lighting render section
	SResult RenderOutputPlane(DirectX11LightingRenderSection* pLightingSection);

	// Getter / setter
public:
	DirectX11OutputPlane* GetOutputPlane() { return &m_OutputPlane; }
};



SP_NMSPACE_END