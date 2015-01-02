// ********************************************************************************************

//	SpeedPoint Bound Box

// ********************************************************************************************

#pragma once
#include <SPrerequisites.h>
#include "Vector3.h"
#include "Matrix.h"
//#include "SPlane.h"


SP_NMSPACE_BEG


// SpeedPoint BoundBox is always axis aligned
struct S_API SAxisAlignedBoundBox
{
	SVector3 vMin, vMax;

	SAxisAlignedBoundBox()
	{
	}

	SAxisAlignedBoundBox(const SAxisAlignedBoundBox& aabb)
		: vMin(aabb.vMin),
		vMax(aabb.vMax)
	{		
	}

	void AddPoint(const SVector3& v)
	{
		vMin.CheckMin(v);
		vMax.CheckMax(v);
	}	

	void Reset()
	{
		vMin = SVector3(0, 0, 0);
		vMax = SVector3(0, 0, 0);
	}
};
typedef struct S_API SAxisAlignedBoundBox AABB;

struct S_API SOrientedBoundBox
{
	SVector3 directions[3];	// rotated x, y, z axis
	SVector3 dimensions;
	SVector3 center;	// center position

	SOrientedBoundBox()		
	{
		directions[0] = SVector3(1.0f, 0, 0);
		directions[1] = SVector3(0, 1.0f, 0);
		directions[2] = SVector3(0, 0, 1.0f);
	}

	SOrientedBoundBox(const SOrientedBoundBox& bb)
	{
		directions[0] = bb.directions[0];
		directions[1] = bb.directions[1];
		directions[2] = bb.directions[2];

		dimensions = bb.dimensions;
		center = bb.center;
	}

	SOrientedBoundBox(const SAxisAlignedBoundBox& aabb)
	{		
		directions[0] = SVector3(1.0f, 0, 0);
		directions[1] = SVector3(0, 1.0f, 0);
		directions[2] = SVector3(0, 0, 1.0f);

		dimensions = aabb.vMax - aabb.vMin;
		center = (aabb.vMin + aabb.vMax) * 0.5f;
	}

	void Transform(const SVector3& position, const SVector3& rotation, const SVector3& size)
	{
		SMatrix rotMat = SMatrix::MakeRotationMatrix(rotation);
		for (unsigned int i = 0; i < 3; ++i)
			directions[i] = (rotMat * SVector4(directions[i], 0.0f)).xyz();

		center += position;
		dimensions += size;
	}
};
typedef struct S_API SOrientedBoundBox OBB;

SP_NMSPACE_END