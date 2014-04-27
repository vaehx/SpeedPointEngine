// ********************************************************************************************

//	SpeedPoint Plane

// ********************************************************************************************

#pragma once

#include <SPrerequisites.h>
#include "SVector3.h"

namespace SpeedPoint
{

	// SpeedPoint Plane
	class S_API SPlane
	{
	public:	
		union
		{
			struct
			{
				float a, b, c, d;			
			};
			struct
			{
				SVector3	n;
				float		d;
			};
	
			float v[4];
		};
	
		// Konstruktoren
		SPlane() {}
		SPlane(const SPlane& p) : a(p.a), b(p.b), c(p.c), d(p.d), n(p.n) {}
		SPlane(const float _a, const float _b, const float _c, const float _d) : a(_a), b(_b), c(_c), d(_d) {}
		SPlane(const float* pfValue) : a(pfValue[0]), b(pfValue[1]), c(pfValue[2]), d(pfValue[3]) {}
		SPlane(const SVector3& _n, float _d) : n(_n), d(_d) {}
	
		// Casting-Operatoren
		operator float* () {return (float*)(v);}
	
		// Zuweisungsoperatoren
		SPlane& operator = (const SPlane& p) {a = p.a; b = p.b; c = p.c; d = p.d; return *this;}	
	};
	
	inline bool operator == (const SPlane& a, const SPlane& b) {if(a.a != b.a) return false; if(a.b != b.b) return false; if(a.c != b.c) return false; return a.d == b.d;}
	inline bool operator != (const SPlane& a, const SPlane& b) {if(a.a != b.a) return true; if(a.b != b.b) return true; if(a.c != b.c) return true; return a.d != b.d;}
	
	inline SPlane	SPlaneNormalize(const SPlane& p)						{const float fLength = SVector3Length(p.n); return SPlane(p.n / fLength, p.d / fLength);}
	inline float	SPlaneDotNormal(const SPlane& p, const SVector3& v)				{return p.a * v.x + p.b * v.y + p.c * v.z;}
	inline float	SPlaneDotCoords(const SPlane& p, const SVector3& v)				{return p.a * v.x + p.b * v.y + p.c * v.z + p.d;}
	inline SPlane	SPlaneFromPointNormal(const SVector3& p, const SVector3& n)			{return SPlane(n, -n.x * p.x - n.y * p.y - n.z * p.z);}
	inline SPlane	SPlaneFromPoints(const SVector3& v1, const SVector3& v2, const SVector3& v3)	{return SPlaneFromPointNormal(v1, SVector3Cross(v3 - v2, v1 - v2));}

}