//////////////////////////////////////////////////////////////////////////////////////////////////
//
//	SpeedPoint Game Engine
//	Copyright (c) 2011-2017 Pascal Rosenkranz, All rights reserved.
//
//////////////////////////////////////////////////////////////////////////////////////////////////

#pragma once

#include <Abstract\IGameEngine.h>
#include <Abstract\IResourcePool.h>
#include <Abstract\IRenderer.h>
#include <Abstract\IFont.h>
#include <Abstract\I3DEngine.h>
#include <Abstract\SPrerequisites.h>

//////////////////////////////////////////////////////////////////////////////////////////////////

SP_NMSPACE_BEG

struct S_API IViewport;
struct S_API IRenderer;
class S_API FileLogListener;

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

	EngineComponent<IFramePipeline> m_pFramePipeline;
	EngineComponent<IRenderer> m_pRenderer;		// Renderer Engine Component (DirectX9, DirectX11, OpenGL)
	EngineComponent<IFontRenderer> m_pFontRenderer;
	EngineComponent<IResourcePool> m_pResourcePool;	// Common Resource Pool handling Vertex-, Index-, Texture-, ...-buffers
	EngineComponent<IScene> m_pScene;
	EngineComponent<IMaterialManager> m_pMaterialManager;
	EngineComponent<I3DEngine> m_p3DEngine;
	EngineComponent<IPhysics> m_pPhysics;
	EngineComponent<IEntityReceiptManager> m_pEntityReceiptManager;
	EngineComponent<FileLogListener> m_pFileLogListener;

	std::vector<IShutdownHandler*> m_ShutdownHandlers;	

	void CheckFinishInit();

public:	
	SpeedPointEngine(const char* logFileName);
	SpeedPointEngine() : SpeedPointEngine("SpeedPoint.log") {}
	virtual ~SpeedPointEngine();


	virtual void RegisterApplication(IApplication* pApp);
	virtual IApplication* GetApplication() const
	{
		return m_pApplication;
	}

	virtual void RegisterShutdownHandler(IShutdownHandler* pShutdownHandler);
	virtual void UnregisterShutdownHandler(IShutdownHandler* pShutdownHandler);

	// returns next iterator after first found instance. If not found, increment cur (if possible) is returned.
	virtual std::vector<IShutdownHandler*>::iterator UnregisterShutdownHandlerIntrnl(
		IShutdownHandler* pShutdownHandler, std::vector<IShutdownHandler*>::iterator cur);

	virtual void Shutdown();

	virtual SResult InitializeFramePipeline(IFramePipeline* pCustomFramePipeline = 0);
	virtual SResult InitializeRenderer(const S_RENDERER_TYPE& type, IRenderer* pRender, bool bManageDealloc = true);
	virtual SResult Initialize3DEngine(I3DEngine* p3DEngine, bool bManageDealloc = true);
	virtual SResult InitializeFontRenderer();
	virtual SResult InitializeResourcePool(IMaterialManager* pMatMgr, bool bManageDealloc = true);
	virtual SResult InitializePhysics();
	virtual SResult InitializeScene(IScene* pScene);


public:
	virtual SResult FinishInitialization();
	virtual SResult ExecuteFramePipeline(usint32 iSkippedSections = DEFAULT_SKIPPED_SECTIONS);	

	// Budgeting System
public:
	virtual unsigned int StartBudgetTimer(const char* name);
	virtual void ResumeBudgetTimer(unsigned int id);
	virtual void StopBudgetTimer(unsigned int id);

public:
	virtual bool IsRunning() const
	{
		return m_bRunning;
	}

	ILINE virtual ISettings* GetSettings() const { return m_pSettings; }
	ILINE virtual IFramePipeline* GetFramePipeline() const { return m_pFramePipeline; }
	ILINE virtual IPhysics* GetPhysics() const { return m_pPhysics; }
	ILINE virtual IRenderer* GetRenderer() const { return m_pRenderer; }	
	ILINE virtual I3DEngine* Get3DEngine() const { return m_p3DEngine; }
	ILINE virtual IFontRenderer* GetFontRenderer() const { return m_pFontRenderer; }
	ILINE virtual IResourcePool* GetResources() const { return m_pResourcePool; }
	ILINE virtual IViewport* GetTargetViewport() const { return GetRenderer()->GetTargetViewport(); }
	ILINE virtual IScene* GetScene() const { return m_pScene; }
	ILINE virtual IEntityReceiptManager* GetEntityReceiptManager() const { return m_pEntityReceiptManager; }
	ILINE virtual IMaterialManager* GetMaterialManager() const { return m_pMaterialManager; }

//	virtual SMaterial* GetDefaultMaterial() { return &m_DefaultMaterial; }

	virtual string GetShaderPath(EShaderFileType shader) const;
	virtual string GetResourcePath(const string& file = "") const;

	virtual void LoadWorld(const string& file);
};

SP_NMSPACE_END
