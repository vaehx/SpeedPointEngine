// ********************************************************************************************

//	SpeedPoint Camera

// ********************************************************************************************

#pragma once
#include <SPrerequisites.h>
#include "Transformable.h"
#include <math.h>

SP_NMSPACE_BEG




// Summary:
//	SpeedPoint Camera structure
struct S_API SCamera
{
	f32 fViewRadius;		// Keep it short for performant rendering	
	SMatrix4 viewMatrix;
	Vec3f position;
	Vec3f rotation;	// tait-bryan angles

	// Default constructor
	SCamera()
		: fViewRadius(20.0f),
		viewMatrix(0, 0, 0, 0, 0, 0, 0, 0, 0, 0, 0, 0, 0, 0, 0, 0)
	{
	}

	~SCamera()			
	{		
		viewMatrix = SMatrix4(0,0,0,0, 0,0,0,0, 0,0,0,0, 0,0,0,0);
	}


	// Summary:
	//	Calculate and store rotation angles by look direction to a specified point
	ILINE Vec3f& LookAt(const Vec3f& lookAt)
	{
		Vec3f dir = Vec3Normalize(lookAt - position);
		rotation.x = asinf(dir.y);
		rotation.y = acosf(dir.z / sqrtf(1.0f - (dir.y * dir.y)));
		
		// Todo: is there a way to eliminate dynamic branching here?
		if (dir.x < 0.0f)
			rotation.y = (2.0f * SP_PI) - rotation.y;

		rotation.z = 0.0f;
		return rotation;
	}


	// Summary:
	//	Recalculate the view matrix
	//	should only be called by the camera itself, but is public to be more flexible, if you want to do it anways...
	// Arguments:
	//	- roll: Set to true if you also want to take roll into account (rotation.z)
	// Return Value:
	//	Returns new view matrix as const ref
	ILINE SMatrix4& RecalculateViewMatrix(bool roll = false)
	{
		Vec3f lookAt;
		if (!roll)
		{
			lookAt.x = position.x + sinf(rotation.y) * cosf(rotation.x);
			lookAt.y = position.y + sinf(rotation.x);
			lookAt.z = position.z + cosf(rotation.y) * cosf(rotation.x);
		}
		else
		{
			SMatrix4 mtxRot = SMatrix::MakeRotationMatrix(rotation);

			// TODO


		}

		SPMatrixLookAtRH(&viewMatrix, position, lookAt, Vec3f(0,1.0f,0));
		//SPMatrixLookAtRH(&viewMatrix, SVector3(0, 5.0f, -10.0f), SVector3(0,0,0), SVector3(0,1.0f,0));	
		return viewMatrix;
	}
};


SP_NMSPACE_END