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
#include <Util\SPool.h>
#include <Abstract\Pipelines\IRenderPipelineSection.h>
#include "DirectX11FBO.h"
#include "DirectX11Effect.h"

SP_NMSPACE_BEG


// Forward declarations
class S_API DirectX11OutputPlane;
class S_API DirectX11RenderPipeline;
class S_API DirectX11GeometryRenderSection;
class S_API SLight;
struct S_API IRenderPipeline;

//////////////////////////////////////////////////////////////////////////////////////////////////

class DirectX11LightingRenderSection : public IRenderPipelineSection
{
private:
	SpeedPointEngine* m_pEngine;	// Ptr to the SpeedPoint Game Engine instance
	DirectX11RenderPipeline* m_pDXRenderPipeline;	// Ptr to the DirectX9 Render Pipeline instance		

	DirectX11Effect m_LightingEffect;	// the lighting shader
	DirectX11FBO m_LightingBuffer;	// The output of the lighting shader

	SPool<SLight*>* m_pLightSources;	// Pool of ptrs to the light sources to be rendered			


	// Initialization
public:
	// Default constructor
	DirectX11LightingRenderSection();

	// Default destructor
	~DirectX11LightingRenderSection();
	
	virtual SResult Initialize(SpeedPointEngine* pEngine, IRenderPipeline* pRenderPipeline);
	virtual SResult Clear(void);


	SResult PrepareSection();
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
	SResult RenderLighting(DirectX11GeometryRenderSection* pDXGeometryStage, DirectX11OutputPlane* pOutputPlane);

	// Getter / setter
public:
	DirectX11FBO* GetLightingBuffer() { return &m_LightingBuffer; }
};



SP_NMSPACE_END