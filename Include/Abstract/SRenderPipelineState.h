// ********************************************************************************************

//	SpeedPoint Render Pipeline State

//	This is a virtual class. You cannot instantiate it.
//	Please use Specific Implementation to instantiate.

// ********************************************************************************************

#pragma once
#include <SPrerequisites.h>

namespace SpeedPoint
{
	// SpeedPoint Render Pipeline State (abstract)
	class S_API SRenderPipelineState
	{
	public:
		// Initialize the Render Pipeline State
		virtual SResult Initialize( SpeedPointEngine* pEngine, SRenderer* pRenderer ) = 0;

		// The actual Render call
		virtual SResult Render( void );

		// Clear the Render Pipeline state
		virtual SResult Clear( void ) = 0;
	};
}