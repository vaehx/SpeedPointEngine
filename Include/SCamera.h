// ********************************************************************************************

//	SpeedPoint Camera

// ********************************************************************************************

#pragma once
#include "SPrerequisites.h"
#include "STransformable.h"

namespace SpeedPoint
{
	// SpeedPoint Camera
	class S_API SCamera : public STransformable
	{
	public:
		float	fViewRadius;		// Keep it short for performant rendering

		// Default constructor
		SCamera() : STransformable(), fViewRadius( 20.0f ) {};
	};
}