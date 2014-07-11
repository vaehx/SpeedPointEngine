// ********************************************************************************************

//	This file is part of the SpeedPoint Game Engine

//	(c) 2011-2014 Pascal R. aka iSmokiieZz
//	All rights reserved.

// ********************************************************************************************

#pragma once

#include <SPrerequisites.h>
#include <Util\SPool.h>


SP_NMSPACE_BEG

class S_API SpeedPointEngine;
class S_API FramePipeline;
struct S_API IRenderer;
struct S_API IViewport;
struct S_API SCommandDescription;
struct S_API ISolid;


// Default Render call stack (example for DirectX9 and PhysicalSolid (which inherits from BasicSolid)):
// Application::S_E_RENDER_GEOMETRY_CALLS-Handler()
//	SpeedPoint::SBasicSolid::RenderGeometry()
//		SpeedPoint::SDirectX9Renderer::RenderSolidGeometry(ISolid*, boolbTextured)
//			SpeedPoint::SDirectX9RenderPipeline::RenderSolidGeometry(ISolid*, bool bTextured)
//				SpeedPoint::SDirectX9GeometryRenderSection::RenderSolidGeometry(ISolid*, bool bTextured)

// SpeedPoint Render Pipeline (abstract)
// This is a pure virtual class. You cannot instantiate it.
// Please use Specific Implementation to instantiate.
struct S_API IRenderPipeline
{
// Initialization / FramePipeline interaction
public:
	// Initialize this render pipeline by given SpeedPoint Engine pointer and renderer
	virtual SResult Initialize(SpeedPointEngine* pEngine, IRenderer* pRenderer) = 0;

	// Check whether the pipeline is initialized
	virtual inline bool IsInitialized() = 0;

	// Set the Frame pipeline pointer (due to aggregation)
	virtual SResult SetFramePipeline(FramePipeline* pFramePipeline) = 0;

	// Clearout the renderpipeline
	virtual SResult Clear(void) = 0;

	// Get the current state the pipeline is running at
	virtual S_FRAMEPIPELINE_STAGE GetCurrentStage(void) = 0;

	// Get the handling Renderer
	virtual IRenderer* GetRenderer() = 0;


// Viewport management
public:
	// Get the current target buffer. By default this is the backbuffer of this pipeline
	virtual IViewport* GetTargetViewport(void) = 0;

	// Set the current target buffer. By default this is the backbuffer of this pipeline
	virtual SResult SetTargetViewport(IViewport* pVP) = 0;

// The Sections
public:
	// Begin Rendering (S_RENDER_BEGIN)

	// Begin rendering
	// Prepares backbuffer, begins scene and does stuff like this...
	virtual SResult DoBeginRendering(S_GEOMETRY_RENDER_INTERACTION_STRATEGY geomRenderStrategy = eGEOMRENDER_STRATEGY_EVENTS) = 0;

	// Summary:
	//	Examine the renderqueue
	// Description:
	//	Pops through the whole render commmand queue, if in hybrid or pure command geom input strategy mode
	virtual SResult ExamineRenderCommandQueue() = 0;

	// Summary:
	//	Examine a single render command
	virtual SResult ExamineRenderCommandHybrid(SCommandDescription* pDesc) = 0;

	// Summary:
	//	Examine a single render command
	virtual SResult ExamineRenderCommand(SCommandDescription* pDesc) = 0;


	// Geometry (S_RENDER_GEOMETRY)

	// Do the Geometry Section (Draw calls)
	// Will only call event, so client application can draw its stuff using calls to
	// RenderSolidGeometry. After Event was raised, the function returns with result.
	// Events:
	//	- S_E_RENDER_GEOMETRY_BEGIN before BeginScene
	//	- S_E_RENDER_GEOMETRY_CALLS shortly before return
	virtual SResult DoGeometrySection() = 0;

	// Renders the Geometry of a solid when in S_RENDER_GEOMETRY stage
	// should normally called by ISolid Implementation!
	// Events: Fires no events!
	virtual SResult RenderSolidGeometry(ISolid* pSolid, bool bTextured) = 0;

	// Tell the render pipeline that geometry rendering is finished, so we can go on now
	// Events: Fires S_E_RENDER_GEOMETRY_EXIT before exiting geometry section
	virtual SResult ExitGeometrySection() = 0;

	
	// Lighting (S_RENDER_LIGHTING)

	// Generate lighting buffer and apply shading to G-Buffer.
	// Will abort if not in lighting stage
	// Events: Fires S_E_RENDER_LIGHTING_PREP
	virtual SResult DoLightingSection() = 0;

	
	// Post Effects (S_RENDER_POST)

	// Puts everything together and renders final OutputPlane. Calls
	// Will abort if not in Post state
	virtual SResult DoPost() = 0;


	// End Rendering (S_RENDER_END)

	// Ends a frame section and presents backbuffer
	virtual SResult DoEndRendering(void) = 0;			
};


SP_NMSPACE_END