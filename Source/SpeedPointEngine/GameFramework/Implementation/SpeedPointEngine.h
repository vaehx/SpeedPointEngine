//////////////////////////////////////////////////////////////////////////////////////////////////
//
//	SpeedPoint Game Engine
//	Copyright (c) 2011-2017 Pascal Rosenkranz, All rights reserved.
//
//////////////////////////////////////////////////////////////////////////////////////////////////

#pragma once

#include "..\IGameEngine.h"
#include <Common\SPrerequisites.h>

//////////////////////////////////////////////////////////////////////////////////////////////////

SP_NMSPACE_BEG

class S_API FileLogListener;
class S_API ProfilingDebugView;

//////////////////////////////////////////////////////////////////////////////////////////////////

template<typename T> struct S_API EngineComponent
{
	T* pComponent;
	bool bCustom;

	EngineComponent()
		: pComponent(nullptr),
		bCustom(false)
	{
	}

	EngineComponent(const EngineComponent<T>& o)
		: pComponent(o.pComponent),
		bCustom(o.bCustom)
	{
	}

	~EngineComponent()
	{
		Clear();
	}

	void Clear()
	{
		if (!bCustom && pComponent)
		{
			delete pComponent;

		}
		pComponent = 0;
		bCustom = false;
	}

	void SetCustom(T* component)
	{
		pComponent = component;
		bCustom = true;
	}

	void SetOwn(T* pInstance)
	{
		pComponent = pInstance;
		bCustom = false;
	}

	ILINE operator T*() const
	{
		return pComponent;
	}

	ILINE T* operator ->() const { return pComponent; }
};

//////////////////////////////////////////////////////////////////////////////////////////////////

// SpeedPoint Game Engine contains all parts of the default SpeedPoint Engine components (Frame, Physics, Rendering, ...)
// If you want to use custom components you can either hook your components into the other components
class S_API SpeedPointEngine : public IGameEngine
{
private:	
	IApplication*		m_pApplication;

	bool			m_bRunning;		// Is the Game Engine started and initialized?
	bool			m_bLoggedSkipstages;
	ISettings*		m_pSettings;		// Main Settings of the Game Engine	

	ProfilingDebugView* m_pProfilingDebugView;

	EngineComponent<IRenderer> m_pRenderer;		// Renderer Engine Component (DirectX9, DirectX11, OpenGL)
	EngineComponent<IResourcePool> m_pResourcePool;	// Common Resource Pool handling Vertex-, Index-, Texture-, ...-buffers
	EngineComponent<IScene> m_pScene;
	EngineComponent<I3DEngine> m_p3DEngine;
	EngineComponent<IPhysics> m_pPhysics;
	EngineComponent<IEntityReceiptManager> m_pEntityReceiptManager;
	EngineComponent<FileLogListener> m_pFileLogListener;


	SResult CheckFinishInit();

	// returns next iterator after first found instance. If not found, increment cur (if possible) is returned.
	vector<IShutdownHandler*>::iterator UnregisterShutdownHandlerIntrnl(
		IShutdownHandler* pShutdownHandler, vector<IShutdownHandler*>::iterator cur);

public:	
	SpeedPointEngine();
	virtual ~SpeedPointEngine();

public: // Initialization
	virtual SResult Initialize(const SGameEngineInitParams& params);
	virtual SResult FinishInitialization();
	virtual void Shutdown();
	virtual bool IsRunning() const
	{
		return m_bRunning;
	}

public: // Component getters
	ILINE virtual ISettings* GetSettings() const { return m_pSettings; }
	ILINE virtual IPhysics* GetPhysics() const { return m_pPhysics; }
	ILINE virtual IRenderer* GetRenderer() const { return m_pRenderer; }
	ILINE virtual I3DEngine* Get3DEngine() const { return m_p3DEngine; }
	ILINE virtual IFontRenderer* GetFontRenderer() const { return m_pRenderer->GetFontRenderer(); }
	ILINE virtual IResourcePool* GetResources() const { return m_pResourcePool; }
	ILINE virtual IViewport* GetTargetViewport() const { return GetRenderer()->GetTargetViewport(); }
	ILINE virtual IScene* GetScene() const { return m_pScene; }
	ILINE virtual IEntityReceiptManager* GetEntityReceiptManager() const { return m_pEntityReceiptManager; }
	ILINE virtual IMaterialManager* GetMaterialManager() const;

public: // Game / Application interface
	virtual void RegisterApplication(IApplication* pApp);
	virtual IApplication* GetApplication() const
	{
		return m_pApplication;
	}

//	virtual SMaterial* GetDefaultMaterial() { return &m_DefaultMaterial; }

	virtual string GetShaderDirectoryPath() const;
	virtual string GetResourceSystemPath(const string& absResourcePath = "") const;

	virtual SResult LoadWorld(const string& absResourcePath);

	virtual void DoFrame();
};

SP_NMSPACE_END
