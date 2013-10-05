// ********************************************************************************************

//	SpeedPoint 3 Dimensional Vector

// ********************************************************************************************

#pragma once
#include "SAPI.h"
#include <math.h>

namespace SpeedPoint
{

	// SpeedPoint 3 Dimensional Vector
	struct S_API SVector3
	{
		float x;	// X Value of the vector
		float y;	// Y Value of the vector
		float z;	// Z Value of the vector

		// ---

		SVector3() : x(0), y(0), z(0) {};
		
		SVector3( float aa ) : x(aa), y(aa), z(aa) {};
		
		SVector3( float xx, float yy, float zz ) : x(xx), y(yy), z(zz) {};

		// ---

		SVector3& operator += ( const SVector3& v ) { this->x += v.x; this->y += v.y; this->z += v.z; return *this; }
		
		SVector3& operator -= ( const SVector3& v ) { this->x -= v.x; this->y -= v.y; this->z -= v.z; return *this; }		
		
		SVector3 operator - () { return SVector3( -x, -y, -z ); }
	};

	typedef struct S_API SVector3 float3;
	
	inline bool operator == ( const SVector3& v1, const SVector3& v2 ) { return ( v1.x == v2.x && v1.y == v2.y && v1.z == v2.z ); }
	inline SVector3 operator - (const SVector3& va, const SVector3& vb) { return SVector3(va.x - vb.x, va.y - vb.y, va.z - vb.z); }
	inline SVector3 operator - (const SVector3& va, const float& f) { return SVector3(va.x - f, va.y - f, va.z - f); }
	inline SVector3 operator - (const float& f, const SVector3& va) { return SVector3(va.x - f, va.y - f, va.z - f); }
	inline SVector3 operator + (const SVector3& va, const SVector3& vb) { return SVector3(va.x + vb.x, va.y + vb.y, va.z + vb.z); }
	inline SVector3 operator + (const SVector3& va, const float& f) { return SVector3(va.x + f, va.y + f, va.z + f); }
	inline SVector3 operator + (const float& f, const SVector3& va) { return SVector3(va.x + f, va.y + f, va.z + f); }
	inline SVector3 operator * (const SVector3& va, const SVector3& vb) { return SVector3(va.x * vb.x, va.y * vb.y, va.z * vb.z); }
	inline SVector3 operator * (const SVector3& va, const float& f) { return SVector3(va.x * f, va.y * f, va.z * f); }
	inline SVector3 operator * (const float& f, const SVector3& va) { return SVector3(va.x * f, va.y * f, va.z * f); }
	inline SVector3 operator / (const SVector3& va, const SVector3& vb) { return SVector3(va.x / vb.x, va.y / vb.y, va.z / vb.z); }
	inline SVector3 operator / (const SVector3& va, const float& f) { return SVector3(va.x / f, va.y / f, va.z / f); }
	inline SVector3 operator / (const float& f, const SVector3& va) { return SVector3(va.x / f, va.y / f, va.z / f); }
	
	inline SVector3 SVector3Cross( const SVector3& va, const SVector3& vb ) {
		return SVector3( va.y * vb.z - va.z * vb.y, va.z * vb.x - va.x * vb.z, va.x * vb.y - va.y * vb.x);
	}
	
	inline float SVector3Dot( const SVector3& va, const SVector3& vb ) {
		return va.x * vb.x + va.y * vb.y + va.z * vb.z;
	}

	inline float SVector3Length(const SVector3& v) {
		return sqrtf(v.x * v.x + v.y * v.y + v.z * v.z);
	}
	
	inline SVector3 SVector3Normalize(const SVector3& v) {
		return v / sqrtf(v.x * v.x + v.y * v.y + v.z * v.z);
	}	

}

#define S_DEFAULT_VEC3 SpeedPoint::SVector3()
#define SPoint3 SpeedPoint::SVector3