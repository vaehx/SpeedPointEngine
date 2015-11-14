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
		: vMin(aabb.vMin), vMax(aabb.vMax) {}

	SAxisAlignedBoundBox(const Vec3f& min, const Vec3f& max)
		: vMin(min), vMax(max) {}

	void AddPoint(const SVector3& v)
	{
		vMin.CheckMin(v);
		vMax.CheckMax(v);
	}	

	void Reset()
	{
		vMin = SVector3(FLT_MAX);
		vMax = SVector3(-FLT_MAX);
	}

	// length is set to the length of the vector from ray origin to the intersection
	bool HitsRay(const Vec3f& v, const Vec3f& p, float* pLength = 0) const
	{
		Vec3f invDir;

		invDir.x = 1.0f / v.x;
		invDir.y = 1.0f / v.y;
		invDir.z = 1.0f / v.z;

		float t1 = (vMin.x - p.x)*invDir.x;
		float t2 = (vMax.x - p.x)*invDir.x;
		float t3 = (vMin.y - p.y)*invDir.y;
		float t4 = (vMax.y - p.y)*invDir.y;
		float t5 = (vMin.z - p.z)*invDir.z;
		float t6 = (vMax.z - p.z)*invDir.z;

		float tmin = max(max(min(t1, t2), min(t3, t4)), min(t5, t6));
		float tmax = min(min(max(t1, t2), max(t3, t4)), max(t5, t6));

		// if tmax < 0, ray (line) is intersecting AABB, but whole AABB is behing us
		if (tmax < 0)
		{
			if (IS_VALID_PTR(pLength))
				*pLength = tmax;

			return false;
		}

		// if tmin > tmax, ray doesn't intersect AABB
		if (tmin > tmax)
		{
			if (IS_VALID_PTR(pLength))
				*pLength = tmax;
			
			return false;
		}

		if (IS_VALID_PTR(pLength))
			*pLength = tmin;

		return true;
	}

	bool Intersects(const SAxisAlignedBoundBox& aabb) const
	{
		if ((vMin.x > aabb.vMax.x) || (aabb.vMin.x > vMax.x)) return false;		
		if ((vMin.y > aabb.vMax.y) || (aabb.vMin.y > vMax.y)) return false;		
		if ((vMin.z > aabb.vMax.z) || (aabb.vMin.z > vMax.z)) return false;		
		
		return true;
	}

	inline void MoveRelative(const Vec3f& v)
	{
		vMin += v;
		vMax += v;
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