#pragma once

#include <SPrerequisites.h>
#include <Abstract\Vector3.h>

using namespace SpeedPoint;

std::ostream& operator<<(std::ostream& os, const Vec3f& v)
{
	os << "(" << v.x << ", " << v.y << ", " << v.z << ")";
	return os;
}

// hd = OBB half dim, c = OBB Center, p = ray point, v = ray direction

/*

	x: Written, but not implemented yet
	Y: Written and tested successfully

	| Point | Ray  | Plane	| Sphere | Cylinder | Capsule | Box | Mesh
==============================================================================
Point	|!!!!!!!|  Y   |   Y	|   Y	 |          |	 Y    |	 Y  |
------------------------------------------------------------------------------
Ray	|#######|  Y   |   Y	|	 |     Y    |	 Y    |  Y  |
------------------------------------------------------------------------------
Plane	|#######|######|	|	 |	    |	      |	    |
------------------------------------------------------------------------------
Sphere	|#######|######|########|   Y    |	    |	      |	    |
------------------------------------------------------------------------------
Cylinder|#######|######|########|########|	    |	      |	    |
------------------------------------------------------------------------------
Capsule |#######|######|########|########|##########|	      |	    |
------------------------------------------------------------------------------
Box	|#######|######|########|########|##########|#########|	    |
------------------------------------------------------------------------------
Mesh	|#######|######|########|########|##########|#########|#####|


*/

#define FLOAT_TOLERANCE FLT_EPSILON

///////////////////////////////////////////////////////////////////////////////////////////////

typedef SpeedPoint::Vec3f SPoint;

///////////////////////////////////////////////////////////////////////////////////////////////

struct SRay
{
	Vec3f p; // point
	Vec3f v; // direction

	inline Vec3f GetPoint(float t) const
	{
		return p + t * v;
	}
};

///////////////////////////////////////////////////////////////////////////////////////////////

struct SPlane
{
	Vec3f n; // normal
	float d; // dot(n, x) = d

	// normal = cross(b - a, c - a)
	static inline SPlane FromPoints(const Vec3f& a, const Vec3f& b, const Vec3f& c)
	{
		SPlane plane;
		plane.n = Vec3Cross(b - a, c - a).Normalized();
		plane.d = Vec3Dot(plane.n, a);
		return plane;
	}

	static inline SPlane FromNormalAndPoint(const Vec3f& normal, const Vec3f& point)
	{
		SPlane plane;
		plane.n = normal;
		plane.d = Vec3Dot(normal, point);
		return plane;
	}
};

///////////////////////////////////////////////////////////////////////////////////////////////

struct SSphere
{
	Vec3f center; // center
	float radius; // radius

	SSphere() : radius(1.0f) {}
	SSphere(const Vec3f& c, float r) : center(c), radius(r) {}
};

///////////////////////////////////////////////////////////////////////////////////////////////

struct SCylinder
{
	Vec3f p1, p2; // Start and End-Points on the ray
	float r; // radius
};

///////////////////////////////////////////////////////////////////////////////////////////////

struct SCapsule
{
	Vec3f p1, p2; // Start and End-Points on the ray, centers of the cap-spheres
	float r;
};

///////////////////////////////////////////////////////////////////////////////////////////////

struct SBox
{
	Vec3f c; // Center
	Vec3f hd[3]; // Half-Dimension vectors, also specifying the orientation of the box
	SPlane planes[6]; // is ordered: 0-1: x-Planes; 2-3: y-Planes; 4-5: zPlanes

	// points 0-4 will be (in clockwise order) the vertices of the local +x side, starting at local (+x,+y,+z)
	inline void ComputeVertices(SPoint points[8]) const
	{
		points[0] = c + hd[0] + hd[1] + hd[2];
		points[1] = c + hd[0] + hd[1] - hd[2];
		points[2] = c + hd[0] - hd[1] - hd[2];
		points[3] = c + hd[0] - hd[1] + hd[2];

		const Vec3f doubleXDim = 2.0f * hd[0];
		for (unsigned char i = 4; i < 8; ++i)
			points[i] = points[i - 4] - doubleXDim;
	}

