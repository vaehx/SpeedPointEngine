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

namespace SpeedPoint
{

	struct S_API IFramePipeline;	
	struct S_API IRenderer;
	struct S_API IFontRenderer;
	//struct S_API IPhysics;
	//struct S_API IAI;
	struct S_API IApplication;
	//struct S_API IScriptEngine;
	struct S_API IResourcePool;
	struct S_API ILogHandler;
	struct S_API ILog;
	struct S_API SVector3;
	struct S_API IScene;
	struct S_API SMaterial;



	enum EShaderType
	{
		eSHADER_FORWARD,
		eSHADER_TERRAIN,
		eSHADER_FONT
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


	// specifying in which direction the vertices are set if the polygon is front-faced
	enum S_API EFrontFace
	{
		eFF_CCW,	// counter-clock-wise
		eFF_CW		// clock-wise
	};

	enum S_API EAntiAliasingQuality
	{
		eAAQUALITY_LOW		// no antialiasing
//		eAAQUALITY_NORMAL,	// 2 sample MSAA
//		eAAQUALITY_HIGH		// special AA techniques (Gauss, Quincunx, FXAA, Adaptive AA, ...)
	};	


	enum S_API EShadowQuality
	{
		eSHADOWQUALITY_NOSHADOW
//		eSHADOWQUALITY_LOW,	// uses simple pcf
//		eSHADOWQUALITY_NORMAL,	// uses short-radius random lookup pcf
//		eSHADOWQUALITY_HIGH	// uses random lookup pcf and screenspace blur
	};


	struct S_API SSettingsDesc
	{		
		////////////////////////////////////////////////////////////////////////////////////////////////////////

		// Summary:
		//	Window and video and application (client) settings	
		struct ApplicationSet
		{
			HWND		hWnd;			// Window for the main viewport
			int		nXResolution;		// X-Resolution of the main viewport
			int		nYResolution;		// Y-Resolution of the main viewport
			bool		bMultithreaded;		// set to true if you render from a different thread than the message queue
			bool		bWindowed;		// set to true if application runs in windowed mode						
		} app; // Window / device / video mode settings

		////////////////////////////////////////////////////////////////////////////////////////////////////////

		// Summary:
		//	Environmental settings
		// Description:
		//	By "Environment" is not meant the coding environment. Its actually the preamble for
		//	Scene data and its beheaviour.
		struct EnvironmentSet
		{
//			bool		bSound;			// Play sounds?
//			bool		bPhysics;		// Calculate Physics?			

			// Maximum count of child instances of a scene node
			// hint: we use fixed array size for scene node childs to avoid resizing in runtime. you can			
			//unsigned int	nMaxSceneNodeChilds;

			// set bAllowNodeChildsOverflow to true if you want to bypass this and use nMaxSceneNodeChilds as
			// the initial size of the child nodes array
			//bool		bAllowNodeChildsOverflow;

			//float		fFogNear;		// Nearest depth of Fog interpolation
			//float		fFogFar;		// End depth of Fog interpolation
			//float		fFogDensity;		// Density factor for the fog
			//SColor		colFogColor;		// Color of the fog to interpolate to
		} env; // Settings for Scene components and its beheaviour

		////////////////////////////////////////////////////////////////////////////////////////////////////////

		// Settings belonging to rendering
		struct RenderSet
		{
			S_RENDERER_TYPE tyRendererType; // maybe put this into a separate struct Iface sometimes

			bool		bEnableVSync;
			unsigned int	vsyncInterval;
			EAntiAliasingQuality antiAliasingQuality;
			EShadowQuality shadowQuality;

			bool		bRenderTextures;	// Textures enabled?
			float		fClipNear;		// Near-Clipping depth
			float		fClipFar;		// Far-Clipping depth

			float		fTerrainDMFadeRange;	// Radius from camera where Detailmap of terrain should fade out

			bool		bRenderWireframe;		// is the wireframe shader enabled or not						
			EFrontFace	frontFaceType;

			bool		bEnableDepthTest;

			bool		bRenderLighting;		// Render lighting although it is set up in environment?			
			//bool		bCustomLighting;	// prevents lights-buffer to be updated automatically if light added/removed or camera moved

			//bool		bRenderFog;		// is the fog shader enabled or not
		} render; // Rendering settings	

		////////////////////////////////////////////////////////////////////////////////////////////////////////		

