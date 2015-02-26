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
#include <fstream>

//////////////////////////////////////////////////////////////////////////////////////////////////

SP_NMSPACE_BEG

//////////////////////////////////////////////////////////////////////////////////////////////////
// forward declarations

struct S_API IViewport;
struct S_API IRenderer;




class S_API EngineLog : public ILog
{
private:
	ELogLevel m_LogLevel;	
	std::ofstream m_LogFile;
	std::vector<ILogHandler*> m_LogHandlers;

public:
	~EngineLog() {}

	virtual void Clear();
	virtual SResult SetLogFile(const SString& file);
	virtual SResult RegisterLogHandler(ILogHandler* pLogHandler);
	virtual SResult SetLogLevel(ELogLevel loglevel);
	virtual ELogLevel GetLogLevel() const;
	virtual SResult Log(SResult res, const SString& msg);
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

	operator T*() const
	{
		return pComponent;
	}

	T* operator ->() const { return pComponent; }
};

// SpeedPoint Game Engine contains all parts of the default SpeedPoint Engine components (Frame, Physics, Rendering, ...)
// If you want to use custom components you can either hook your components into the other components
class S_API SpeedPointEngine : public IExceptionProxy, public IGameEngine
{
private:	
	IApplication*		m_pApplication;

	bool			m_bRunning;		// Is the Game Engine started and initialized?
	bool			m_bLoggedSkipstages;
	IEngineSettings*	m_pSettings;		// Main Settings of the Game Engine	

	EngineComponent<IFramePipeline> m_pFramePipeline;
	EngineComponent<IRenderer> m_pRenderer;		// Renderer Engine Component (DirectX9, DirectX11, OpenGL)
	EngineComponent<IFontRenderer> m_pFontRenderer;
	EngineComponent<IResourcePool> m_pResourcePool;	// Common Resource Pool handling Vertex-, Index-, Texture-, ...-buffers
	EngineComponent<IScene> m_pScene;
	ILog* m_pLog;

	std::vector<IShutdownHandler*> m_ShutdownHandlers;

	SMaterial m_DefaultMaterial;

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
	virtual SResult InitializeFontRenderer();
	virtual SResult InitializeResourcePool();
	virtual SResult InitializeLogger(ILogHandler* pCustomLogHandler = 0);
	virtual SResult InitializeScene(IScene* pScene);


public:
	virtual SResult FinishInitialization();
	virtual SResult ExecuteFramePipeline(usint32 iSkippedSections = DEFAULT_SKIPPED_SECTIONS);	

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


public:
	virtual bool IsRunning() const
	{
		return m_bRunning;
	}

	virtual IEngineSettings* GetSettings() const { return m_pSettings; }
	virtual IFramePipeline* GetFramePipeline() const { return m_pFramePipeline; }
	virtual IRenderer* GetRenderer() const { return m_pRenderer; }	
	virtual IFontRenderer* GetFontRenderer() const { return m_pFontRenderer; }
	virtual IResourcePool* GetResources() const { return m_pResourcePool; }
	virtual ILog* GetLog() const { return m_pLog; }	
	virtual IViewport* GetTargetViewport() const { return GetRenderer()->GetTargetViewport(); }
	virtual IScene* GetLoadedScene() const { return m_pScene; }

	virtual SMaterial* GetDefaultMaterial() { return &m_DefaultMaterial; }

	virtual SString GetShaderPath(EShaderType shader);
};


SP_NMSPACE_END