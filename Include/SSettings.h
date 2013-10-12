// ********************************************************************************************

//	SpeedPoint Engine Settings

// ********************************************************************************************

#pragma once

#include <SPrerequisites.h>
#include "SColor.h"

namespace SpeedPoint
{
	// SpeedPoint Engine settings
	struct S_API SSettings
	{
		//bool bParallelRendering;	// Use Parallel Rendering?		
		S_RENDERER_TYPE tyRendererType;

		// -- basic settings --	
		HWND	hWnd;			// Window for the main viewport
		int	nXResolution;		// X-Resolution of the main viewport
		int	nYResolution;		// Y-Resolution of the main viewport
		bool	bMultithreaded;		// Use Multithreading of renderer if possible
		bool	bWindowed;		// Is the default viewport in windowed mode or fullscren
		bool	bWireframe;		// turn on/off wireframe
		bool	bLighting;		// Calculate lighting?
		bool	bPhysics;		// Calulcate Physics?
		bool	bSound;			// Play sounds?
		int	nLOD;			// Maximum Level of detail (for automatic LOD)
		float	fClipNear;		// Near-Clipping depth
		float	fClipFar;		// Far-Clipping depth
		SViewport* pTargetViewport;	// Target viewport of the engine
		
		// -- Simulated Fog by integrated Fog shader --
		float	fFogNear;		// Nearest depth of Fog interpolation
		float	fFogFar;		// End depth of Fog interpolation
		float	fFogDensity;		// Density factor for the fog
		SColor	colFogColor;		// Color of the fog to interpolate to

		/// -------

		// Default constructor
		SSettings();

		// Copy constructor
		SSettings( const SSettings& o );
	};
}