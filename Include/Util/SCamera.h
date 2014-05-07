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
		f32		m_fViewRadius;		// Keep it short for performant rendering
		SVector3	m_vLookAt;
		SMatrix4	m_ViewMatrix;
		bool		m_bViewMatrixCalculated;	// has the view matrix ever been calculated?

	public:		
		// Default constructor
		SCamera()
			: STransformable(),
			m_fViewRadius(20.0f),
			m_bViewMatrixCalculated(false)
		{
		}

		~SCamera()			
		{
			m_bViewMatrixCalculated = false;
		}

		// Get a view matrix from this camera
		SMatrix4& GetViewMatrix();

		// Summary:
		//	Recalculate the view matrix
		//	should only be called by the camera itself, but is public to be more flexible, if you want to do it anways...
		// Return Value:
		//	Returns new view matrix as const ref
		SMatrix4& RecalculateViewMatrix();

		// Get view radius of the camera
		f32 GetViewRadius() { return m_fViewRadius; }
	};
}