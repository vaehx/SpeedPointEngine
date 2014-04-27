// ********************************************************************************************

//	SpeedPoint 4 Dimensional Vector

// ********************************************************************************************

#pragma once
#include "SAPI.h"
#include <cmath>

namespace SpeedPoint
{

	// SpeedPoint 4 Dimensional Vector
	struct S_API SVector4
	{
		float x;	// X Value of the vector
		float y;	// Y Value of the vector
		float z;	// Z Value of the vector
		float w;	// W Value of the vector

		// ---

		SVector4() : x(0), y(0), z(0), w(0) {};

		SVector4(float aa) : x(aa), y(aa), z(aa), w(aa) {};

		SVector4(float xx, float yy, float zz) : x(xx), y(yy), z(zz), w(0) {};

		SVector4(float xx, float yy, float zz, float ww) : x(xx), y(yy), z(zz), w(ww) {};

		// ---

		SVector4& operator += (const SVector4& v) { this->x += v.x; this->y += v.y; this->z += v.z; this->w += v.w; return *this; }

		SVector4& operator -= (const SVector4& v) { this->x -= v.x; this->y -= v.y; this->z -= v.z; this->w -= v.w; return *this; }

		SVector4& operator *= (const SVector4& v) { this->x *= v.x; this->y *= v.y; this->z *= v.z; this->w *= v.w; return *this; }

		SVector4& operator /= (const SVector4& v) { this->x /= v.x; this->y /= v.y; this->z /= v.z; this->w /= v.w; return *this; }

		SVector4 operator - () { return SVector4(-x, -y, -z, -w); }
	};

	typedef struct S_API SVector4 float4;

	inline bool operator == (const SVector4& v1, const SVector4& v2) { return (v1.x == v2.x && v1.y == v2.y && v1.z == v2.z && v1.w == v2.w); }
	inline SVector4 operator - (const SVector4& va, const SVector4& vb) { return SVector4(va.x - vb.x, va.y - vb.y, va.z - vb.z, va.w - vb.w); }
	inline SVector4 operator - (const SVector4& va, const float& f) { return SVector4(va.x - f, va.y - f, va.z - f, va.w - f); }
	inline SVector4 operator - (const float& f, const SVector4& va) { return SVector4(f - va.x, f - va.y, f - va.z, f - va.w); }
	inline SVector4 operator + (const SVector4& va, const SVector4& vb) { return SVector4(va.x + vb.x, va.y + vb.y, va.z + vb.z, va.w + vb.w); }
	inline SVector4 operator + (const SVector4& va, const float& f) { return SVector4(va.x + f, va.y + f, va.z + f, va.w + f); }
	inline SVector4 operator + (const float& f, const SVector4& va) { return SVector4(va.x + f, va.y + f, va.z + f, va.w + f); }
	inline SVector4 operator * (const SVector4& va, const SVector4& vb) { return SVector4(va.x * vb.x, va.y * vb.y, va.z * vb.z, va.w * vb.w); }
	inline SVector4 operator * (const SVector4& va, const float& f) { return SVector4(va.x * f, va.y * f, va.z * f, va.w * f); }
	inline SVector4 operator * (const float& f, const SVector4& va) { return SVector4(va.x * f, va.y * f, va.z * f, va.w * f); }
	inline SVector4 operator / (const SVector4& va, const SVector4& vb) { return SVector4(va.x / vb.x, va.y / vb.y, va.z / vb.z, va.w / vb.w); }
	inline SVector4 operator / (const SVector4& va, const float& f) { return SVector4(va.x / f, va.y / f, va.z / f, va.w / f); }
	inline SVector4 operator / (const float& f, const SVector4& va) { return SVector4(f / va.x, f / va.y, f / va.z, f / va.w); }	

	inline float SVector4Dot(const SVector4& va, const SVector4& vb) {
		return va.x * vb.x + va.y * vb.y + va.z * vb.z + va.w * vb.w;
	}

	inline float SVector4Length(const SVector4& v) {
		return sqrtf(v.x * v.x + v.y * v.y + v.z * v.z + v.w * v.w);
	}

	inline SVector4 SVector4Normalize(const SVector4& v) {
		return v / sqrtf(v.x * v.x + v.y * v.y + v.z * v.z + v.w * v.w);
	}

}

#define S_DEFAULT_VEC4 SpeedPoint::SVector4()
#define SPoint4 SpeedPoint::SVector4