	inline void CalculatePlanePairs()
	{
		SPoint points[8];
		ComputeVertices(points);

		// x-Planes
		planes[0] = SPlane::FromPoints(points[0], points[3], points[1]);
		planes[1] = SPlane::FromPoints(points[7], points[4], points[6]);

		// y-Planes
		planes[2] = SPlane::FromPoints(points[0], points[1], points[4]);
		planes[3] = SPlane::FromPoints(points[6], points[2], points[7]);

		// z-Planes
		planes[4] = SPlane::FromPoints(points[7], points[3], points[4]);
		planes[5] = SPlane::FromPoints(points[1], points[2], points[5]);
	}
};

///////////////////////////////////////////////////////////////////////////////////////////////

struct SMeshVertex
{
	float x, y, z;
	float nx, ny, nz;
};

struct SMeshEdge
{
	unsigned long vtx1, vtx2;
};

struct SMesh
{
	std::vector<SMeshVertex> vertices;
	std::vector<SMeshEdge> edges;
};



inline bool IntersectPointOBB(const SBox& obb, const SPoint& point);
inline bool IntersectRayOBB(const SBox& obb, const SRay& ray, float* pMin = 0, float *pMax = 0, Vec3f *pMinNormal = 0, Vec3f *pMaxNormal = 0);
inline bool IntersectRayRay(const SRay& ray1, const SRay& ray2, float* pParam1 = 0, float* pParam2 = 0, float* pDist = 0);

// incidentNormal - Set to true to get the actual incident normal, instead of the normal of the plane. The incident normal
//		calculated by checking on which side the plane start point lies
inline bool IntersectRayPlane(const SRay& ray, const SPlane& plane, float *param = 0, Vec3f* pNormal = 0, bool incidentNormal = false);

// pOutside - Set to true, if the point lies on the side in which the normal points to.
inline bool IntersectPlanePoint(const SPlane& plane, const SPoint& point, bool *pOutside = 0);

inline bool IntersectRayPoint(const SRay& ray, const SPoint& point, float* pDist = 0);
inline bool IntersectRaySphere(const SRay& ray, const SSphere& sphere, float* pDist = 0);
inline bool IntersectRayCylinder(const SRay& ray, const SCylinder& cyl);
inline bool IntersectRayCapsule(const SRay& ray, const SCapsule& capsule);

// pDist is set the to minimum distance of the spheres from surface to surface
// If pDist is negative, this indicates, that the spheres inter-penetrate.
inline bool IntersectSphereSphere(const SSphere& sphere1, const SSphere& sphere2, float* pDist = 0);

// pDist is set to the minimum distance of the point to the surface of the sphere
// If pDist is negativ, this indicates, that the point inter-penetrates the sphere.
inline bool IntersectSpherePoint(const SSphere& sphere, const SPoint& point, float *pDist = 0);






// -------------------------------------------------------------------------------------------
//
//	Ray - OBB
//
// -------------------------------------------------------------------------------------------
inline bool IntersectRayOBB(const SBox& obb, const SRay& ray, float* pMin/*= 0*/, float *pMax/*= 0*/, Vec3f *pMinNormal /*=0*/, Vec3f *pMaxNormal /*=0*/)
{
	float t_min = -FLT_MAX, t_max = FLT_MAX;
	Vec3f minNormal, maxNormal;
	float t1, t2;
	Vec3f n1, n2;
	unsigned char num_intersections = 0;
	unsigned char last_intersection_i = 0;
	for (unsigned char i = 0; i < 6; i += 2)
	{		
		if (!IntersectRayPlane(ray, obb.planes[i], &t1, &n1))			
			continue;	

		if (!IntersectRayPlane(ray, obb.planes[i + 1], &t2, &n2))			
			continue;

		++num_intersections;
		last_intersection_i = i;

		// Swap t1 and t2 if necessary
		if (t1 > t2)
		{
			float temp = t1;
			t1 = t2;
			t2 = temp;

			Vec3f ntemp = n1;
			n1 = n2;
			n2 = ntemp;
		}

		// Determine new min/max
		if (t1 > t_min)
		{
			t_min = t1;
			minNormal = n1;
		}
		if (t2 < t_max)
		{
			t_max = t2;
			maxNormal = n2;
		}

		if (t_max < t_min)
			return false;	
	}

	if (num_intersections == 0)
		return false;

	if (num_intersections == 1)
	{
		const Vec3f minPoint = ray.GetPoint(t_min);
		const Vec3f maxPoint = ray.GetPoint(t_max);

		// We have to few information here.
		// We'll take the found min/max intersection points and check that they are all at the inside of each other plane
		bool outside;
		for (unsigned char i = 0; i < 6; ++i)
		{
			if (i == last_intersection_i || i == (last_intersection_i + 1))
				continue;

			IntersectPlanePoint(obb.planes[i], minPoint, &outside);
			if (outside)
				return false;			
		}
	}


	if (IS_VALID_PTR(pMin)) *pMin = t_min;
	if (IS_VALID_PTR(pMax)) *pMax = t_max;
	if (IS_VALID_PTR(pMinNormal)) *pMinNormal = minNormal;
	if (IS_VALID_PTR(pMaxNormal)) *pMaxNormal = maxNormal;

	return true;
}

