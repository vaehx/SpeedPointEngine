// SpeedPoint Engine Settings

#include <SSettings.h>

namespace SpeedPoint
{

	// **********************************************************************************

	S_API SSettings::SSettings()
		: tyRendererType(S_DIRECTX9),
		hWnd(NULL),
		nXResolution(1024),
		nYResolution(768),
		bMultithreaded(false),
		bWindowed(true),
		bLighting(false),	///// TODO: Switch this to true as soon as lighting shader is implemented
		bPhysics(false),	//// TODO: Switch this to true as soon as physics is implemented
		bSound(false),	//// TOOD: Switch this to true as soon as sound system is implemented		
		bEnableTextures(false),	// TODO: Swith this to true as soon as non-texturing was tested
		fClipNear(2.0f),
		fClipFar(200.0f),
		pTargetViewport(NULL),

		fFogNear(20.0f),
		fFogFar(40.0f),
		fFogDensity(0.08f),
		colFogColor(SColor(0.1f, 0.1f, 0.1f))
	{
	}

	// **********************************************************************************

	S_API SSettings::SSettings(const SSettings& o)
		: tyRendererType(o.tyRendererType),
		hWnd(o.hWnd),
		nXResolution(o.nXResolution),
		nYResolution(o.nYResolution),
		bMultithreaded(o.bMultithreaded),
		bWindowed(o.bWindowed),
		bLighting(o.bLighting),
		bPhysics(o.bPhysics),
		bSound(o.bSound),
		bEnableTextures(o.bEnableTextures),
		fClipNear(o.fClipNear),
		fClipFar(o.fClipFar),
		pTargetViewport(o.pTargetViewport),

		fFogNear(o.fFogNear),
		fFogFar(o.fFogFar),
		fFogDensity(o.fFogDensity),
		colFogColor(o.colFogColor)
	{
	}

}