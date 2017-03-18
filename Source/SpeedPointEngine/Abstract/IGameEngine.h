//////////////////////////////////////////////////////////////////////////////////////////////////
//
//	SpeedPoint Game Engine
//	Copyright (c) 2011-2017 Pascal Rosenkranz, All rights reserved.
//
//////////////////////////////////////////////////////////////////////////////////////////////////

#pragma once

#include "SPrerequisites.h"
#include "IFramePipeline.h"	// for default skipsections

SP_NMSPACE_BEG

struct S_API IFramePipeline;	
struct S_API IRenderer;
struct S_API I3DEngine;
struct S_API IFontRenderer;
struct S_API IApplication;
struct S_API IResourcePool;
struct S_API IScene;
struct S_API IMaterial;
struct S_API IMaterialManager;
struct S_API ISettings;
struct S_API IEntityReceiptManager;
struct S_API IPhysics;

//////////////////////////////////////////////////////////////////////////////////////////////////

enum EShaderFileType
{
	eSHADERFILE_SKYBOX,
		
	eSHADERFILE_FORWARD_HELPER,	// Forward helper effect
	eSHADERFILE_FORWARD,

	eSHADERFILE_FONT,	// forward font effect

	eSHADERFILE_DEFERRED_ZPASS,	// ZPass for Deferred Shading
	eSHADERFILE_DEFERRED_SHADING,	// shading pass for Deferred Shading
	eSHADERFILE_TERRAIN,	// Deferred Shading Terrain
	eSHADERFILE_GUI
};

//////////////////////////////////////////////////////////////////////////////////////////////////

// Summary:
//	An object that keeps a pointer to the engine or the renderer can implement this interface
//	to get informed about the shutdown. This prevents memory access violations.
//
// TODO: Maybe better do this using smart pointers to the engine / the renderer someday
struct S_API IShutdownHandler
{
	virtual SResult HandleShutdown() = 0;
	virtual string GetShutdownHandlerDesc() const { return "unknown"; }
};

//////////////////////////////////////////////////////////////////////////////////////////////////

// Interface to all important modules of the engine
struct S_API IGameEngine
{
public:
	virtual ~IGameEngine()
	{
	}

	virtual void RegisterApplication(IApplication* pApp) = 0;
	virtual IApplication* GetApplication() const = 0;


	// Arguments:
	//	pCustomFramePipeline - pass a valid ptr 
	virtual SResult InitializeFramePipeline(IFramePipeline* pCustomFramePipeline = 0) = 0;		

	// Arguments:
	//	bManageDealloc - Set to true to let the engine handle the destruction of the instance. Otherwise pay attention
	//		to do this on your own! Default: true
	virtual SResult InitializeRenderer(const S_RENDERER_TYPE& type, IRenderer* pRender, bool bManageDealloc = true) = 0;
	virtual SResult InitializeFontRenderer() = 0;
	virtual SResult Initialize3DEngine(I3DEngine* p3DEngine, bool bManageDealloc = true) = 0;

	// Inits Renderer specific resource pool and the material manager
	virtual SResult InitializeResourcePool(IMaterialManager* pMatMgr, bool bManageDealloc = true) = 0;		
		
	virtual SResult InitializePhysics() = 0;

	// instance is deleted by engine on shutdown
	virtual SResult InitializeScene(IScene* pScene) = 0;

	virtual bool IsRunning() const = 0;
	virtual void RegisterShutdownHandler(IShutdownHandler* pShutdownHandler) = 0;
	virtual void UnregisterShutdownHandler(IShutdownHandler* pShutdownHandler) = 0;
	virtual void Shutdown() = 0;		


	ILINE virtual IFramePipeline* GetFramePipeline() const = 0;
	ILINE virtual IPhysics* GetPhysics() const = 0;
	ILINE virtual IRenderer* GetRenderer() const = 0;	
	ILINE virtual I3DEngine* Get3DEngine() const = 0;
	ILINE virtual IFontRenderer* GetFontRenderer() const = 0;
	ILINE virtual IResourcePool* GetResources() const = 0;
	ILINE virtual ISettings* GetSettings() const = 0;	
	ILINE virtual IScene* GetScene() const = 0;
	ILINE virtual IEntityReceiptManager* GetEntityReceiptManager() const = 0;
	ILINE virtual IMaterialManager* GetMaterialManager() const = 0;

	virtual string GetShaderPath(EShaderFileType shaderFile) const = 0;
	virtual string GetResourcePath(const string& file = "") const = 0;

	virtual void LoadWorld(const string& file) = 0;
		
	// Summary:
	//	Will start the framepipeline and call the Update() and Render() callback functions of IApplication impl
	virtual SResult ExecuteFramePipeline(usint32 iSkippedSections = DEFAULT_SKIPPED_SECTIONS) = 0;

	// --------------------------------------
	// Budgeting System:
	// Todo - put this into a separate interface

	virtual unsigned int StartBudgetTimer(const char* name) = 0;

	// Re-Starts the given timer, adding time to the current timer duration
	// If the timer does not exist, this function does nothing.
	virtual void ResumeBudgetTimer(unsigned int timerId) = 0;
		
	// timerId - id returned by StartBudgetTimer()
	virtual void StopBudgetTimer(unsigned int timerId) = 0;		

	// --------------------------------------

	virtual SResult FinishInitialization() = 0;
	virtual SResult Start() { return FinishInitialization(); }
};

//////////////////////////////////////////////////////////////////////////////////////////////////

class S_API SpeedPointEnv
{
private:
	static IGameEngine* m_pEngine;
public:
	static IGameEngine* GetEngine()
	{
		return m_pEngine;
	}

	static void SetEngine(IGameEngine* pEngine)
	{
		m_pEngine = pEngine;
	}
};

SP_NMSPACE_END