// -------------------------------------------------------------------------------------------
//
//	Point - OBB
//
// -------------------------------------------------------------------------------------------
inline bool IntersectPointOBB(const SBox& obb, const SPoint& point)
{
	// Go through all planes and check if the point is on the inside
	bool outside;
	for (unsigned char i = 0; i < 6; ++i)
	{
		if (IntersectPlanePoint(obb.planes[i], point, &outside))
			return true;

		if (outside)
			return false;
	}

	return true;
}


// -------------------------------------------------------------------------------------------
//
//	Ray - Ray
//
// -------------------------------------------------------------------------------------------
inline bool IntersectRayRay(const SRay& ray1, const SRay& ray2, float* pParam1/* = 0*/, float* pParam2/* = 0*/, float* pDist/*=0*/)
{
	// p + t*v = q + s*u
	const Vec3f &p = ray1.p, &v = ray1.v, &q = ray2.p, &u = ray2.v;

	// Both start points are near enough together?
	if ((p - q).LengthSq() <= (FLOAT_TOLERANCE * FLOAT_TOLERANCE))
	{
		if (IS_VALID_PTR(pParam1)) *pParam1 = 0.0f;
		if (IS_VALID_PTR(pParam2)) *pParam2 = 0.0f;
		if (IS_VALID_PTR(pDist)) *pDist = 0.0f;
		return true;
	}
		
	// dist = dot(p - q, normalized(cross(v, u)))
	// This is determined using the function for the distance between two lines and dot-multiplying both
	// equation sides with distDir, which is perpendicular to v and u.
	const Vec3f distDir = Vec3Cross(v, u).Normalized();
	const float dist = fabsf(Vec3Dot(q - p, distDir));

	Vec3f vxu = Vec3Cross(v, u);
	float den = vxu.LengthSq();
	if (den <= FLOAT_TOLERANCE)
	{
		if (IS_VALID_PTR(pParam1)) *pParam1 = 0.0f;
		if (IS_VALID_PTR(pParam2)) *pParam2 = 0.0f;
		if (IS_VALID_PTR(pDist))
		{
			// Determine distance by projecting p-q-vector onto ray1 and then subtracting this from the p-q-vector
			const Vec3f vNormalized = v.Normalized();
			const Vec3f pq = q - p;
			*pDist = (pq - Vec3Dot(vNormalized, pq) * vNormalized).Length();		
		}

		return false; // lines are coincident or parallel
	}

	// When lines do not intersect, t and s are the positions of the nearest distance
	float t = Vec3Dot(Vec3Cross(q - p, u), vxu) / den;
	float s = Vec3Dot(Vec3Cross(q - p, v), vxu) / den;
	if (IS_VALID_PTR(pParam1)) *pParam1 = t;
	if (IS_VALID_PTR(pParam2)) *pParam2 = s;
	
	bool intersect = (dist <= FLOAT_TOLERANCE);
	if (IS_VALID_PTR(pDist)) *pDist = (intersect ? 0.0f : dist);

	return intersect;
}


