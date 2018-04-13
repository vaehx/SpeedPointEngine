// ********************************************************************************************

//	SpeedPoint Bound Box

// ********************************************************************************************

#pragma once
#include "Vector3.h"
#include "Mat44.h"
#include "SPrerequisites.h"


SP_NMSPACE_BEG


// SpeedPoint BoundBox is always axis aligned
struct S_API AABB
{
	Vec3f vMin, vMax;

	AABB()
	{
	}

	AABB(const AABB& aabb)
		: vMin(aabb.vMin), vMax(aabb.vMax) {}

	AABB(const Vec3f& min, const Vec3f& max)
		: vMin(min), vMax(max) {}

	void AddPoint(const Vec3f& v)
	{
		vMin.CheckMin(v);
		vMax.CheckMax(v);
	}

	void AddAABB(const AABB& aabb)
	{
		vMin.CheckMin(aabb.vMin);
		vMax.CheckMax(aabb.vMin);
		vMin.CheckMin(aabb.vMax);
		vMax.CheckMax(aabb.vMax);
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

	// Returns true if this aabb truly intersects with the other aabb.
	// In case they are just touching with their sides, this is still considered an intersection.
	bool Intersects(const AABB& aabb) const
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

	inline AABB& Transform(const Mat44& transform)
	{
		Vec3f sz = (transform * Vec4f((vMax - vMin) * 0.5f, 0.0f)).xyz();
		Vec3f pos = (transform * Vec4f((vMax + vMin) * 0.5f, 1.0f)).xyz();
		vMin = pos - sz;
		vMax = pos + sz;

		return *this;
	}

	// min.z plane first, then max.z
	// Per plane: (min.x, min.y), (min.x, max.y), (max.x, max.y), (max.x, min.y)
	inline void GetCorners(Vec3f corners[8])
	{
		if (!corners)
			return;

		corners[0] = Vec3f(vMin.x, vMin.y, vMin.z);
		corners[1] = Vec3f(vMin.x, vMax.y, vMin.z);
		corners[2] = Vec3f(vMax.x, vMax.y, vMin.z);
		corners[3] = Vec3f(vMax.x, vMin.y, vMin.z);
		corners[4] = Vec3f(vMin.x, vMin.y, vMax.z);
		corners[5] = Vec3f(vMin.x, vMax.y, vMax.z);
		corners[6] = Vec3f(vMax.x, vMax.y, vMax.z);
		corners[7] = Vec3f(vMax.x, vMin.y, vMax.z);
	}

	inline void GetCornersTransformed(Vec3f corners[8], const Mat44& mtx)
	{
		if (!corners)
			return;

		GetCorners(corners);
		for (int i = 0; i < 8; ++i)
			corners[i] = (mtx * Vec4f(corners[i], 1.0f)).xyz();
	}
};
typedef struct S_API AABB SAxisAlignedBoundBox;

struct S_API OBB
{
	Vec3f directions[3];	// rotated basis vectors, normalized
	float dimensions[3]; // half-dimensions in each direction
	Vec3f center;	// center position

	OBB()
	{
		directions[0] = Vec3f(1.0f, 0, 0);
		directions[1] = Vec3f(0, 1.0f, 0);
		directions[2] = Vec3f(0, 0, 1.0f);
		dimensions[0] = dimensions[1] = dimensions[2] = FLT_MAX;
	}

	OBB(const OBB& bb)
	{
		directions[0] = bb.directions[0];
		directions[1] = bb.directions[1];
		directions[2] = bb.directions[2];
		dimensions[0] = bb.dimensions[0];
		dimensions[1] = bb.dimensions[1];
		dimensions[2] = bb.dimensions[2];
		center = bb.center;
	}

	OBB(const AABB& aabb)
	{		
		directions[0] = Vec3f(1.0f, 0, 0);
		directions[1] = Vec3f(0, 1.0f, 0);
		directions[2] = Vec3f(0, 0, 1.0f);

		dimensions[0] = (aabb.vMax.x - aabb.vMin.x) * 0.5f;
		dimensions[1] = (aabb.vMax.y - aabb.vMin.y) * 0.5f;
		dimensions[2] = (aabb.vMax.z - aabb.vMin.z) * 0.5f;
		center = (aabb.vMin + aabb.vMax) * 0.5f;
	}

	void Transform(const Vec3f& position, const Vec3f& rotation, const Vec3f& size)
	{
		Mat44 rotMat = Mat44::MakeRotationMatrix(rotation);
		for (unsigned int i = 0; i < 3; ++i)
			directions[i] = (rotMat * Vec4f(directions[i], 0.0f)).xyz();

		center += position;
		dimensions[0] += size.x;
		dimensions[1] += size.y;
		dimensions[2] += size.z;
	}

	void Transform(const Mat44& mtx)
	{
		center = (mtx * Vec4f(center, 1.0f)).xyz();
		for (unsigned int i = 0; i < 3; ++i)
		{
			directions[i] = (mtx * Vec4f(directions[i] * dimensions[i], 0)).xyz();
			directions[i] /= (dimensions[i] = directions[i].Length());
		}
	}
};
typedef struct S_API OBB SOrientedBoundBox;

SP_NMSPACE_END