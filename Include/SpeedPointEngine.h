//////////////////////////////////////////////////////////////////////////////////////////////////
//
//	SpeedPoint Game Engine
//	This file is part of the SpeedPoint Game Engine
//
//	(c) 2011-2014 Pascal R. aka iSmokiieZz
//	All rights reserved.
//
//////////////////////////////////////////////////////////////////////////////////////////////////

#pragma once

#include "SPrerequisites.h"

#include "Abstract\IResourcePool.h"
#include "SSettings.h"
#include "Util\SCamera.h"
#include "Util\SLogStream.h"

#include "Pipelines\FramePipeline.h"

#include "OnionPhysics\SPhysicsEngine.h"
#include "OnionPhysics\SPhysicalSolidSystem.h"

#include "Lighting\SLightSystem.h"

// ~~~~~~
// TODO: Try to eliminate this limit
#define SP_MAX_VIEWPORTS 10
// ~~~~~~

//////////////////////////////////////////////////////////////////////////////////////////////////

SP_NMSPACE_BEG


	// The Default skipstages due to some sections begin not implemented yet
	// Note: its not recommended to skip render stages!
#define SP_DEFAULT_SKIPSTAGES S_SKIP_DYNAMICS \
	| S_SKIP_DYNAMICS_ANIM \
	| S_SKIP_DYNAMICS_PHYSICS \
	| S_SKIP_DYNAMICS_INPUT \
	| S_SKIP_DYNAMICS_SCRIPT \
	| S_SKIP_DYNAMICS_RENDERSCRIPT


//////////////////////////////////////////////////////////////////////////////////////////////////
// forward declarations

struct S_API IViewport;
struct S_API IRenderer;





// TODO: Try to get an interface from this engine!!





// SpeedPoint Game Engine contains all parts of the default SpeedPoint Engine components (Frame, Physics, Rendering, ...)
// If you want to use custom components you can either hook your components into the other components
class S_API SpeedPointEngine : public IExceptionProxy
{
private:	
	bool			m_bRunning;		// Is the Game Engine started and initialized?
	SSettings		m_Settings;		// Main Settings of the Game Engine	

	FrameEngine		m_FrameEngine;		// Frame Engine Component	
	IRenderer*		m_pRenderer;		// Renderer Engine Component (DirectX9, DirectX11, OpenGL)

	SPhysicsEngine		m_PhysicsEngine;	// Default Physics Engine Component
	ISolidSystem*		m_pWorld;		// global system of physical objects (terrain, cow, rocks, barrels, ...)	

	SLightSystem		m_GlobalLightSystem;	// global (default) system of light sources

	IViewport**		m_pViewports;		// Viewport Buffer
	IViewport*		m_pDefaultViewport;	// Pointer to the Default viewport	

	IResourcePool*		m_pResourcePool;	// Common Resource Pool handling Vertex-, Index-, Texture-, ...-buffers

	SLogStream*		m_pLoggingStream;	// Logging stream used in this Game Engine
	bool			m_bCustomLogStream;	// Is a custom logging stream used?

public:
	// Default constructor
	SpeedPointEngine()
		: m_bRunning(false),
		m_pRenderer(0),
		m_pViewports(0),
		m_pDefaultViewport(0),
		m_pResourcePool(0),
		m_pLoggingStream(0)
	{
	}

	// Check the settings of a given SSettings instance
	bool IsValidSettings(const SSettings& settings);

	// Setup the Game Engine settings and update components if running already
	SResult UpdateSettings(const SSettings& customSettings);

	// Summary:
	//	Start the Engine components
	// Arguments:
	//	pRendererInstance - pass an Instance created by RendererImplementation::GetInstance()
	SResult Start(IRenderer* pRendererInstance);

	// Start the Engine components with given settings (avoids extra SetSettings()-Call)
	SResult Start(IRenderer* pRendererInstance, const SSettings& customSettings);

	// Stop the Engine components and clean up everything
	SResult Shutdown();

	// Set the custom event data ptr
	void SetCustomEventData(void* pData);


// Viewport Management
public:
	// Get an existing viewport
	IViewport* GetViewport(unsigned int index);

	// Add an additional viewport and retrieve pointer to it
	IViewport* AddViewport(HWND hWnd);


// Global physical solids management
public:
	// Add a new physical solid and return the id of it
	// (We assume there is no non-physical object in the world, water is physical, particles have
	// their own ParticleSystem and an extra System for Billboards would overdo it)
	SP_ID AddSolid(SResult* pResult = 0);

	// Add a new physical solid and return temporary pointer of it
	// (We assume there is no non-physical object in the world, water is physical, particles have
	// their own ParticleSystem and an extra System for Billboards would overdo it)
	ISolid* AddSolid(SP_ID* pUID, SResult* pResult = 0);

	// Get an existing physical solid by its unique id
	ISolid* GetSolid(const SP_ID& id);

// Frame pipeline control-methods
public:
	// Begin a new frame (a new round of the whole frame pipeline)
	// - Once this is called, the Engine will call any containing method by itself
	// - iSkipStages is bit-combination of which frame pipeline stages should be skipped
	SResult BeginFrame(unsigned int iSkipStages = SP_DEFAULT_SKIPSTAGES);

	// Abort Frame, no matter which state it is (please use this only in panic situations :))
	//~~~~~~~~
	// TODO:
	//	SResult AbortFrame();
	//~~~~~~~~

	// Render a physical solid by given id, stored in PhysWorld of SSpeedPointEngine instance
	// This function will return error if frame pipeline didnt reach FRAME::RENDER::GEOMETRY State
	SResult RenderSolid(const SP_ID& iSolid);

// Logging methods
public:
	// Set a custom logging stream for this engine instance
	SResult SetCustomLogStream(SLogStream* pLogStream);

	// Report a log entry and return the result given as parameter
	SResult LogReport(SResult res, SString msg);

	// Report an error log entry and return S_ERROR
	SResult LogE(SString msg);

	// Report an warning log entry and return S_WARN
	SResult LogW(SString msg);

	// Report an info / success message and return S_INFO
	SResult LogI(SString msg);

	// Implement IExceptionProxy methods
	virtual void HandleException(char* msg);

// Getter / Setter
public:
	bool IsInitialized();

	bool IsRunning() { return m_bRunning; }

	SSettings& GetSettings() { return m_Settings; }	
	SResult SetSettings(const SSettings& settings) { return UpdateSettings(settings); }

	FrameEngine* GetFrameEngine() { return &m_FrameEngine; }

	SPhysicsEngine* GetPhysicsEngine() { return &m_PhysicsEngine; }

	SLightSystem* GetGlobalLightSystem() { return &m_GlobalLightSystem; }

	IRenderer* GetRenderer() { return m_pRenderer; }
	// TODO: Function to get renderer type

	IResourcePool* GetResourcePool() { return m_pResourcePool; }
};


SP_NMSPACE_END