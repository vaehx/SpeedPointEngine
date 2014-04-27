// ********************************************************************************************

//	SpeedPoint Camera

// ********************************************************************************************

#pragma once
#include <SPrerequisites.h>
#include "STransformable.h"

namespace SpeedPoint
{
	// SpeedPoint Camera
	class S_API SCamera : public STransformable
	{
	private:
		f32	m_fViewRadius;		// Keep it short for performant rendering

	public:		
		// Default constructor
		SCamera()
			: STransformable(),
			m_fViewRadius(20.0f)
		{
		}

		// Get a view matrix from this camera
		SMatrix4 GetViewMatrix();

		// Get view radius of the camera
		f32 GetViewRadius() { return m_fViewRadius; }
	};
}