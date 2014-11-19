//////////////////////////////////////////////////////////////////////////////////
//
// This file is part of the SpeedPointEngine
// Copyright (c) 2011-2014, iSmokiieZz
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

#define FLOAT_THRESHOLD_F 0.0000001f

SP_NMSPACE_BEG

inline static bool float_equal(float f1, float f2)
{
	return f1 > (f2 - FLOAT_THRESHOLD_F) && f1 < (f2 + FLOAT_THRESHOLD_F);
}
inline static bool float_equal(double d1, double d2)
{
	return d1 > (d2 - FLOAT_THRESHOLD_F) && d1 < (d2 + FLOAT_THRESHOLD_F);
}

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


template<typename F>
struct S_API SphereT
{
	F radius;


	SphereT()
		: radius(1.0)
	{
	}

	SphereT(const SphereT<F>& s)
		: radius(s.radius)
	{
	}
};

typedef SphereT<float> SphereF;
typedef SphereT<double> SphereD;
typedef SphereF Sphere;


template<typename F>
static bool S_API Intersects(const SphereT<F>& s1, const SphereT<F>& s2)
{

}

SP_NMSPACE_END