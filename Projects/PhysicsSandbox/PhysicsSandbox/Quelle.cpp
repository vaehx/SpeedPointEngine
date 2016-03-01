#include <iostream>
#include <Abstract\MathGeom.h>
#include <Abstract\Quaternion.h>
//#include "Primitives.h"
//#include "Quaternion.h"

using namespace SpeedPoint;
using namespace std;

std::ostream& operator <<(std::ostream& os, const Vec3f& v)
{
	os << "(" << v.x << ", " << v.y << ", " << v.z << ")";
	return os;
}

std::ostream& operator <<(std::ostream& os, const Quat& q)
{
	os << "[w=" << q.w << " v=" << q.v << "]";
	return os;
}

#define SAN_FLT(x) ((fabsf(x) <= FLT_EPSILON) ? 0.0f : x)

std::ostream& operator <<(std::ostream& os, const SMatrix& m)
{
	for (unsigned i = 0; i < 4; ++i)
	{
		os << " ( " << SAN_FLT(m.m[i][0]) << " " << SAN_FLT(m.m[i][1]) << " " << SAN_FLT(m.m[i][2]) << " " << SAN_FLT(m.m[i][3]) << " )" << std::endl;
	}

	return os;
}

void test_intersections1()
{
	{
		SRay ray;
		SPlane plane;

		ray.p = Vec3f(2.0f, 0, 0);
		ray.v = Vec3f(1.0f, 0, 0);

		plane.n = Vec3f(1.0f, 0, 0);
		plane.d = 0;

		float param;
		Vec3f normal;
		bool intersect = IntersectRayPlane(ray, plane, &param, &normal, true);
		std::cout << "Intersect Ray-Plane: " << intersect << std::endl;
		if (intersect)
		{
			std::cout << "  At: " << param << " " << ray.GetPoint(param) << " normal=" << normal << std::endl;
		}
		std::cout << std::endl;
	}


	{
		SBox box;
		SRay ray;
		SPoint point;
		box.c = Vec3f(0, 0, 0);
		box.hd[0] = Vec3f(1.0f, 0, 0);
		box.hd[1] = Vec3f(0, 1.0f, 0);
		box.hd[2] = Vec3f(0, 0, 1.0f);
		box.CalculatePlanePairs();


		
		ray.p = Vec3f(0, -1.2f, 0);
		ray.v = Vec3Normalize(Vec3f(1.2f, 0, 0) - ray.p);
		
		
		ray.p = Vec3f(5.0f, 0, 0);
		ray.v = Vec3f(0, 0, 1.0f).Normalized();		

		float min, max;
		Vec3f minNormal, maxNormal;
		bool intersect = IntersectRayOBB(box, ray, &min, &max, &minNormal, &maxNormal);	

		std::cout << "Intersect ray-box: " << intersect << std::endl;
		if (intersect)
		{
			std::cout << "min: " << min << " " << ray.GetPoint(min) << " Normal: " << minNormal << std::endl;
			std::cout << "max: " << max << " " << ray.GetPoint(max) << " Normal: " << maxNormal << std::endl;
		}

		point = SPoint(2.0f, 0, 0.1f);

		intersect = IntersectPointOBB(box, point);
		std::cout << "Intersect point-box: " << intersect << std::endl;

		std::cout << std::endl << "Box planes:" << std::endl;
		for (unsigned char i = 0; i < 6; ++i)
		{
			std::cout << "[" << (int)i << "] " << "n=" << box.planes[i].n << " d=" << box.planes[i].d << std::endl;
		}
	}

	{
		SSphere sphere;
		SPoint point;
		sphere.center = Vec3f(0, 0, 0);
		sphere.radius = 3.0f;
		point = Vec3f(0, 0, 0);

		float dist;
		bool intersect = IntersectSpherePoint(sphere, point, &dist);

		std::cout << "(With dist): Intersect Sphere-Point: " << intersect << std::endl;
		std::cout << "  dist=" << dist << std::endl;
		std::cout << std::endl;

		intersect = IntersectSpherePoint(sphere, point);

		std::cout << "(Without dist): Intersect Sphere-Point: " << intersect << std::endl;
		std::cout << std::endl;
	}

	{
		SRay ray;
		SPoint point;

		ray.p = Vec3f(0, 0, 0);
		ray.v = Vec3f(1.0f, 0, 0);

		point = SPoint(3.0f, 5.0f, 0);

		float dist;
		bool intersect = IntersectRayPoint(ray, point, &dist);

		std::cout << "Intersect Ray-Point: " << intersect << std::endl;
		std::cout << "  dist=" << dist << std::endl;	

		std::cout << std::endl;
	}

	{
		SRay ray1;
		SRay ray2;

		ray2.p = Vec3f(0, 0, 0);
		ray2.v = Vec3f(0, 0, 1.0f);

		ray1.p = Vec3f(5.0f, 0, 0);
		ray1.v = (Vec3f(0, 0, 1.0f) - ray1.p).Normalized();

		float param1, param2, dist;
		bool intersect = IntersectRayRay(ray1, ray2, &param1, &param2, &dist);
		std::cout << "Intersect ray-ray: " << intersect << std::endl;
		std::cout << "  dist = " << dist << std::endl;
		std::cout << "  t1=" << param1 << " " << ray1.GetPoint(param1) << std::endl;
		std::cout << "  t2=" << param2 << " " << ray2.GetPoint(param2) << std::endl;	

		std::cout << std::endl;
	}

	{
		SCapsule capsule;
		SPoint point;

		capsule.p1 = Vec3f(1.0f, 0, 0);
		capsule.p2 = Vec3f(9.0f, 0, 0);
		capsule.r = 1.0f;

		point = SPoint(1000.0f, 0.0f, 0);

		float dist;
		bool intersect = IntersectCapsulePoint(capsule, point, &dist);
		std::cout << "Intersect Capsule-Point: " << intersect << std::endl;
		std::cout << "  dist = " << dist << std::endl;

		std::cout << std::endl;
	}

	{
		SCylinder cylinder;
		SRay ray;

		cylinder.p1 = Vec3f(0.0f, 0, 0);
		cylinder.p2 = Vec3f(10.0, 0, 0);
		cylinder.r = 1.0f;

		ray.p = Vec3f(0, 2.0f, 0);
		ray.v = Vec3f(0, -1.0f, 0);

		bool intersect = IntersectRayCylinder(ray, cylinder);
		std::cout << "  Intersect ray-cylinder: " << intersect << std::endl;

		std::cout << std::endl;
	}

	{
		SCapsule capsule;
		SRay ray;

		capsule.p1 = Vec3f(1.0f, 0, 0);
		capsule.p2 = Vec3f(9.0, 0, 0);
		capsule.r = 1.0f;

		ray.p = Vec3f(-15.0f, 0.5f, 0);
		ray.v = Vec3f(-1.0f, 0, 0);

		bool intersect = IntersectRayCapsule(ray, capsule);
		std::cout << "  Intersect ray-capsule: " << intersect << std::endl;

		std::cout << std::endl;
	}
}

