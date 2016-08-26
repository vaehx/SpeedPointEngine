////////////////////////////////////////////////////////////////////////////////////////////////////////

//	SpeedPoint Game Engine Source Code

//	written by iSmokiieZz
//	(c) 2011 - 2014, All rights reserved.

////////////////////////////////////////////////////////////////////////////////////////////////////////

#pragma once
#include <SPrerequisites.h>
#include <Util\SColor.h>
#include "Abstract\IFramePipeline.h"	// for default skipsections
#include <Util\SLogStream.h>
#include "Matrix.h"
#include "Vector3.h"

namespace SpeedPoint
{

	struct S_API IFramePipeline;	
	struct S_API IRenderer;
	struct S_API I3DEngine;
	struct S_API IFontRenderer;
	//struct S_API IPhysics;
	//struct S_API IAI;
	struct S_API IApplication;
	//struct S_API IScriptEngine;
	struct S_API IResourcePool;
	struct S_API IFileLogHandler;
	struct S_API IFileLog;
	class CLogWrapper;	
	struct S_API IScene;
	struct S_API IMaterial;
	struct S_API IMaterialManager;
	struct S_API ISettings;
	struct S_API IEntitySystem;
	struct S_API IPhysics;

	enum EShaderFileType
	{
		eSHADERFILE_SKYBOX,
		
		eSHADERFILE_FORWARD_HELPER,	// Forward helper effect
		eSHADERFILE_FORWARD,

		eSHADERFILE_FONT,	// forward font effect

		eSHADERFILE_DEFERRED_ZPASS,	// ZPass for Deferred Shading
		eSHADERFILE_DEFERRED_SHADING,	// shading pass for Deferred Shading
		eSHADERFILE_TERRAIN,	// Deferred Shading Terrain
	};



	// -------------------------------------------------------------------------

/*

	// use 64 bits for engsetting mask
#define ENGSETTINGMASKLENGTH 64

	// Note: Do not use enum for this due to restriction of enum to 32 bits.
	
	// Current restriction: 64 bits! (using long long)
	// Try to group settings as much as possible or find another approach instead of putting it all into engine settings!
	// If needed to raise, use __int128 for SSettingsDesc::mask

#define ENGSETTING_NONE			0x00
#if ENGSETTINGMASKLENGTH == 64
	#define ENGSETTING_ALL 0xffffffffffffffff
	#define ENGSETTING_MASKTYPE usint64
#else
	#define ENGSETTING_ALL 0xffffffff
	#define ENGSETTING_MASKTYPE usint32
#endif

#define ENGSETTING_HWND			BITN(1)
#define ENGSETTING_RESOLUTION		BITN(2)
#define ENGSETTING_MULTITHREADING	BITN(3)
#define ENGSETTING_WINDOWEDMODE		BITN(4)
//#define ENGSETTING_ENABLESOUND		BITN(5)
//#define ENGSETTING_ENABLEPHYSICS	BITN(6)
#define ENGSETTING_ENABLEVSYNC		BITN(7)
#define ENGSETTING_AAQUALITY		BITN(8)
#define ENGSETTING_SHADOWQUALITY	BITN(9)
#define ENGSETTING_FRONTFACE_TYPE	BITN(10)
#define ENGSETTING_LIGHTING		BITN(11)
#define ENGSETTING_WIREFRAME		BITN(12)
#define ENGSETTING_CLIPPLANES		BITN(13)
#define ENGSETTING_ENABLETEXTURES	BITN(14)
#define ENGSETTING_RENDERAPI		BITN(15)
#define ENGSETTING_VSYNCINTERVAL	BITN(16)
#define ENGSETTING_TERRAINDMFADERADIUS	BITN(17)

*/



