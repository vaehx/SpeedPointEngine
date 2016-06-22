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
#include "Abstract\ILog.h"

#include "Abstract\IGameEngine.h"
#include "Abstract\IResourcePool.h"
#include "Abstract\IRenderer.h"
#include "Abstract\IFont.h"
#include "Abstract\I3DEngine.h"
#include <fstream>
#include <deque>

//////////////////////////////////////////////////////////////////////////////////////////////////

using std::deque;

SP_NMSPACE_BEG

//////////////////////////////////////////////////////////////////////////////////////////////////
// forward declarations

struct S_API IViewport;
struct S_API IRenderer;



class S_API EngineFileLog : public IFileLog
{
private:
	struct Line
	{
		string text;
	};

	ELogLevel m_LogLevel;	
	std::ofstream m_LogFile;
	std::vector<IFileLogHandler*> m_LogHandlers;
	deque<Line> m_IOQueue;

public:
	~EngineFileLog() {}

	virtual void Clear();
	virtual SResult SetLogFile(const SString& file);
	virtual SResult RegisterLogHandler(IFileLogHandler* pLogHandler);
	virtual SResult SetLogLevel(ELogLevel loglevel);
	virtual ELogLevel GetLogLevel() const;
	virtual SResult Log(SResult res, const SString& msg);
	virtual void ReleaseIOQueue();
};

class CLogWrapper
{
public:
	SResult Log(SResult res, const SString& msg)
	{
		return CLog::Log(res, msg);
	}
	SResult LogE(const SString& msg) { return Log(S_ERROR, msg); }
	SResult LogI(const SString& msg) { return Log(S_INFO, msg); }
	SResult LogW(const SString& msg) { return Log(S_WARN, msg); }
	SResult LogD(const SString& msg) { return Log(S_DEBUG, msg); }

	SResult SetLogLevel(ELogLevel logLevel)
	{
		return S_SUCCESS;
	}
};




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

// SpeedPoint Game Engine contains all parts of the default SpeedPoint Engine components (Frame, Physics, Rendering, ...)
// If you want to use custom components you can either hook your components into the other components
class S_API SpeedPointEngine : public IExceptionProxy, public IGameEngine, public ILogListener
{
private:	
	IApplication*		m_pApplication;

	bool			m_bRunning;		// Is the Game Engine started and initialized?
	bool			m_bLoggedSkipstages;
	ISettings*		m_pSettings;		// Main Settings of the Game Engine	

	EngineComponent<IFramePipeline> m_pFramePipeline;
	EngineComponent<IEntitySystem> m_pEntitySystem;
	EngineComponent<IRenderer> m_pRenderer;		// Renderer Engine Component (DirectX9, DirectX11, OpenGL)
	EngineComponent<IFontRenderer> m_pFontRenderer;
	EngineComponent<IResourcePool> m_pResourcePool;	// Common Resource Pool handling Vertex-, Index-, Texture-, ...-buffers
	EngineComponent<IScene> m_pScene;
	EngineComponent<IMaterialManager> m_pMaterialManager;
	EngineComponent<I3DEngine> m_p3DEngine;
	EngineComponent<IPhysics> m_pPhysics;
	EngineFileLog m_FileLog;
	CLogWrapper m_LogWrapper;

	std::vector<IShutdownHandler*> m_ShutdownHandlers;	

	void CheckFinishInit();

public:
	SpeedPointEngine();
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
	virtual SResult InitializeLogger(IFileLogHandler* pCustomLogHandler = 0);
	virtual SResult InitializeScene(IScene* pScene);


public:
	virtual SResult FinishInitialization();
	virtual SResult ExecuteFramePipeline(usint32 iSkippedSections = DEFAULT_SKIPPED_SECTIONS);	

	// Budgeting System
public:
	virtual unsigned int StartBudgetTimer(const char* name);
	virtual void ResumeBudgetTimer(unsigned int id);
	virtual void StopBudgetTimer(unsigned int id);

// Logging methods
public:	
	virtual SResult LogReport(const SResult& res, const SString& msg);
	virtual SResult LogE(const SString& msg);
	virtual SResult LogW(const SString& msg);
	virtual SResult LogI(const SString& msg);
	virtual SResult LogD(const SString& msg, SResultType defRetVal = S_DEBUG);
	virtual void LogD(const SMatrix4& mtx, const SString& mtxname);
	virtual void LogD(const SVector3& vec, const SString& vecname);
	virtual void LogD(bool b, const SString& boolname);
	virtual void LogD(unsigned int i, const SString& intname);
	virtual void LogD(float f, const SString& floatname);
	virtual void LogD(const SString& str, const SString& strname);

	// Implement IExceptionProxy methods
	virtual void HandleException(char* msg);	

	// Implement ILogListener
	virtual void OnLog(SResult res, const SString& msg);

public:
	virtual bool IsRunning() const
	{
		return m_bRunning;
	}

	ILINE virtual ISettings* GetSettings() const { return m_pSettings; }
	ILINE virtual IFramePipeline* GetFramePipeline() const { return m_pFramePipeline; }
	ILINE virtual IEntitySystem* GetEntitySystem() const { return m_pEntitySystem; }
	ILINE virtual IPhysics* GetPhysics() const { return m_pPhysics; }
	ILINE virtual IRenderer* GetRenderer() const { return m_pRenderer; }	
	ILINE virtual I3DEngine* Get3DEngine() const { return m_p3DEngine; }
	ILINE virtual IFontRenderer* GetFontRenderer() const { return m_pFontRenderer; }
	ILINE virtual IResourcePool* GetResources() const { return m_pResourcePool; }
	ILINE virtual IFileLog* GetFileLog() { return &m_FileLog; }	
	ILINE virtual CLogWrapper* GetLog() { return &m_LogWrapper; }	
	ILINE virtual IViewport* GetTargetViewport() const { return GetRenderer()->GetTargetViewport(); }
	ILINE virtual IScene* GetLoadedScene() const { return m_pScene; }
	ILINE virtual IMaterialManager* GetMaterialManager() const { return m_pMaterialManager; }

//	virtual SMaterial* GetDefaultMaterial() { return &m_DefaultMaterial; }

	virtual string GetShaderPath(EShaderFileType shader) const;
};


SP_NMSPACE_END