// ********************************************************************************************

//	SpeedPoint Camera

// ********************************************************************************************

#pragma once
#include <SPrerequisites.h>
#include "STransformable.h"
#include <math.h>

SP_NMSPACE_BEG




// Summary:
//	SpeedPoint Camera structure
struct S_API SCamera
{
	f32 fViewRadius;		// Keep it short for performant rendering	
	SMatrix4 viewMatrix;
	SVector3 position;
	SVector3 rotation;	// tait-bryan angles

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
	ILINE SVector3& LookAt(const SVector3& lookAt)
	{
		SVector3 dir = SVector3Normalize(lookAt - position);	
		SVector3 dirsq = dir * dir;
		rotation.x = asinf(dir.z / sqrtf(dirsq.z + dirsq.y));
		rotation.y = asinf(dir.z / sqrtf(dirsq.z + dirsq.x));
		rotation.z = asinf(dir.x / sqrtf(dirsq.x + dirsq.y));
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
		SVector3 lookAt;
		if (!roll)
		{
			lookAt.x = -sinf(rotation.y) * cosf(rotation.x);
			lookAt.y = sinf(rotation.y);
			lookAt.z = cosf(rotation.y) * cosf(rotation.x);
		}
		else
		{
			SMatrix4 mtxRot = SMatrix::MakeRotationMatrix(rotation);

			// TODO


		}

		SPMatrixLookAtRH(&viewMatrix, position, lookAt, SVector3(0,1.0f,0));
		return viewMatrix;
	}
};


SP_NMSPACE_END