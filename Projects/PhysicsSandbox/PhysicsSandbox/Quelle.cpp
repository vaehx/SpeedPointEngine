#include <iostream>
#include "Primitives.h"
#include "Quaternion.h"

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

		SCapsule capsule;
		capsule.p1 = Vec3f(1.0f, 0.5f, -0.50f);
		capsule.p2 = Vec3f(1.0f, 0.5f, -10.0f);
		capsule.r = 0.5f;

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


void main()
{
	//test_intersections1();
	test_intersections_triangle();
	//test_quaternion();

	std::cin.ignore();
}