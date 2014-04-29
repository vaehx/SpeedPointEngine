// ********************************************************************************************

//	SpeedPoint Renderer specific Output Plane (abstract)

//	This is a virtual class. You cannot instantiate it.
//	Please use Specific Implementation to instantiate.

// ********************************************************************************************

#pragma once

#include <SPrerequisites.h>

namespace SpeedPoint
{
	// SpeedPoint Renderer specific Output Plane (abstract)
	class S_API IOutputPlane
	{
	public:
		// Initialize the output plane
		virtual SResult Initialize( SpeedPointEngine* pEngine, IRenderer* pRenderer, int nW, int nH ) = 0;

		// Render the output plane
		virtual SResult Render( IFrameBuffer* pGBufferAlbedo, IFrameBuffer* pLightingBuffer ) = 0;

		// Clearout the output plane
		virtual SResult Clear( void ) = 0;
	};
}