void test_intersections_triangle()
{
	{
		SMeshVertex vtx1, vtx2, vtx3;
		vtx1.x = 0.0f; vtx1.y = 0.0f; vtx1.z = 0.0f;
		vtx2.x = 2.0f; vtx2.y = 0.0f; vtx2.z = 0.0f;
		vtx3.x = 1.0f; vtx3.y = 2.0f; vtx3.z = 0.0f;

		cout << "Triangle: " << endl;
		cout << "    vtx1: " << MESHVERTEX_TO_VEC3F(vtx1) << endl;
		cout << "    vtx2: " << MESHVERTEX_TO_VEC3F(vtx2) << endl;
		cout << "    vtx3: " << MESHVERTEX_TO_VEC3F(vtx3) << endl;

		cout << endl;

		SCapsule capsule;
		capsule.p1 = Vec3f(1.0f, 0.5f, -0.5f);
		capsule.p2 = Vec3f(1.0f, 0.5f, -10.0f);
		capsule.r = 0.5f;

		cout << "Capsule:" << endl;
		cout << "    p1 = " << capsule.p1 << endl;
		cout << "    p2 = " << capsule.p2 << endl;
		cout << "    radius = " << capsule.r << endl;

		cout << endl;

		bool intersect = IntersectTriangleCapsule(vtx1, vtx2, vtx3, capsule);
		std::cout << "Intersect triangle-capsule: " << intersect << std::endl;
	}
}

