////////////////////////////////////////////////////////////////////////////////////////////////////////
//	SpeedPoint Engine Source File
//
//	(c) 2011-2014 Pascal Rosenkranz aka iSmokiieZz
//	All rights reserved.
////////////////////////////////////////////////////////////////////////////////////////////////////////

#pragma once

#include <SPrerequisites.h>
#include <Util\SColor.h>

namespace SpeedPoint
{
	// specifying in which direction the vertices are set if the polygon is front-faced
	enum S_API EFrontFace
	{
		eFF_CCW,	// counter-clock-wise
		eFF_CW		// clock-wise
	};

	// SpeedPoint Engine settings
	struct S_API SSettings
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
			bool		bSound;			// Play sounds?
			bool		bPhysics;		// Calculate Physics?			

			// Maximum count of child instances of a scene node
			// hint: we use fixed array size for scene node childs to avoid resizing in runtime. you can			
			unsigned int	nMaxSceneNodeChilds;

			// set bAllowNodeChildsOverflow to true if you want to bypass this and use nMaxSceneNodeChilds as
			// the initial size of the child nodes array
			bool		bAllowNodeChildsOverflow;
			
			float		fFogNear;		// Nearest depth of Fog interpolation
			float		fFogFar;		// End depth of Fog interpolation
			float		fFogDensity;		// Density factor for the fog
			SColor		colFogColor;		// Color of the fog to interpolate to
		} env; // Settings for Scene components and its beheaviour

		////////////////////////////////////////////////////////////////////////////////////////////////////////

		// Settings belonging to rendering
		struct RenderSet
		{
			S_RENDERER_TYPE tyRendererType; // maybe put this into a separate struct Iface sometimes

			bool		bEnableVSync;	// (default 1) is not evaluated by DX9 !			
			bool		bEnableMSAA;

			bool		bRenderTextures;	// Textures enabled?
			float		fClipNear;		// Near-Clipping depth
			float		fClipFar;		// Far-Clipping depth

			bool		bRenderWireframe;		// is the wireframe shader enabled or not						
			EFrontFace	frontFaceType; 

			bool		bRenderLighting;		// Render lighting although it is set up in environment?			
			bool		bCustomLighting;	// prevents lights-buffer to be updated automatically if light added/removed or camera moved

			bool		bRenderFog;		// is the fog shader enabled or not
		} render; // Rendering settings	

		////////////////////////////////////////////////////////////////////////////////////////////////////////		

		// Default constructor
		SSettings::SSettings()
		{
			// ApplicationSet
			app.hWnd = 0;
			app.nXResolution = 1024;
			app.nYResolution = 768;
			app.bMultithreaded = false;
			app.bWindowed = true;

			// EnvironmentSet
			env.bPhysics = false;	//// TODO: Switch this to true as soon as physics is implemented
			env.bSound = false;	//// TOOD: Switch this to true as soon as sound system is implemented		
			env.nMaxSceneNodeChilds = 10;
			env.bAllowNodeChildsOverflow = false;
			env.fFogNear = 20.0f;
			env.fFogFar = 40.0f;
			env.fFogDensity = 0.08f;
			env.colFogColor = SColor(0.1f, 0.1f, 0.1f);

			// RenderSet
			render.tyRendererType = S_DIRECTX11;
			render.bEnableVSync = true;
			render.bEnableMSAA = false;	// TODO: switch this to true as soon as everything else works
			render.bRenderTextures = false;	// TODO: Swith this to true as soon as non-texturing was tested
			render.fClipNear = 2.0f;
			render.fClipFar = 200.0f;
			render.bRenderLighting = false;	///// TODO: Switch this to true as soon as lighting shader is implemented									
			render.bRenderWireframe = false;
			render.frontFaceType = eFF_CW;
			render.bRenderFog = false;
			render.bCustomLighting = false;
		}

		// Default copy constructor
		SSettings::SSettings(const SSettings& o)			
		{
			// ApplicationSet
			app.hWnd = o.app.hWnd;
			app.nXResolution = o.app.nXResolution;
			app.nYResolution = o.app.nYResolution;
			app.bMultithreaded = o.app.bMultithreaded;
			app.bWindowed = o.app.bWindowed;

			// EnvironmentSet
			env.bSound = o.env.bSound;
			env.bPhysics = o.env.bPhysics;
			env.nMaxSceneNodeChilds = o.env.nMaxSceneNodeChilds;
			env.bAllowNodeChildsOverflow = o.env.bAllowNodeChildsOverflow;
			env.fFogNear = o.env.fFogNear;
			env.fFogFar = o.env.fFogFar;
			env.fFogDensity = o.env.fFogDensity;
			env.colFogColor = o.env.colFogColor;

			// RenderSet
			render.tyRendererType = o.render.tyRendererType;
			render.bEnableVSync = o.render.bEnableVSync;
			render.bEnableMSAA = o.render.bEnableMSAA;
			render.bRenderTextures = o.render.bRenderTextures;
			render.fClipFar = o.render.fClipFar;
			render.fClipNear = o.render.fClipNear;
			render.frontFaceType = o.render.frontFaceType;
			render.bRenderWireframe = o.render.bRenderWireframe;
			render.bRenderLighting = o.render.bRenderLighting;
			render.bCustomLighting = o.render.bCustomLighting;
			render.bRenderFog = o.render.bRenderFog;
		}
	};
}