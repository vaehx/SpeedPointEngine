// ********************************************************************************************

//	SpeedPoint Engine Settings

// ********************************************************************************************

#pragma once

#include <SPrerequisites.h>
#include <Util\SColor.h>

namespace SpeedPoint
{
	// SpeedPoint Engine settings
	struct S_API SSettings
	{
		//bool bParallelRendering;	// Use Parallel Rendering?		
		S_RENDERER_TYPE tyRendererType;

		// Window / device settings
		HWND		hWnd;			// Window for the main viewport
		int		nXResolution;		// X-Resolution of the main viewport
		int		nYResolution;		// Y-Resolution of the main viewport
		bool		bMultithreaded;		// Use Multithreading of renderer if possible
		bool		bWindowed;		// Is the default viewport in windowed mode or fullscren						
		SViewport*	pTargetViewport;	// Target viewport of the engine
		bool		bSound;			// Play sounds?

		// Dynamics Stage settings
		bool		bPhysics;		// Calculate Physics?			

		// Geometry Section settings
		bool		bEnableTextures;	// Textures enabled?
		float		fClipNear;		// Near-Clipping depth
		float		fClipFar;		// Far-Clipping depth

		// Lighting section settings
		bool		bLighting;		// Calculate lighting?
		bool		bCustomLighting;	// prevents lights-buffer to be created automatically before event is called
		
		// Wireframe shader
		bool		bWireframe;		// is the wireframe shader enabled or not

		// Fog Shader settings
		bool		bEnableFog;		// is the fog shader enabled or not
		float		fFogNear;		// Nearest depth of Fog interpolation
		float		fFogFar;		// End depth of Fog interpolation
		float		fFogDensity;		// Density factor for the fog
		SColor		colFogColor;		// Color of the fog to interpolate to

		/// -------

		// Default constructor
		SSettings();

		// Copy constructor
		SSettings( const SSettings& o );
	};
}