// -------------------------------------------------------------------------------------------
//
//	Ray - Plane
//
// -------------------------------------------------------------------------------------------
inline bool IntersectRayPlane(const SRay& ray, const SPlane& plane, float *param/* = 0*/, Vec3f* pNormal/* = 0*/, bool incidentNormal/*=false*/)
{
	// Ray: x = p + tv
	// Plane: dot(x, n) = d
	const Vec3f &p = ray.p, &v = ray.v, &n = plane.n;
	const float &d = plane.d;

	// dot(p + tv, n) = d  --> t = (d - dot(p, n)) / dot(v, n)
	float den = Vec3Dot(v, n);

	// Parallel?  dot(v,n) == 0
	if (fabs(den) <= FLOAT_TOLERANCE)
	{		
		if (IntersectPlanePoint(plane, p))
		{
			if (IS_VALID_PTR(param)) *param = 0.0f;

			// Simply use planes normal here
			if (IS_VALID_PTR(pNormal)) *pNormal = n;

			return true;
		}
		else
		{
			return false;
		}
	}

	if (IS_VALID_PTR(param))
	{
		*param = (d - Vec3Dot(p, n)) / den;
	}

	if (IS_VALID_PTR(pNormal))
	{
		if (incidentNormal)
		{
			// If the start point of the line lies on the opossite side of the plane,
			// than its normal points to, we simply flip the normal.
			if ((Vec3Dot(p, n) - d) < 0)
				*pNormal = -n;
			else
				*pNormal = n;
		}
		else
		{
			*pNormal = n;
		}
	}

	return true;
}

// -------------------------------------------------------------------------------------------
//
//	Plane - Point
//
//	pOutside - Set to true, if the point lies on the side in which the normal points to.
//
// -------------------------------------------------------------------------------------------
inline bool IntersectPlanePoint(const SPlane& plane, const SPoint& point, bool *pOutside /*= 0*/)
{
	// Plane: dot(x, n) = d
	const Vec3f &n = plane.n;
	const float &d = plane.d;

	// Calculate distance here and check against 0

	// D = (dot(n, x) - d) / n.Length()
	float den = n.Length();
	if (fabs(den) <= FLOAT_TOLERANCE)
		return false;
	
	float D = Vec3Dot(n, point) - d / den;
	if (fabs(D) > FLOAT_TOLERANCE)
	{
		if (IS_VALID_PTR(pOutside))
			*pOutside = (D > 0.0f);

		return false;
	}
	else
	{
		return true;
	}
}

// -------------------------------------------------------------------------------------------
//
//	Ray - Point
//
// -------------------------------------------------------------------------------------------
inline bool IntersectRayPoint(const SRay& ray, const SPoint& point, float* pDist/* = 0*/)
{
	// Ray: x = p + s * v
	const Vec3f &p = ray.p, &v = ray.v, &x = point;

	// 1. Project (x - p) onto v, normalize with length of v
	Vec3f projected = v * Vec3Dot(v, x - p) / v.Length();

	// 2. Calculate distance
	float dist = Vec3Length(x - (p + projected));

	if (IS_VALID_PTR(pDist))
		*pDist = dist;

	return (dist <= FLOAT_TOLERANCE);
}

// -------------------------------------------------------------------------------------------
//
//	Ray - Sphere
//
// -------------------------------------------------------------------------------------------
inline bool IntersectRaySphere(const SRay& ray, const SSphere& sphere, float* pDist/* = 0*/)
{
	float dist;
	if (IntersectRayPoint(ray, sphere.center, &dist))
	{
		if (IS_VALID_PTR(pDist)) *pDist = -sphere.radius;
		return true;
	}

	if (IS_VALID_PTR(pDist)) *pDist = dist - sphere.radius;

	return (dist <= sphere.radius);
}

