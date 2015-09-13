//////////////////////////////////////////////////////////////////////////////////
//
// This file is part of the SpeedPointEngine
// Copyright (c) 2011-2015, iSmokiieZz
// ------------------------------------------------------------------------------
// Filename:	MathGeom.h
// Created:	11/18/2014 by iSmokiieZz
// Description:
// -------------------------------------------------------------------------------
// History:
//
//////////////////////////////////////////////////////////////////////////////////

#pragma once

#include <SPrerequisites.h>
#include "Vector3.h"
#include "Matrix.h"
#include "BoundBox.h"

#include "Math.h"

SP_NMSPACE_BEG

////////////////////////////////////////////////////////////////////////////////////////////////////////////////
////////////////////////////////////////////////////////////////////////////////////////////////////////////////
//					P o i n t
////////////////////////////////////////////////////////////////////////////////////////////////////////////////
////////////////////////////////////////////////////////////////////////////////////////////////////////////////

template<typename F>
struct S_API PointT
{
	F x, y, z;

	PointT() : x(0.0), y(0.0), z(0.0) {}
	PointT(const PointT<F>& p) : x(p.x), y(p.y), z(p.z) {}

	bool operator ==(const PointT<F>& p) const
	{


		// TODO



	}
};



////////////////////////////////////////////////////////////////////////////////////////////////////////////////
////////////////////////////////////////////////////////////////////////////////////////////////////////////////
//					 R a y
////////////////////////////////////////////////////////////////////////////////////////////////////////////////
////////////////////////////////////////////////////////////////////////////////////////////////////////////////

template<typename F>
struct S_API RayT
{
	Vec3<F> p;
	Vec3<F> v;

	RayT() : p(0, 0, 0), v(0, 0, 0) {}
	RayT(const RayT<F>& rhs) : p(rhs.p), v(rhs.v) {}
	RayT(const Vec3<F>& pp, const Vec3<F>& vv) : p(pp), v(vv) {}

	// p = p1, v = p2 - p1
	ILINE static RayT<F> FromPoints(const Vec3<F>& p1, const Vec3<F>& p2) {
		return RayT<F>(p1, p2 - p1)
	}
};

typedef RayT<float> S_API SRay;

template<typename F>
ILINE F GeomDistance(const RayT<F>& ray1, const RayT<F>& ray2) {
	Vec3<F> v1xv2 = Vec3Cross(ray1.v, ray2.v);
	F d = v1xv2.Length();
	if (float_equal(d, 0))
		return 0; // parallel

	return float_abs(Vec3Dot(ray1.p - ray2.p, v1xv2)) / d;
}



////////////////////////////////////////////////////////////////////////////////////////////////////////////////
////////////////////////////////////////////////////////////////////////////////////////////////////////////////
//					S p h e r e
////////////////////////////////////////////////////////////////////////////////////////////////////////////////
////////////////////////////////////////////////////////////////////////////////////////////////////////////////

template<typename F>
struct S_API SphereT
{
	F radius;
	Vec3<F> center;

	SphereT()
		: radius(1.0)
	{
	}

	SphereT(const SphereT<F>& s)
		: radius(s.radius),
		center(s.center)
	{
	}
};

typedef SphereT<float> SphereF;
typedef SphereT<double> SphereD;
typedef SphereF Sphere;


template<typename F>
static bool S_API Intersects(const SphereT<F>& s1, const SphereT<F>& s2)
{
	return false;
}






////////////////////////////////////////////////////////////////////////////////////////////////////////////////
////////////////////////////////////////////////////////////////////////////////////////////////////////////////
//					P l a n e
////////////////////////////////////////////////////////////////////////////////////////////////////////////////
////////////////////////////////////////////////////////////////////////////////////////////////////////////////

// SpeedPoint Plane
template<typename F>
struct S_API PlaneT
{
	Vec3<F> n;
	F d;
	
	PlaneT() {}
	PlaneT(const PlaneT<F>& p) : n(p.n) {}
	PlaneT(const float a, const float b, const float c, const float dd) : n(a, b, c), d(dd) {}	
	PlaneT(const Vec3<F>& nn, float dd) : n(nn), d(dd) {}		
	
	ILINE PlaneT<F>& operator = (const PlaneT<F>& p) {
		n = p.n;
		d = p.d;
		return *this;
	}

	ILINE static PlaneT<F> FromHeight(float h) {
		return PlaneT<F>(0, 1.0f, 0, h);
	}
};

typedef PlaneT<float> S_API SPlane;
typedef PlaneT<double> S_API PlaneD;

template<typename F1, typename F2>
ILINE bool operator == (const PlaneT<F1>& a, const PlaneT<F2>& b) {
	if (a.a != b.a) return false;
	if (a.b != b.b) return false;
	if (a.c != b.c) return false;
	return a.d == b.d;
}

