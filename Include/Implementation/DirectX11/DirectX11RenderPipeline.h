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
#include <Abstract\Pipelines\IRenderPipeline.h>

#include "DirectX11.h"
#include "DirectX11GeometryRenderSection.h"
#include "DirectX11LightingRenderSection.h"
#include "DirectX11PostRenderSection.h"

#include <Pipelines\SCommandBuffering.h>

SP_NMSPACE_BEG

class S_API DirectX11Renderer;
struct S_API IViewport;
class S_API FramePipeline;

// DirectX9 Implementation of the SpeedPoint Render Pipeline
class S_API DirectX11RenderPipeline : public IRenderPipeline
{
private:
	SpeedPointEngine*		m_pEngine;
	DirectX11Renderer*		m_pDXRenderer;
	IViewport*			m_pTargetViewport;
	FramePipeline*			m_pFramePipeline;	// the handling frame pipeline



	// Deferred Rendering:

//	LPDIRECT3DVERTEXDECLARATION9	m_pVertexDeclaration;

	DirectX11GeometryRenderSection	m_GeometryRenderSection;
	DirectX11LightingRenderSection	m_LightingRenderSection;
	DirectX11PostRenderSection	m_PostRenderSection;



	// Command buffering:

	SCommandQueue			m_Queue;

	S_GEOMETRY_RENDER_INTERACTION_STRATEGY	m_GeometryRenderStrategy;


	// Initialization / Interaction with Frame pipeline
public:
	// Default constructor
	DirectX11RenderPipeline();

	// Default destructor
	~DirectX11RenderPipeline();

	// Initialize this render pipeline by given SpeedPoint Engine pointer and renderer
	virtual SResult Initialize(SpeedPointEngine* pEngine, IRenderer* pRenderer);

	// Check whether the pipeline is initialized
	virtual inline bool IsInitialized();

	// Set the Frame pipeline pointer (due to aggregation)
	virtual SResult SetFramePipeline(FramePipeline* pFramePipeline);

	// Clearout the renderpipeline
	virtual SResult Clear(void);

	// Get the current state the pipeline is running at
	virtual S_FRAMEPIPELINE_STAGE GetCurrentStage(void);

	// Get the handling Renderer
	virtual IRenderer* GetRenderer();

	// Viewport management
public:
	// Get the current target buffer. By default this is the backbuffer of this pipeline
	virtual IViewport* GetTargetViewport(void);

	// Set the current target buffer. By default this is the backbuffer of this pipeline
	virtual SResult SetTargetViewport(IViewport* pVP);

	// The RenderPipeline Sections
public:
	// Begin Rendering (S_RENDER_BEGIN)

	// Begin rendering
	// prepares backbuffer, begins scene and does stuff like this...
	// WARNING: This functions assumes that the render target is still the backbuffer,
	// as this is the last render target used in Post Render Section!
	virtual SResult DoBeginRendering(S_GEOMETRY_RENDER_INTERACTION_STRATEGY geomRenderStrategy = eGEOMRENDER_STRATEGY_EVENTS);

	// Summary:
	//	Examine the renderqueue
	// Description:
	//	Pops through the whole render commmand queue, if in hybrid or pure command geom input strategy mode
	virtual SResult ExamineRenderCommandQueue();

	// Summary:
	//	Examine a single render command
	virtual SResult ExamineRenderCommandHybrid(SCommandDescription* pDesc);

	// Summary:
	//	Examine a single render command
	virtual SResult ExamineRenderCommand(SCommandDescription* pDesc);


	// Geometry (S_RENDER_GEOMETRY)

	// Do the Geometry Section (Draw calls)
	// Will only call event, so client application can draw its stuff using calls to
	// RenderSolidGeometry. After Event was raised, the function returns with result.
	virtual SResult DoGeometrySection();

	// Renders the Geometry of a solid when in S_RENDER_GEOMETRY stage
	// should normally called by ISolid Implementation!
	virtual SResult RenderSolidGeometry(ISolid* pSolid, bool bTextured);

	// Tell the render pipeline that geometry rendering is finished, so we can go on now
	virtual SResult ExitGeometrySection();


	// Lighting (S_RENDER_LIGHTING)

	// Generate lighting buffer and apply shading to G-Buffer. Calls S_E_RENDER_LIGHTING event.
	// Will abort if not in lighting stage
	virtual SResult DoLightingSection();


	// Post Effects (S_RENDER_POST)

	// Puts everything together and renders final OutputPlane. Calls
	// Will abort if not in Post state
	virtual SResult DoPost();


	// End Rendering (S_RENDER_END)

	// Ends a frame section and presents backbuffer
	virtual SResult DoEndRendering(void);

	// Getter / setter
public:
	FramePipeline* GetFramePipeline() { return m_pFramePipeline; }
};


SP_NMSPACE_END