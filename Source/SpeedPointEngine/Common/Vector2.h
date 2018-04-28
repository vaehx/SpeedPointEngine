// ********************************************************************************************

//	SpeedPoint 2 Dimensional Vector

// ********************************************************************************************

#pragma once
#include "SAPI.h"
#include <cmath>

namespace SpeedPoint
{

	// SpeedPoint 2 Dimensional Vector
	template<typename F>
	struct S_API Vec2
	{
		F x;	// X Value of the vector
		F y;	// Y Value of the vector

		// ---

		Vec2() : x(0), y(0) {};
		
		Vec2( F aa ) : x(aa), y(aa) {};
		
		Vec2( F xx, F yy ) : x(xx), y(yy) {};

		// ---

		Vec2<F>& operator += (const Vec2<F>& v) { x += v.x; y += v.y; return *this; }		
		Vec2<F>& operator -= (const Vec2<F>& v) { x -= v.x; y -= v.y; return *this; }
		Vec2<F>& operator *= (const Vec2<F>& v) { x *= v.x; y *= v.y; return *this; }
		Vec2<F>& operator /= (const Vec2<F>& v) { x /= v.x; y /= v.y; return *this; }

		Vec2<F>& operator += (float k) { x += k; y += k; return *this; }
		Vec2<F>& operator -= (float k) { x -= k; y -= k; return *this; }
		Vec2<F>& operator *= (float k) { x *= k; y *= k; return *this; }
		Vec2<F>& operator /= (float k) { x /= k; y /= k; return *this; }
		
		Vec2<F> operator - () { return Vec2<F>( -x, -y ); }
	};	
	
	template<typename F>
	inline bool operator == ( const Vec2<F>& v1, const Vec2<F>& v2 ) { return ( v1.x == v2.x && v1.y == v2.y ); }

	template<typename F>
	inline Vec2<F> operator - (const Vec2<F>& va, const Vec2<F>& vb) { return Vec2<F>(va.x - vb.x, va.y - vb.y); }

	template<typename F>
	inline Vec2<F> operator - (const Vec2<F>& va, const F& f) { return Vec2<F>(va.x - f, va.y - f); }

	template<typename F>
	inline Vec2<F> operator - (const F& f, const Vec2<F>& va) { return Vec2<F>(f - va.x, f - va.y); }

	template<typename F>
	inline Vec2<F> operator + (const Vec2<F>& va, const Vec2<F>& vb) { return Vec2<F>(va.x + vb.x, va.y + vb.y); }

	template<typename F>
	inline Vec2<F> operator + (const Vec2<F>& va, const F& f) { return Vec2<F>(va.x + f, va.y + f); }

	template<typename F>
	inline Vec2<F> operator + (const F& f, const Vec2<F>& va) { return Vec2<F>(va.x + f, va.y + f); }

	template<typename F>
	inline Vec2<F> operator * (const Vec2<F>& va, const Vec2<F>& vb) { return Vec2<F>(va.x * vb.x, va.y * vb.y); }

	template<typename F>
	inline Vec2<F> operator * (const Vec2<F>& va, const F& f) { return Vec2<F>(va.x * f, va.y * f); }

	template<typename F>
	inline Vec2<F> operator * (const F& f, const Vec2<F>& va) { return Vec2<F>(va.x * f, va.y * f); }

	template<typename F>
	inline Vec2<F> operator / (const Vec2<F>& va, const Vec2<F>& vb) { return Vec2<F>(va.x / vb.x, va.y / vb.y); }

	template<typename F>
	inline Vec2<F> operator / (const Vec2<F>& va, const F& f) { return Vec2<F>(va.x / f, va.y / f); }

	template<typename F>
	inline Vec2<F> operator / (const F& f, const Vec2<F>& va) { return Vec2<F>(f / va.x, f / va.y); }

	template<typename F>
	inline Vec2<F> operator % (const Vec2<F>& va, const Vec2<F>& vb) { return Vec2<F>(fmod(va.x, vb.x), fmod(va.y, vb.y)); }
	
	template<typename F>
	inline float Vec2Dot( const Vec2<F>& va, const Vec2<F>& vb ) {
		return va.x * vb.x + va.y * vb.y;
	}

	template<typename F>
	inline float Vec2Length(const Vec2<F>& v) {
		return sqrtf(v.x * v.x + v.y * v.y);
	}
	
	template<typename F>
	inline Vec2<F> Vec2Normalize(const Vec2<F>& v) {
		return v / sqrtf(v.x * v.x + v.y * v.y);
	}	


	typedef Vec2<float> S_API Vec2f;
	typedef Vec2f S_API SVector2;
	typedef Vec2f S_API float2;

}

#define S_DEFAULT_VEC2 SVector2()
#define SPoint2 SVector2
#define float2 SVector2