// -------------------------------------------------------------------------------------------
//
//	Ray - Cylinder
//
// -------------------------------------------------------------------------------------------
inline bool IntersectRayCylinder(const SRay& ray, const SCylinder& cyl)
{
	// Ray: x = p + t * v
	// Cylinder: p1, p2, radius, u = direction (normalized)
	const Vec3f &p = ray.p, &v = ray.v;
	const Vec3f &p1 = cyl.p1, &p2 = cyl.p2;
	const float &r = cyl.r;
	Vec3f u = p2 - p1;
	const float ulength = u.Length();
	u /= ulength; // normalize u

	// Parallel?
	if (fabs(fabs(Vec3Dot(v.Normalized(), u)) - 1.0f) <= FLOAT_TOLERANCE)
	{		
		// Determine distance by projecting p-p1-vector onto ray and then subtracting this from the p-p1-vector
		const Vec3f vNormalized = v.Normalized();
		const Vec3f pq = p1 - p;
		float distSq = (pq - Vec3Dot(vNormalized, pq) * vNormalized).LengthSq();
		return (distSq <= (r * r));
	}

	// Determine perpendicular foot on cyl of the shortest distance-vector
	float param, dist;
	SRay cylRay;
	cylRay.p = p1;
	cylRay.v = u;	
	bool intersect = IntersectRayRay(ray, cylRay, 0, &param, &dist);

	// Check if perpendicular foot is in the cyl-ray-segment
	if (param >= 0 && param <= ulength)
		return (dist <= r + FLOAT_TOLERANCE); // intersecting the cylinder
	
	// Check if the ray intersects with the cap-planes. IntersectRayPlane() must not modify the param, if there was no intersection
	float bottomParam = FLT_MAX, topParam = FLT_MAX;	
	bool bottomIntersect = IntersectRayPlane(ray, SPlane::FromNormalAndPoint(-u, p1), &bottomParam);
	bool topIntersect = IntersectRayPlane(ray, SPlane::FromNormalAndPoint(u, p2), &topParam);

	if (!bottomIntersect && !topIntersect)
		return false;

	Vec3f checkPnt = p1;
	param = bottomParam;
	if (topParam < bottomParam)
	{
		param = topParam;
		checkPnt = p2;
	}

	// possibleIntersection and distSq should lie on the same plane (bottom-cap-plane)
	const Vec3f possibleIntersection = ray.GetPoint(param);
	const float distSq = (possibleIntersection - checkPnt).LengthSq();

	return (distSq <= (r * r));
}



// -------------------------------------------------------------------------------------------
//
//	Ray - Capsule
//
// -------------------------------------------------------------------------------------------
inline bool IntersectRayCapsule(const SRay& ray, const SCapsule& capsule)
{
	// Ray: x = p + t * v
	// Cylinder: p1, p2, radius, u = direction (normalized)
	const Vec3f &p = ray.p, &v = ray.v;
	const Vec3f &p1 = capsule.p1, &p2 = capsule.p2;
	const float &r = capsule.r;
	Vec3f u = p2 - p1;
	const float ulength = u.Length();
	u /= ulength; // normalize u

		      // Parallel?
	if (fabs(fabs(Vec3Dot(v.Normalized(), u)) - 1.0f) <= FLOAT_TOLERANCE)
	{
		// Determine distance by projecting p-p1-vector onto ray and then subtracting this from the p-p1-vector
		const Vec3f vNormalized = v.Normalized();
		const Vec3f pq = p1 - p;
		float distSq = (pq - Vec3Dot(vNormalized, pq) * vNormalized).LengthSq();
		return (distSq <= (r * r));
	}

	// Determine perpendicular foot on cyl of the shortest distance-vector
	float cylParam, rayParam, dist;
	SRay cylRay;
	cylRay.p = p1;
	cylRay.v = u;
	IntersectRayRay(ray, cylRay, &rayParam, &cylParam, &dist);

	// Check if perpendicular foot is in the cyl-ray-segment
	if (cylParam >= 0 && cylParam <= ulength)
		return (dist <= r + FLOAT_TOLERANCE); // intersecting the cylinder

	// Check if the ray intersects with the cap-sphere, that is nearest to the point on the ray that is nearest to the cyl-ray	
	Vec3f checkSphereCenter = p1;
	Vec3f rayPoint = ray.GetPoint(rayParam);
	if ((p2 - rayPoint).LengthSq() < (p1 - rayPoint).LengthSq())
		checkSphereCenter = p2;
	
	bool intersect = IntersectRaySphere(ray, SSphere(checkSphereCenter, r));
	return intersect;
}



