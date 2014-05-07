////////////////////////////////////////////////////////////////////////////////////////////////////////

//	SpeedPoint Game Engine Source Code

//	written by iSmokiieZz
//	(c) 2011 - 2014, All rights reserved.

////////////////////////////////////////////////////////////////////////////////////////////////////////

#pragma once
#include <SPrerequisites.h>

namespace SpeedPoint
{
	////////////////////////////////////////////////////////////////////////////////////////////////////////
	//
	// E N G I N E   =   P I P E L I N E  +  R E N D E R I N G   +   D Y N A M I C S   +   D A T A  ( +  T O O L S)
	
	// PIPELINE = FRAMEPIPELINE + LOGGING + EVENTS
	// RENDERING = RENDERER + RENDERPIPELINE + SHADERS
	// DYNAMICS = PHYSICS + ANIMATION + SCRIPT + AI
	// DATA = SOLIDS + SCENEGRAPH + TEXTURES
	// ENGINE = PIPELINE + RENDERER + PHYSICS + AI + SCRIPT + GEOMETRY + RESOURCES
	//
	////////////////////////////////////////////////////////////////////////////////////////////////////////
	

	// A factory to bundle all important modules of an engine
	class S_API IEngine
	{
	// Framepipe
	public:		
		// Summary:
		//	Initializes the main Frame Pipeline. The FramePipeline will handle all necessary sequences of the Game Engine		
		// Arguments:
		//	pCustomFramePipeline - (default 0) pass to set and initialize a custom frame pipeline instance
		virtual SResult InitializeFramePipeline(SFramePipeline* pCustomFramePipeline = 0) = 0;		

		// Summary:
		//	Get the FramePipeline registers in this engine
		// Return Value:
		//	Pointer to the FramePipeline
		virtual SFramePipeline* GetFramePipeline() = 0;

		// Summary:
		//	Set the custom event data
		// Note:
		//	Will also be set, if pData is 0!
		virtual void SetCustomEventData(void* pData) = 0;

	// Rendering
	public:
		// Summary:
		//	Initialize the render with given type.
		// Arguments:
		//	type - The type of the built-in Renderer implementations. use S_RENDERER_CUSTOM to use pCustomRenderer
		//	pCustomRenderer - a pointer to a renderer implementation that is NOT yet initialized
		virtual SResult InitializeRenderer(const S_RENDERER_TYPE& type, IRenderer* pCustomRenderer = 0) = 0;

		// Summary:
		//	Get the renderer.
		// Return Value:
		//	Pointer to the implementation of the renderer (or custom renderer)
		virtual IRenderer* GetRenderer() = 0;


	// Physics
		// TODO: Add IPhysicsEngine
	public:	
		// Summary:
		//	Initialize the physics engine.
		// Description:
		//	Hooks into engine and physical geometry that has to be initialized already.
		//	A pointer to a custom implementation of the physics engine is not yet supported.
		// Return Value:
		//	S_NOTINIT - InitializePhysicalGeometry() was not called yet
		virtual SResult InitializePhysics() = 0;

		// Summary:
		//	Get the physics engine
		// Return Value:
		//	Returns pointer to the physics engine implementation
		virtual SPhysicsEngine* GetPhysics() = 0;


	// AI
		// Todo: Add class IAI
	public:		
		// Summary:
		//	Initializes the AI Engine.
		// Description:
		//	(TODO) This function does nothing yet.
		// Return Value:
		//	S_NOTIMPLEMENTED
		virtual SResult InitializeAI() = 0;	// no custom AI allowed yet

		// Summary:
		//	Get the AI Engine
		// Description:
		//	(TODO) This function does nothing yet
		// Return Value:
		//	Null-Pointer !
		virtual IAI* GetAI() = 0;


	// Script
		// Todo: Add class IScriptEngine
	public:
		// Summary:
		//	Initialize the Script Engine (RunPowder)
		// Description:
		//	No custom implementation of the ScriptEngine supported yet.
		virtual SResult InitializeScriptEngine() = 0;

		// Summary:
		//	Get the Script Engine
		// Return Value:
		//	Pointer to the implementation of the Scripting Engine
		virtual IScriptEngine* GetScriptEngine() = 0;


	// Geometry
	public:
		// Summary:
		//	Initialize the container for the physical solids.
		// Description:
		//	This also handles non-physical geometry, just use SPhysicalSolid::EnablePhysics();
		virtual SResult InitializePhysicalGeometry() = 0;

		// Summary:
		//	Get the container for the physical solids
		// Return Value:
		//	Pointer to the physical implementation of the ISolidSystem interface
		virtual ISolidSystem* GetPhysicalGeometry() = 0;	

	// Resources
	public:
		// Summary:
		//	Initialize the Resource Pool.
		// Description:
		//	The ResourcePool handles all resources like textures, scripts, sounds, ...		
		//	The ResourcePool is Renderer-specific, as DX uses other texture buffer classes than GL
		virtual SResult InitializeResourcePool() = 0;	// no custom resource pool allowed yet

		// Summary:
		//	Get the Resource Pool.
		// Return Value:
		//	Returns pointer to the renderer specific implementation of the ResourcePool
		virtual IResourcePool* GetResources() = 0;


	// Logging		
	public:
		// Summary:
		//	Initialize the Logger
		// Arguments:
		//	pCustomLogStream - Set this to a valid pointer to use your own Logging stream instance. THIS MUST ALREADY BE INITIALIZED!
		// Return Value:
		//	S_INVALIDPARAM - pCustomLogStream is an invalid ptr or points to an uninitialized Logging Stream
		virtual SResult InitializeLogger(SLogStream* pCustomLogStream = 0) = 0;		

		// Summary:
		//	Get the registered logging stream
		// Return Value:
		//	Pointer to the registered logging stream (either the default or the custom one)
		virtual SLogStream* GetLogStream() = 0;

		// Summary:
		//	Registers a LogHandler function to the registered logging stream
		// Arguments:
		//	pHandler - Valid ptr to a LogHandler function (!= EventHandler)
		// Return Value:
		//	S_NOTINIT - No initialized logger is registered in this engine. Use InitializeLogger()
		virtual SResult AddLogHandler(SLogHandler pHandler) = 0;
	};
}