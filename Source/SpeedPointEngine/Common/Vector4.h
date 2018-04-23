// ********************************************************************************************

//	SpeedPoint 4 Dimensional Vector

// ********************************************************************************************

#pragma once
#include "SAPI.h"
#include "Vector3.h"
#include <cmath>
#include <xmmintrin.h>

namespace SpeedPoint
{

	// SpeedPoint 4 Dimensional Vector
	template<typename F>
	struct S_API Vec4
	{		
		struct
		{
			F x, y, z, w;
		};			

		// ---

		Vec4<F>() : x(0), y(0), z(0), w(0) {}

		Vec4<F>(const Vec3<F>& v, F ww) : x(v.x), y(v.y), z(v.z), w(ww) {}

		Vec4<F>(F aa) : x(aa), y(aa), z(aa), w(aa) {}

		Vec4<F>(F xx, F yy, F zz) : x(xx), y(yy), z(zz), w(0) {}

		Vec4<F>(F xx, F yy, F zz, F ww) : x(xx), y(yy), z(zz), w(ww) {}

		// ---

		Vec4<F>& operator += (const Vec4<F>& v) { this->x += v.x; this->y += v.y; this->z += v.z; this->w += v.w; return *this; }

		Vec4<F>& operator -= (const Vec4<F>& v) { this->x -= v.x; this->y -= v.y; this->z -= v.z; this->w -= v.w; return *this; }

		Vec4<F>& operator *= (const Vec4<F>& v) { this->x *= v.x; this->y *= v.y; this->z *= v.z; this->w *= v.w; return *this; }

		Vec4<F>& operator /= (const Vec4<F>& v) { this->x /= v.x; this->y /= v.y; this->z /= v.z; this->w /= v.w; return *this; }

		Vec4<F> operator - () { return Vec4<F>(-x, -y, -z, -w); }

		Vec4<F>& operator = (const Vec4<F>& v)
		{
			x = v.x;
			y = v.y;
			z = v.z;
			w = v.w;
			return *this;
		}

		Vec3<F> xyz() const
		{
			return Vec3<F>(x, y, z);
		}
	};

	typedef struct S_API Vec4<float> Vec4f;
	typedef struct S_API Vec4<float> float4;
	typedef struct S_API Vec4<float> SVector4;
	typedef struct S_API Vec4<double> Vec4d;	

	template<typename F> inline bool operator == (const Vec4<F>& v1, const Vec4<F>& v2) { return (v1.x == v2.x && v1.y == v2.y && v1.z == v2.z && v1.w == v2.w); }
	template<typename F> inline Vec4<F> operator - (const Vec4<F>& va, const Vec4<F>& vb) { return Vec4<F>(va.x - vb.x, va.y - vb.y, va.z - vb.z, va.w - vb.w); }
	template<typename F> inline Vec4<F> operator - (const Vec4<F>& va, const F& f) { return Vec4<F>(va.x - f, va.y - f, va.z - f, va.w - f); }
	template<typename F> inline Vec4<F> operator - (const F& f, const Vec4<F>& va) { return Vec4<F>(f - va.x, f - va.y, f - va.z, f - va.w); }
	template<typename F> inline Vec4<F> operator + (const Vec4<F>& va, const Vec4<F>& vb) { return Vec4<F>(va.x + vb.x, va.y + vb.y, va.z + vb.z, va.w + vb.w); }
	template<typename F> inline Vec4<F> operator + (const Vec4<F>& va, const F& f) { return Vec4<F>(va.x + f, va.y + f, va.z + f, va.w + f); }
	template<typename F> inline Vec4<F> operator + (const F& f, const Vec4<F>& va) { return Vec4<F>(va.x + f, va.y + f, va.z + f, va.w + f); }
	template<typename F> inline Vec4<F> operator * (const Vec4<F>& va, const Vec4<F>& vb)
	{
		_mm_load_ps()
		return Vec4<F>(va.x * vb.x, va.y * vb.y, va.z * vb.z, va.w * vb.w);
	}

	template<typename F> inline Vec4<F> operator * (const Vec4<F>& va, const F& f) { return Vec4<F>(va.x * f, va.y * f, va.z * f, va.w * f); }
	template<typename F> inline Vec4<F> operator * (const F& f, const Vec4<F>& va) { return Vec4<F>(va.x * f, va.y * f, va.z * f, va.w * f); }
	template<typename F> inline Vec4<F> operator / (const Vec4<F>& va, const Vec4<F>& vb) { return Vec4<F>(va.x / vb.x, va.y / vb.y, va.z / vb.z, va.w / vb.w); }
	template<typename F> inline Vec4<F> operator / (const Vec4<F>& va, const F& f) { F finv = 1 / f; return Vec4<F>(va.x * finv, va.y * finv, va.z * finv, va.w * finv); }
	template<typename F> inline Vec4<F> operator / (const F& f, const Vec4<F>& va) { return Vec4<F>(f / va.x, f / va.y, f / va.z, f / va.w); }	

	template<typename F>
	inline float Dot(const Vec4<F>& va, const Vec4<F>& vb) {
		return va.x * vb.x + va.y * vb.y + va.z * vb.z + va.w * vb.w;
	}

// Warning: SVector4Dot is deprecated and will be replaced by overloaded Dot()
#define SVector4Dot SpeedPoint::Dot

	template<typename F>
	inline float Length(const Vec4<F>& v) {
		return sqrtf(v.x * v.x + v.y * v.y + v.z * v.z + v.w * v.w);
	}

// Warning: SVector4Length is deprecated and will be replaced by overloaded Length()
#define SVector4Length SpeedPoint::Length

	template<typename F>
	inline Vec4<F> Normalize(const Vec4<F>& v) {
		return v / sqrtf(v.x * v.x + v.y * v.y + v.z * v.z + v.w * v.w);
	}

// Warning: SVector4Normalize is deprecated and will be replaced by overloaded Normalize()
#define SVector4Normalize SpeedPoint::Normalize


	template<typename F>
	inline Vec4<F> Lerp(const Vec4<F>& a, const Vec4<F>& b, const float f)
	{
		return a + f * (b - a);
	}

}

#define S_DEFAULT_VEC4 SpeedPoint::Vec4<float>()