void test_quaternion()
{
	{
		Quat yaw = Quat::FromRotationY(SP_PI * 0.5f);
		Quat pitch = Quat::FromRotationX(SP_PI * 0.5f);
		Quat q = yaw * pitch;
		Vec3f v = Vec3f(0, 1.0f, 1.0f).Normalized();

		std::cout << "yaw = " << yaw << std::endl;
		std::cout << "pitch = " << pitch << std::endl;

		std::cout << "v=" << v << "  q=" << q << std::endl;
		std::cout << "vyaw=" << (yaw * v) << std::endl;
		std::cout << "vpitch=" << (pitch * v) << std::endl;
		std::cout << "vrotated=" << (q * v) << std::endl;

		std::cout << std::endl;
	}
	{
		Vec3f position(20.0f, 0, 0);
		Vec3f rotation(SP_PI * 0.5f * 0.5f, SP_PI * 0.5f, 0);

		Vec3f left(1.0f, 0, 0), up(0, 1.0f, 0), forward(0, 0, 1.0f);
		Quat yaw = Quat::FromRotationY(-rotation.y);
		Quat pitch = Quat::FromRotationX(rotation.x);
		Quat q = yaw * pitch;

		std::cout << "fyaw = " << (yaw * forward) << std::endl;
		std::cout << "fpitch = " << (pitch * forward) << std::endl;
		std::cout << "pitch = " << pitch << std::endl;
		std::cout << "q = " << q << std::endl;
		
		left = (q * left).Normalized();
		up = (q * up).Normalized();
		forward = (q * forward).Normalized();
		SMatrix mat1(
			left.x, up.x, forward.x, 0,
			left.y, up.y, forward.y, 0,
			left.z, up.z, forward.z, 0,
			-Vec3Dot(left, position), -Vec3Dot(up, position), -Vec3Dot(forward, position), 1.0f
			);

		
		Vec3f lookAt;
		lookAt.x = position.x + sinf(rotation.y) * cosf(rotation.x);
		lookAt.y = position.y + sinf(rotation.x);
		lookAt.z = position.z + cosf(rotation.y) * cosf(rotation.x);

		std::cout << "lookat = " << lookAt << std::endl;

		SMatrix mat2;
		SPMatrixLookAtRH(&mat2, position, lookAt, Vec3f(0, 1.0f, 0));


		std::cout << "mat1 =" << std::endl << mat1;
		std::cout << "mat2 =" << std::endl << mat2;
	}
}

template<typename T>
void logvar(const char* varname, T v)
{
	cout << "		- " << varname << " = " << v << endl;
}

bool HitsRay(const AABB& aabb, const Vec3f& p, const Vec3f& v, float* pLength, float* pTMin, float* pTMax)
{
	logvar("p", p);
	logvar("v", v);

	Vec3f invDir;

	invDir.x = 1.0f / v.x;
	invDir.y = 1.0f / v.y;
	invDir.z = 1.0f / v.z;

	logvar("invDir", invDir);

	float t1 = (aabb.vMin.x - p.x) * invDir.x;
	float t2 = (aabb.vMax.x - p.x) * invDir.x;
	float t3 = (aabb.vMin.y - p.y) * invDir.y;
	float t4 = (aabb.vMax.y - p.y) * invDir.y;
	float t5 = (aabb.vMin.z - p.z) * invDir.z;
	float t6 = (aabb.vMax.z - p.z) * invDir.z;

	cout << "		- t1 = " << t1 << "       R(t1) = " << (p + v * t1) << endl;	
	cout << "		- t2 = " << t2 << "       R(t2) = " << (p + v * t2) << endl;
	cout << "		- t3 = " << t3 << "       R(t3) = " << (p + v * t3) << endl;
	cout << "		- t4 = " << t4 << "       R(t4) = " << (p + v * t4) << endl;
	cout << "		- t5 = " << t5 << "       R(t5) = " << (p + v * t5) << endl;
	cout << "		- t6 = " << t6 << "       R(t6) = " << (p + v * t6) << endl;

	float tmin = max(max(min(t1, t2), min(t3, t4)), min(t5, t6));
	float tmax = min(min(max(t1, t2), max(t3, t4)), max(t5, t6));

	logvar("tmin", tmin);
	logvar("tmax", tmax);

	if (IS_VALID_PTR(pTMin))
		*pTMin = tmin;
	if (IS_VALID_PTR(pTMax))
		*pTMax = tmax;

	// if tmax < 0, ray (line) is intersecting AABB, but whole AABB is behind us
	if (tmax < 0)
	{
		logvar("t", tmax);
		if (IS_VALID_PTR(pLength))
			*pLength = tmax;

		logvar("intersection", false);
		return false;
	}

	// if tmin > tmax, ray doesn't intersect AABB
	if (tmin > tmax)
	{
		logvar("t", tmax);
		if (IS_VALID_PTR(pLength))
			*pLength = tmax;

		logvar("intersection", false);
		return false;
	}

	// Standard intersection
	if (IS_VALID_PTR(pLength))
		*pLength = tmin;

	logvar("t", tmin);
	logvar("intersection", true);
	return true;
}

void test_intersection_aabb_single(const AABB& aabb, const Vec3f& p, const Vec3f& v)
{
	float lambda, tmin, tmax;
	bool intersection = HitsRay(aabb, p, v, &lambda, &tmin, &tmax);
	cout << "==> AABB-Intersection (p=" << p << ", v=" << v << "): " << (intersection ? "YES" : "NO") << "   lambda = " << lambda << "   tmin = " << tmin << "   tmax = " << tmax << endl;
	cout << "		- Int = " << (p + v * lambda) << endl;
	cout << "		- Int(tmin) = " << (p + v * tmin) << endl;
	cout << "		- Int(tmax) = " << (p + v * tmax) << endl;
	cout << endl << endl;
}

