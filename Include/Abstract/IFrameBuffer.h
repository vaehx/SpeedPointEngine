// **************************************************************************************

//	SpeedPoint FrameBuffer Object

//	This is a virtual class. You cannot instantiate it.
//	Please use Specific Implementation to instantiate.

// **************************************************************************************

#pragma once

#include <SPrerequisites.h>

namespace SpeedPoint
{
	// SpeedPoint FrameBufferObject (abstract)
	class S_API IFrameBuffer
	{
	public:
		// initialize with given engine
		virtual SResult Initialize( SpeedPointEngine* pEngine, int nW, int nH ) = 0;

		// Clear buffers
		virtual void Clear( void ) = 0;

		////// TODO
	};
}