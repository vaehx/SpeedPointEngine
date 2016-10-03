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
	SMatrix4 viewMatrix; // col0=left, col1=up, col2=forward
	Vec3f position;	

	Quat d_turnQuat;
	Vec3f d_turn;
	Vec3f d_lookAt;

	// Default constructor
	SCamera()
		: fViewRadius(20.0f),
		viewMatrix(1.0f, 0, 0, 0, 0, 1.0f, 0, 0, 0, 0, 1.0f, 0, 0, 0, 0, 1.0f)
	{
	}

	~SCamera()			
	{		
		viewMatrix = SMatrix4(1.0f, 0, 0, 0, 0, 1.0f, 0, 0, 0, 0, 1.0f, 0, 0, 0, 0, 1.0f);
	}


	// Summary:
	//	Uses the up vector (0,1,0) to calculate the rotation quaternion.
	//	The direction from camera position to lookAt must not be the upvector or its negative
	ILINE void LookAt(const Vec3f& lookAt)
	{
		Vec3f dir = Vec3Normalize(lookAt - position);
		
		//float cosAngle = Vec3Dot(dir, Vec3f(0.0f, 0.0f, 1.0f));

		// cosAngle must not be 1.0 (which means that the camera is looking up and
		// the lookat-dir is the same as the up-vector.
		if (/*fabsf(fabsf(cosAngle) - 1.0f) > FLT_EPSILON*/true)
		{
			/*Vec3f axis = Vec3Cross(Vec3f(0, 0, 1.0f), dir).Normalized();
			float angle = acosf(cosAngle);
			Quat rotation = Quat::FromAxisAngle(axis, angle);


			Vec3f d_forward = rotation.GetForward();
			float d_length = rotation.Length();

			Vec3f left = (rotation * Vec3f(1.0f, 0, 0)).Normalized();
			Vec3f up = (rotation * Vec3f(0, 1.0f, 0)).Normalized();
			Vec3f forward = (rotation * Vec3f(0, 0, 1.0f)).Normalized();
			viewMatrix = SMatrix(
				left.x, up.x, forward.x, 0,
				left.y, up.y, forward.y, 0,
				left.z, up.z, forward.z, 0,
				0, 0, 0, 1
				);*/

			SPMatrixLookAtRH(&viewMatrix, position, lookAt, Vec3f(0, 1.0f, 0));


			//RecalculateViewMatrix();
		}		
	}

	// relative
	ILINE void TurnYaw(float rad)
	{
		Quat rotation = Quat::FromRotationY(rad);		
		Vec3f left = rotation * Vec3f(viewMatrix._11, viewMatrix._21, viewMatrix._31);
		Vec3f up = rotation * Vec3f(viewMatrix._12, viewMatrix._22, viewMatrix._32);
		Vec3f forward = rotation * Vec3f(viewMatrix._13, viewMatrix._23, viewMatrix._33);
		viewMatrix = SMatrix(
			left.x, up.x, forward.x, 0,
			left.y, up.y, forward.y, 0,
			left.z, up.z, forward.z, 0,
			0, 0, 0, 1
			);		
		RecalculateViewMatrix();
	}

	// relative
	ILINE void TurnPitch(float rad)
	{
		Quat rotation = Quat::FromRotationX(rad);
		Vec3f left = rotation * Vec3f(viewMatrix._11, viewMatrix._21, viewMatrix._31);
		Vec3f up = rotation * Vec3f(viewMatrix._12, viewMatrix._22, viewMatrix._32);
		Vec3f forward = rotation * Vec3f(viewMatrix._13, viewMatrix._23, viewMatrix._33);
		viewMatrix = SMatrix(
			left.x, up.x, forward.x, 0,
			left.y, up.y, forward.y, 0,
			left.z, up.z, forward.z, 0,
			0, 0, 0, 1
			);
		RecalculateViewMatrix();
	}

	// relative
	void Turn(float yaw, float pitch);	

	Vec3f GetForward() const;
	Vec3f GetLeft() const;
	Vec3f GetUp() const;


	// Summary:
	//	Recalculate the view matrix
	//	should only be called by the camera itself, but is public to be more flexible, if you want to do it anways...
	// Arguments:
	//	- roll: Set to true if you also want to take roll into account (rotation.z)
	// Return Value:
	//	Returns new view matrix as const ref
	SMatrix4& RecalculateViewMatrix(bool roll = false);
};


SP_NMSPACE_END