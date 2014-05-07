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


		// We need to overwrite all transformation methods as we want to keep track of the modification of the look-at point
		// and the view matrix

		// Move absolutely
		void Move(const SVector3& pos);

		// Move absolutely
		void Move(float x, float y, float z);

		// Translate relatively
		void Translate(const SVector3& vec);

		// Translate relatively
		void Translate(float x, float y, float z);

		// Rotate absolutely
		void Rotate(const SVector3& rotation);

		// Rotate absolutely
		void Rotate(float p, float y, float r);

		// Turn relatively
		void Turn(const SVector3& vec);

		// Turn relatively
		void Turn(float p, float y, float r);

		// will do nothing, as u cannot scale the camera
		void Size(const SVector3& size);

		// will do nothing, as u cannot scale the camera
		void Size(float w, float h, float d);

		// will do nothing, as u cannot scale the camera
		void Scale(const SVector3& vec);

		// will do nothing, as u cannot scale the camera
		void Scale(float w, float h, float d);

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