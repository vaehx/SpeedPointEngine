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

	// length is set to the length of the vector from ray origin to the (first) intersection
	// IntersectionPoint = p + v * (*pLength)
	bool HitsRay(const Vec3f& v, const Vec3f& p, float* pLength = 0) const
	{
		Vec3f invDir;

		invDir.x = 1.0f / v.x;
		invDir.y = 1.0f / v.y;
		invDir.z = 1.0f / v.z;

		float t1 = (vMin.x - p.x) * invDir.x;
		float t2 = (vMax.x - p.x) * invDir.x;
		float t3 = (vMin.y - p.y) * invDir.y;
		float t4 = (vMax.y - p.y) * invDir.y;
		float t5 = (vMin.z - p.z) * invDir.z;
		float t6 = (vMax.z - p.z) * invDir.z;

		float tmin, tmax;
		tmin = min(min(t1, t2), INFINITY);
		tmax = max(max(t1, t2), -INFINITY);

		tmin = max(tmin, min(min(t3, t4), tmax));
		tmax = min(tmax, max(max(t3, t4), tmin));

		tmin = max(tmin, min(min(t5, t6), tmax));
		tmax = min(tmax, max(max(t5, t6), tmin));

		// if tmax < 0, ray (line) is intersecting AABB, but whole AABB is behind us
		if (!(tmax > max(tmin, 0.0f)))
		{
			return false;
		}

		if (IS_VALID_PTR(pLength))
			*pLength = tmin;

		return true;
	}

	// Returns true if the line segment intersects the AABB
	bool HitsLineSegment(const Vec3f& p1, const Vec3f& p2, float* pLambda = 0) const
	{
		float param;
		bool intersect = HitsRay((p2 - p1), p1, &param);
		
		if (IS_VALID_PTR(pLambda))
			*pLambda = param;
		
		if (!intersect)
			return false;

		// Intersection is further away from p1 than p2 is
		if (param > 1.0f)
			return false;

		return true;
	}

	bool Intersects(const SAxisAlignedBoundBox& aabb) const
	{
		if ((vMin.x > aabb.vMax.x) || (aabb.vMin.x > vMax.x)) return false;		
		if ((vMin.y > aabb.vMax.y) || (aabb.vMin.y > vMax.y)) return false;		
		if ((vMin.z > aabb.vMax.z) || (aabb.vMin.z > vMax.z)) return false;		
		
		return true;
	}

	inline bool ContainsPoint(const Vec3f& p) const
	{
		return (p.x >= vMin.x && p.y >= vMin.y && p.z >= vMin.z)
			&& (p.x <= vMax.x && p.y <= vMax.y && p.z <= vMax.z);
	}

	// Outsets the AABB by k in each direction
	inline void Outset(float k)
	{
		vMin -= k;
		vMax += k;
	}

	inline void MoveRelative(const Vec3f& v)
	{
		vMin += v;
		vMax += v;
	}

	inline SAxisAlignedBoundBox& Transform(const SMatrix& transform)
	{
		//TODO: Maybe avoid 7 flops by using a 3x3 matrix here!
		Vec3f sz = (transform * Vec4f((vMax - vMin) * 0.5f, 0.0f)).xyz();
		Vec3f pos = (transform * Vec4f((vMax + vMin) * 0.5f, 1.0f)).xyz();
		vMin = pos - sz;
		vMax = pos + sz;

		return *this;
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