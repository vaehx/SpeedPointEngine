// ********************************************************************************************

//	SpeedPoint 2 Dimensional Vector

// ********************************************************************************************

#pragma once
#include "SAPI.h"
#include <cmath>

namespace SpeedPoint
{

	// SpeedPoint 2 Dimensional Vector
	struct S_API SVector2
	{
		float x;	// X Value of the vector
		float y;	// Y Value of the vector

		// ---

		SVector2() : x(0), y(0) {};
		
		SVector2( float aa ) : x(aa), y(aa) {};
		
		SVector2( float xx, float yy ) : x(xx), y(yy) {};

		// ---

		SVector2& operator += ( const SVector2& v ) { this->x += v.x; this->y += v.y; return *this; }
											      
		SVector2& operator -= ( const SVector2& v ) { this->x -= v.x; this->y -= v.y; return *this; }		
		
		SVector2 operator - () { return SVector2( -x, -y ); }
	};

	typedef struct S_API SVector2 float2;
	
	inline bool operator == ( const SVector2& v1, const SVector2& v2 ) { return ( v1.x == v2.x && v1.y == v2.y ); }
	inline SVector2 operator - (const SVector2& va, const SVector2& vb) { return SVector2(va.x - vb.x, va.y - vb.y); }
	inline SVector2 operator - (const SVector2& va, const float& f) { return SVector2(va.x - f, va.y - f); }
	inline SVector2 operator - (const float& f, const SVector2& va) { return SVector2(va.x - f, va.y - f); }
	inline SVector2 operator + (const SVector2& va, const SVector2& vb) { return SVector2(va.x + vb.x, va.y + vb.y); }
	inline SVector2 operator + (const SVector2& va, const float& f) { return SVector2(va.x + f, va.y + f); }
	inline SVector2 operator + (const float& f, const SVector2& va) { return SVector2(va.x + f, va.y + f); }
	inline SVector2 operator * (const SVector2& va, const SVector2& vb) { return SVector2(va.x * vb.x, va.y * vb.y); }
	inline SVector2 operator * (const SVector2& va, const float& f) { return SVector2(va.x * f, va.y * f); }
	inline SVector2 operator * (const float& f, const SVector2& va) { return SVector2(va.x * f, va.y * f); }
	inline SVector2 operator / (const SVector2& va, const SVector2& vb) { return SVector2(va.x / vb.x, va.y / vb.y); }
	inline SVector2 operator / (const SVector2& va, const float& f) { return SVector2(va.x / f, va.y / f); }
	inline SVector2 operator / (const float& f, const SVector2& va) { return SVector2(va.x / f, va.y / f); }
	
	inline float SVector2Dot( const SVector2& va, const SVector2& vb ) {
		return va.x * vb.x + va.y * vb.y;
	}

	inline float SVector2Length(const SVector2& v) {
		return sqrtf(v.x * v.x + v.y * v.y);
	}
	
	inline SVector2 SVector2Normalize(const SVector2& v) {
		return v / sqrtf(v.x * v.x + v.y * v.y);
	}	

}

#define S_DEFAULT_VEC2 SVector2()
#define SPoint2 SVector2
#define float2 SVector2