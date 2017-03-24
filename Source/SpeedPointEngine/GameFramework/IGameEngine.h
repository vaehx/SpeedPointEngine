//////////////////////////////////////////////////////////////////////////////////////////////////
//
//	SpeedPoint Game Engine
//	Copyright (c) 2011-2017 Pascal Rosenkranz, All rights reserved.
//
//////////////////////////////////////////////////////////////////////////////////////////////////

#pragma once

#include <Common\SPrerequisites.h>
#include <Renderer\IRenderer.h> // SRendererInitParams

SP_NMSPACE_BEG

struct S_API IFramePipeline;
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
struct S_API IShutdownHandler;
class S_API ProfilingSystem;

//////////////////////////////////////////////////////////////////////////////////////////////////

struct S_API SGameEngineInitParams
{
	S_RENDERER_TYPE rendererImplementation;
	SRendererInitParams rendererParams;
	IScene* pCustomScene; // If not set, the engine will create the scene. If set, the engine will NOT destruct the scene.
	string logFilename;

	SGameEngineInitParams()
		: rendererImplementation(S_DIRECTX11),
		pCustomScene(0),
		logFilename("SpeedPoint.log")
	{
	}
};

//////////////////////////////////////////////////////////////////////////////////////////////////

// Interface to all important modules of the engine
struct S_API IGameEngine
{
public:
	virtual ~IGameEngine()
	{
	}

public: // Initialization
	virtual SResult Initialize(const SGameEngineInitParams& params) = 0;
	virtual SResult FinishInitialization() = 0;
	virtual SResult Start() { return FinishInitialization(); }
	virtual void Shutdown() = 0;
	virtual bool IsRunning() const = 0;

public: // Component getter
	ILINE virtual IPhysics* GetPhysics() const = 0;
	ILINE virtual IRenderer* GetRenderer() const = 0;	
	ILINE virtual I3DEngine* Get3DEngine() const = 0;
	ILINE virtual IFontRenderer* GetFontRenderer() const = 0;
	ILINE virtual IResourcePool* GetResources() const = 0;
	ILINE virtual ISettings* GetSettings() const = 0;
	ILINE virtual IScene* GetScene() const = 0;
	ILINE virtual IEntityReceiptManager* GetEntityReceiptManager() const = 0;
	ILINE virtual IMaterialManager* GetMaterialManager() const = 0;

public: // Game / Application interface
	virtual void RegisterApplication(IApplication* pApp) = 0;
	virtual IApplication* GetApplication() const = 0;

	// Does NOT end with a slash
	virtual string GetShaderDirectoryPath() const = 0;
	virtual string GetResourcePath(const string& file = "") const = 0;

	virtual void LoadWorld(const string& file) = 0;

	// Executes an engine Update->Render cycle
	virtual void DoFrame() = 0;
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

	inline static IPhysics* GetPhysics() { return m_pEngine->GetPhysics(); }
	inline static IRenderer* GetRenderer() { return m_pEngine->GetRenderer(); }
	inline static I3DEngine* Get3DEngine() { return m_pEngine->Get3DEngine(); }
	inline static IScene* GetScene() { return m_pEngine->GetScene(); }
	inline static IEntityReceiptManager* GetEntityReceiptManager() { return m_pEngine->GetEntityReceiptManager(); }
};

SP_NMSPACE_END