		// Default constructor
		SSettingsDesc::SSettingsDesc()
		{
			// ApplicationSet
			app.hWnd = 0;
			app.nXResolution = 1024;
			app.nYResolution = 768;
			app.bMultithreaded = false;
			app.bWindowed = true;

			// EnvironmentSet
//			env.bPhysics = false;	//// TODO: Switch this to true as soon as physics is implemented
//			env.bSound = false;	//// TOOD: Switch this to true as soon as sound system is implemented		
//			env.nMaxSceneNodeChilds = 10;
//			env.bAllowNodeChildsOverflow = false;
//			env.fFogNear = 20.0f;
//			env.fFogFar = 40.0f;
//			env.fFogDensity = 0.08f;
//			env.colFogColor = SColor(0.1f, 0.1f, 0.1f);

			// RenderSet
			render.tyRendererType = S_DIRECTX11;
			render.bEnableVSync = true;
			render.vsyncInterval = 2;
			render.antiAliasingQuality = eAAQUALITY_LOW;
			render.bRenderTextures = false;	// TODO: Swith this to true as soon as non-texturing was tested
			render.fClipNear = 2.0f;
			render.fClipFar = 200.0f;
			render.fTerrainDMFadeRange = 20.0f;
			render.bRenderLighting = false;	///// TODO: Switch this to true as soon as lighting shader is implemented									
			render.bRenderWireframe = false;			
			render.frontFaceType = eFF_CW;	
			render.bEnableDepthTest = true;
			render.shadowQuality = eSHADOWQUALITY_NOSHADOW;
		}

		// Default copy constructor
		SSettingsDesc::SSettingsDesc(const SSettingsDesc& o)
		{			
			// ApplicationSet
			app.hWnd = o.app.hWnd;
			app.nXResolution = o.app.nXResolution;
			app.nYResolution = o.app.nYResolution;
			app.bMultithreaded = o.app.bMultithreaded;
			app.bWindowed = o.app.bWindowed;

			// EnvironmentSet
//			env.bSound = o.env.bSound;
//			env.bPhysics = o.env.bPhysics;
//			env.nMaxSceneNodeChilds = o.env.nMaxSceneNodeChilds;
//			env.bAllowNodeChildsOverflow = o.env.bAllowNodeChildsOverflow;
//			env.fFogNear = o.env.fFogNear;
//			env.fFogFar = o.env.fFogFar;
//			env.fFogDensity = o.env.fFogDensity;
//			env.colFogColor = o.env.colFogColor;

			// RenderSet
			render.tyRendererType = o.render.tyRendererType;
			render.bEnableVSync = o.render.bEnableVSync;
			render.vsyncInterval = o.render.vsyncInterval;
			render.antiAliasingQuality = o.render.antiAliasingQuality;
			render.bRenderTextures = o.render.bRenderTextures;
			render.fClipFar = o.render.fClipFar;
			render.fClipNear = o.render.fClipNear;
			render.fTerrainDMFadeRange = o.render.fTerrainDMFadeRange;
			render.frontFaceType = o.render.frontFaceType;
			render.bEnableDepthTest = o.render.bEnableDepthTest;
			render.bRenderWireframe = o.render.bRenderWireframe;
			render.bRenderLighting = o.render.bRenderLighting;		
			render.shadowQuality = o.render.shadowQuality;
		}		
	};




	struct S_API IEngineSettings
	{	
		virtual ~IEngineSettings()
		{
		}

		// Summary:
		//	The ptr to game engine is used to immediately update the component settings.
		//	If nullptr is passed, then no settings will be set.
		virtual SResult Initialize(IGameEngine* pGameEngine) = 0;		

		virtual operator const SSettingsDesc&() const = 0;
		virtual SSettingsDesc& Get() = 0;



		// TODO: specific set functions

		virtual void SetTerrainDetailMapFadeRadius(float radius) = 0;
		virtual bool SetFrontFaceType(EFrontFace ffType) = 0;

		virtual void EnableWireframe(bool state = true) = 0;
		virtual void EnableDepthTest(bool state = true) = 0;
	};



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
//		virtual SResult InitializePhysics() = 0;
//		virtual SResult InitializeAI() = 0;
//		virtual SResult InitializeScriptEngine() = 0;		
		virtual SResult InitializeResourcePool() = 0;
		virtual SResult InitializeLogger(ILogHandler* pCustomLogHandler = 0) = 0;
		
		// instance is deleted by engine on shutdown
		virtual SResult InitializeScene(IScene* pScene) = 0;

		virtual bool IsRunning() const = 0;
		virtual void RegisterShutdownHandler(IShutdownHandler* pShutdownHandler) = 0;
		virtual void UnregisterShutdownHandler(IShutdownHandler* pShutdownHandler) = 0;
		virtual void Shutdown() = 0;		


		virtual IFramePipeline* GetFramePipeline() const = 0;
		virtual IRenderer* GetRenderer() const = 0;	
		virtual IFontRenderer* GetFontRenderer() const = 0;
//		virtual IPhysics* GetPhysics() const = 0;
//		virtual IAI* GetAI() const = 0;
//		virtual IScriptEngine* GetScriptEngine() const = 0;
		virtual IResourcePool* GetResources() const = 0;
		virtual ILog* GetLog() const = 0;	
		virtual IEngineSettings* GetSettings() const = 0;	
		virtual IScene* GetLoadedScene() const = 0;

		virtual SMaterial* GetDefaultMaterial() = 0;

		virtual SString GetShaderPath(EShaderType shader) = 0;

		
		// Summary:
		//	Will start the framepipeline and call the Update() and Render() callback functions of IApplication impl
		virtual SResult ExecuteFramePipeline(usint32 iSkippedSections = DEFAULT_SKIPPED_SECTIONS) = 0;

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
}