void test_intersection_aabb()
{
	AABB aabb;
	aabb.vMin = Vec3f(0, 0, 0);
	aabb.vMax = Vec3f(10.f, 10.f, 10.f);

	cout << "AABB:  min = " << aabb.vMin << "    max = " << aabb.vMax << endl;

	test_intersection_aabb_single(aabb, Vec3f(15.f, 5.f, -5.f), Vec3f(-1.0f, 0, 0));
	test_intersection_aabb_single(aabb, Vec3f(-15.f, 5.f, -5.f), Vec3f(1.0f, 0, 0));
	cout << "Testing Ray start inside aabb..." << endl;
	test_intersection_aabb_single(aabb, Vec3f(5.f, 5.f, 5.f), Vec3f(0, 1.0f, 0));
}

void test_terrain_mesh_creation()
{
	cout << "Testing terrain mesh creation..." << endl;

	unsigned long m_nSegments = 8;
	float m_fSegSz = 1.0f;

	float SAMPLE_HEIGHT = 10.0f;

	cout << "   m_nSegments = " << m_nSegments << endl;
	cout << "   m_fSegSz = " << m_fSegSz << endl;
	cout << "   SAMPLE_HEIGHT = " << SAMPLE_HEIGHT << endl;

	vector<SMeshFace> faces;

	for (unsigned long iSegX = 0; iSegX < m_nSegments; ++iSegX)
	{
		for (unsigned long iSegY = 0; iSegY < m_nSegments; ++iSegY)
		{
			// face1	face2
			//  2     	2-----1
			//  | \   	  \   |
			//  |   \ 	    \ |
			//  0-----1	      0

			// Base pos (minimum index vertex)
			Vec3f bp(iSegX * m_fSegSz, 0, iSegY * m_fSegSz);

			SMeshFace face1, face2;
			face1.vtx[0].x = bp.x;
			face1.vtx[0].z = bp.z;
			face1.vtx[0].y = SAMPLE_HEIGHT;

			face1.vtx[1].x = bp.x + m_fSegSz;
			face1.vtx[1].z = bp.z;
			face1.vtx[1].y = SAMPLE_HEIGHT;

			face1.vtx[2].x = bp.x;
			face1.vtx[2].z = bp.z + m_fSegSz;
			face1.vtx[2].y = SAMPLE_HEIGHT;

			face2.vtx[0] = face1.vtx[1];

			face2.vtx[1].x = bp.x + m_fSegSz;
			face2.vtx[1].z = bp.z + m_fSegSz;
			face2.vtx[1].y = SAMPLE_HEIGHT;

			face2.vtx[2] = face1.vtx[2];

			faces.push_back(face1);
			faces.push_back(face2);
		}
	}

	float bias = 0.02f;

	AABB aabb;
	aabb.vMin = Vec3f(0, SAMPLE_HEIGHT - bias, 0);
	aabb.vMax = Vec3f((m_nSegments + 1) * m_fSegSz, SAMPLE_HEIGHT + bias, (m_nSegments + 1) * m_fSegSz);

	cout << "    aabb:   min = " << aabb.vMin << "    max = " << aabb.vMax << endl;

	SMesh mesh;
	mesh.Init(faces, aabb, eMESH_KTREE_QUADTREE, 2);
}


void test_aabb_linesegment_intersection(AABB& aabb, const Vec3f& p1, const Vec3f& p2)
{
	bool intersect = aabb.HitsLineSegment(p1, p2);

	cout << "AABB-Linesegment: p1 = " << p1 << ", p2 = " << p2 << "     --> " << (intersect ? "YES" : "no") << endl;
}

void test_aabb_linesegment_intersections()
{
	AABB aabb;
	aabb.vMin = Vec3f(0, 0, 0);
	aabb.vMax = Vec3f(10.f, 10.f, 10.f);

	test_aabb_linesegment_intersection(aabb, Vec3f(15.f, 5.f, 5.f), Vec3f(5.f, 5.f, 5.f));
	test_aabb_linesegment_intersection(aabb, Vec3f(15.f, 5.f, 5.f), Vec3f(25.f, 5.f, 5.f));
	test_aabb_linesegment_intersection(aabb, Vec3f(25.f, 5.f, 5.f), Vec3f(20.f, 5.f, 5.f));
}


void main()
{
	//test_intersections1();
	//test_intersections_triangle();
	//test_quaternion();

	//test_intersection_aabb();

	//test_terrain_mesh_creation();

	test_aabb_linesegment_intersections();

	std::cin.ignore();
}