template<typename F1, typename F2>
ILINE bool operator != (const PlaneT<F1>& a, const PlaneT<F2>& b) {
	if (a.a != b.a) return true;
	if (a.b != b.b) return true;
	if (a.c != b.c) return true;
	return a.d != b.d;
}

template<typename F>
ILINE PlaneT<F> PlaneNormalize(const PlaneT<F>& p) {
	const F fLength = p.n.Length();
	return PlaneT<F>(p.n / fLength, p.d / fLength);
}

template<typename F>
ILINE PlaneT<F> PlaneFromPoints(const Vec3<F>& v1, const Vec3<F>& v2, const Vec3<F>& v3) {
	PlaneT<F> p;
	p.n = Vec3Normalize(Vec3Cross(v2 - v1, v3 - v1));
	p.d = Vec3Dot(p.n, v1);
	return p;
}


// Returns false if the plane and the ray are parallel
template<typename F>
ILINE bool GeomIntersects(const PlaneT<F>& plane, const RayT<F>& ray, Vec3<F>* ipoint)
{
	F denominator = Vec3Dot(plane.n, ray.v);

	// check if plane normal and ray perpendicular
	if (float_equal(denominator, 0))
		return false;
	
	if (ipoint)
	{
		F s = (plane.d - Vec3Dot(plane.n, ray.p)) / denominator;
		*ipoint = ray.p + s * ray.v;
	}

	return true;
}






////////////////////////////////////////////////////////////////////////////////////////////////////////////////
////////////////////////////////////////////////////////////////////////////////////////////////////////////////
//					V i e w   F r u s t u m
////////////////////////////////////////////////////////////////////////////////////////////////////////////////
////////////////////////////////////////////////////////////////////////////////////////////////////////////////

enum EFrustumPlane
{
	eFRUSTUMPLANE_LEFT = 0,
	eFRUSTUMPLANE_RIGHT,
	eFRUSTUMPLANE_BOTTOM,
	eFRUSTUMPLANE_TOP,
	eFRUSTUMPLANE_NEAR,
	eFRUSTUMPLANE_FAR
};

struct S_API SViewFrustum
{
	PlaneT<float> planes[6];

	SViewFrustum() {};
	SViewFrustum(const SViewFrustum& frustum)
	{
		for (unsigned int i = 0; i < 6; ++i)
			planes[i] = frustum.planes[i];
	}

	// If vp equals the combined view and projection matrices, the frustum planes will be the clipping planes in world space.	
	ILINE SViewFrustum& BuildFromViewProjMatrix(const SMatrix4& vp)
	{
		planes[eFRUSTUMPLANE_LEFT  ] = PlaneT<float>(vp._14 + vp._11,	vp._24 + vp._21,	vp._34 + vp._31,	vp._44 + vp._41);
		planes[eFRUSTUMPLANE_RIGHT ] = PlaneT<float>(vp._14 - vp._11,	vp._24 - vp._21,	vp._34 - vp._31,	vp._44 - vp._41);
		planes[eFRUSTUMPLANE_BOTTOM] = PlaneT<float>(vp._14 + vp._12,	vp._24 + vp._22,	vp._34 + vp._32,	vp._44 + vp._42);
		planes[eFRUSTUMPLANE_TOP   ] = PlaneT<float>(vp._14 - vp._12,	vp._24 - vp._22,	vp._34 - vp._32,	vp._44 - vp._42);
		planes[eFRUSTUMPLANE_NEAR  ] = PlaneT<float>(vp._13,		vp._23,			vp._33,			vp._43);
		planes[eFRUSTUMPLANE_FAR   ] = PlaneT<float>(vp._14 - vp._13,	vp._24 - vp._23,	vp._34 - vp._33,	vp._44 - vp._43);
		
		return *this;
	}
};















////////////////////////////////////////////////////////////////////////////////////////////////////////////////
////////////////////////////////////////////////////////////////////////////////////////////////////////////////
////////////////////////////////////////////////////////////////////////////////////////////////////////////////
////////////////////////////////////////////////////////////////////////////////////////////////////////////////
//
//					I N T E R S E C T I O N S
//
////////////////////////////////////////////////////////////////////////////////////////////////////////////////
////////////////////////////////////////////////////////////////////////////////////////////////////////////////
////////////////////////////////////////////////////////////////////////////////////////////////////////////////
////////////////////////////////////////////////////////////////////////////////////////////////////////////////


template<typename F>
bool RayHitsAABB(const RayT<F>& ray, const AABB& aabb, float* pLength = NULL)
{
	return aabb.HitsRay(ray.v, ray.p, pLength);
}








SP_NMSPACE_END