	// Summary:
	//	An object that keeps a pointer to the engine or the renderer can implement this interface
	//	to get informed about the shutdown. This prevents memory access violations.
	//
	// TODO: Maybe better do this using smart pointers to the engine / the renderer someday
	struct S_API IShutdownHandler
	{
		virtual SResult HandleShutdown() = 0;
		virtual SString GetShutdownHandlerDesc() const { return SString("unknown"); }
	};






	// -------------------------------------------------------------------------

	// A factory to bundle all important modules of an engine
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
//		virtual SResult InitializeAI() = 0;
//		virtual SResult InitializeScriptEngine() = 0;		
		virtual SResult InitializeLogger(IFileLogHandler* pCustomFileLogHandler = 0) = 0;

		// Inits Renderer specific resource pool and the material manager
		virtual SResult InitializeResourcePool(IMaterialManager* pMatMgr, bool bManageDealloc = true) = 0;		
		
		// instance is deleted by engine on shutdown
		virtual SResult InitializeScene(IScene* pScene) = 0;

		virtual bool IsRunning() const = 0;
		virtual void RegisterShutdownHandler(IShutdownHandler* pShutdownHandler) = 0;
		virtual void UnregisterShutdownHandler(IShutdownHandler* pShutdownHandler) = 0;
		virtual void Shutdown() = 0;		


		ILINE virtual IFramePipeline* GetFramePipeline() const = 0;
		ILINE virtual IEntitySystem* GetEntitySystem() const = 0;
		ILINE virtual IPhysics* GetPhysics() const = 0;
		ILINE virtual IRenderer* GetRenderer() const = 0;	
		ILINE virtual I3DEngine* Get3DEngine() const = 0;
		ILINE virtual IFontRenderer* GetFontRenderer() const = 0;
//		ILINE virtual IAI* GetAI() const = 0;
//		ILINE virtual IScriptEngine* GetScriptEngine() const = 0;
		ILINE virtual IResourcePool* GetResources() const = 0;
		ILINE virtual CLogWrapper* GetLog() = 0;
		ILINE virtual IFileLog* GetFileLog() = 0;
		ILINE virtual ISettings* GetSettings() const = 0;	
		ILINE virtual IScene* GetLoadedScene() const = 0;
		ILINE virtual IMaterialManager* GetMaterialManager() const = 0;

		virtual string GetShaderPath(EShaderFileType shaderFile) const = 0;

		
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

		virtual SResult LogReport(const SResult& res, const SString& msg) = 0;
		virtual SResult LogI(const SString& msg) = 0;
		virtual SResult LogE(const SString& msg) = 0;
		virtual SResult LogD(const SString& msg, SResultType defRetVal = S_DEBUG) = 0;
		virtual void LogD(const SMatrix4& mtx, const SString& mtxname) = 0;
		virtual void LogD(const SVector3& vec, const SString& vecname) = 0;
		virtual void LogD(bool b, const SString& boolname) = 0;
		virtual void LogD(unsigned int i, const SString& intname) = 0;
		virtual void LogD(float f, const SString& floatname) = 0;
		virtual void LogD(const SString& str, const SString& strname) = 0;
		virtual SResult LogW(const SString& msg) = 0;
	};	




	// do not use typedef for this to allow easy forward declaration

	struct S_API ISpeedPointEngine : public IGameEngine
	{
		virtual ~ISpeedPointEngine()
		{
		}
	};


	// Useful logging methods that shortens the check if pEng is a valid ptr and supports varargs.
	// TODO: Support varargs, How to access varargs???
	inline static SpeedPoint::SResult EngLog(const SpeedPoint::SResult& r, SpeedPoint::IGameEngine* pEng, const char* fmt, ...)
	{
		va_list args;

		char pOut[1000];
		va_start(args, fmt);		
		vsprintf_s(pOut, fmt, args);
		va_end(args);

		if (IS_VALID_PTR(pEng))
		{
			return pEng->LogReport(r, pOut);
		}
		else
		{
			printf(pOut);
			return r;
		}
	}




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
}