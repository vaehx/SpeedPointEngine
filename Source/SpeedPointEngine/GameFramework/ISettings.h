////////////////////////////////////////////////////////////////////////////////////////////////////////
//	SpeedPoint Engine Source File
//
//	(c) 2011-2016 Pascal Rosenkranz
//	All rights reserved.
////////////////////////////////////////////////////////////////////////////////////////////////////////

#pragma once

#include <Common\SPrerequisites.h>
#include <Common\SColor.h>

SP_NMSPACE_BEG

struct S_API IGameEngine;



// specifying in which direction the vertices are set if the polygon is front-faced





enum S_API EShadowQuality
{
	eSHADOWQUALITY_NOSHADOW
	//		eSHADOWQUALITY_LOW,	// uses simple pcf
	//		eSHADOWQUALITY_NORMAL,	// uses short-radius random lookup pcf
	//		eSHADOWQUALITY_HIGH	// uses random lookup pcf and screenspace blur
};






////////////////////////////////////////////////////////////////////////////////////////////////////////
// Summary:
//	Window and video and application (client) settings	
struct S_API SApplicationSettings
{
	HWND		hWnd;			// Window for the main viewport
	int		nXResolution;		// X-Resolution of the main viewport
	int		nYResolution;		// Y-Resolution of the main viewport
	bool		bMultithreaded;		// set to true if you render from a different thread than the message queue
	bool		bWindowed;		// set to true if application runs in windowed mode						
};


////////////////////////////////////////////////////////////////////////////////////////////////////////
struct S_API SResourceSettings
{
	string worldFile; // filename of loaded world, relative to rootDir
};


////////////////////////////////////////////////////////////////////////////////////////////////////////
// Settings belonging to rendering
struct S_API SRenderSettings
{
	S_RENDERER_TYPE tyRendererType; // maybe put this into a separate struct Iface sometimes

	SColor clearColor;

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
};


////////////////////////////////////////////////////////////////////////////////////////////////////////

struct S_API SSettingsDesc
{
	SApplicationSettings app; // Window / device / video mode settings
	SRenderSettings render; // Rendering settings	 
	SResourceSettings resources;

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

		// Resources
		resources.worldFile = o.resources.worldFile;
	}
};




struct S_API ISettings
{
	virtual ~ISettings()
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


SP_NMSPACE_END