// -------------------------------------------------------------------------------------------
//
//	Point - Sphere
//
//-------------------------------------------------------------------------------------------
inline bool IntersectSpherePoint(const SSphere& sphere, const SPoint& point, float *pDist /*= 0*/)
{
	// If the actual distance is not required, do not use a square-root
	if (!IS_VALID_PTR(pDist))
	{
		return ((point - sphere.center).LengthSq() <= sphere.radius * sphere.radius);
	}
	else
	{
		*pDist = (point - sphere.center).Length() - sphere.radius;
		return (*pDist <= FLOAT_TOLERANCE);
	}
}

// -------------------------------------------------------------------------------------------
//
//	Sphere - Sphere
// 
// 	pDist might be negative, meaning that the spheres inter-penetrate
//
// -------------------------------------------------------------------------------------------
inline bool IntersectSphereSphere(const SSphere& sphere1, const SSphere& sphere2, float* pDist/* = 0*/)
{
	if (IS_VALID_PTR(pDist))
	{
		*pDist = Vec3Length(sphere2.center - sphere1.center) - sphere1.radius - sphere2.radius;

		// dist can be negative
		return (*pDist <= FLOAT_TOLERANCE);
	}
	else
	{
		// Do not use a square root when the actual distance is not important.

		float radSum = sphere1.radius + sphere2.radius;
		if ((sphere2.center - sphere1.center).LengthSq() <= radSum * radSum)
			return true;
		else
			return false;
	}
}


// -------------------------------------------------------------------------------------------
//
//	Point - Capsule
//
// -------------------------------------------------------------------------------------------
inline bool IntersectCapsulePoint(const SCapsule& capsule, const SPoint& point, float* pDist/*=0*/)
{
	// Capsule: p1, p2, r
	const Vec3f &p1 = capsule.p1, &p2 = capsule.p2;
	const Vec3f v = p2 - p1;
	const float vlength = v.Length();
	const float &r = capsule.r;

	// Calculate position of the perpendicular foot of that point on the line by projecting (point - p1) onto v	
	const Vec3f vnorm = v / vlength;
	const float projectedLength = Vec3Dot(point - p1, vnorm);
	const Vec3f perpFoot = p1 + projectedLength * vnorm;

	// If the distance is greater than the radius, the point definetly lies not inside the capsule
	const float distSq = (perpFoot - point).LengthSq();	
	if (distSq > (r * r))
	{
		if (IS_VALID_PTR(pDist))
		{
			float cylDist = sqrtf(distSq) - r;
			float dist1 = FLT_MAX, dist2 = FLT_MAX;
			SSphere testSphere;
			testSphere.radius = r;
			testSphere.center = p1;
			IntersectSpherePoint(testSphere, point, &dist1);

			testSphere.center = p2;
			IntersectSpherePoint(testSphere, point, &dist2);

			*pDist = min(cylDist, min(dist1, dist2));
		}		
		return false;
	}

	// Check if the perpFoot is in the line segment of the inner cylinder
	if (projectedLength >= 0.0f && projectedLength <= vlength)
	{
		if (IS_VALID_PTR(pDist)) *pDist = sqrtf(distSq) - r;		
		return true;
	}

	// If the projected is out of (-radius;length+radius) range, there is definetly not a collision.	
	bool inPossibleRange = (projectedLength >= -r && projectedLength <= (vlength + r));	
	if (inPossibleRange || IS_VALID_PTR(pDist))
	{
		// Test with both cap-spheres to determine minimum distance to the capsule
		float dist1 = FLT_MAX, dist2 = FLT_MAX;
		bool test1, test2;
		SSphere testSphere;
		testSphere.radius = r;
		testSphere.center = p1;
		test1 = IntersectSpherePoint(testSphere, point, &dist1);
		
		testSphere.center = p2;
		test2 = IntersectSpherePoint(testSphere, point, &dist2);

		// minimal distance is ALWAYS the minimum of dist1 and dist2
		if (IS_VALID_PTR(pDist)) *pDist = min(dist1, dist2);

		if (inPossibleRange)
		{			
			return (test1 || test2);
		}
	}
	
	// pDist is already set above
	return false;
}