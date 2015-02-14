//////////////////////////////////////////////////////////////////////////////////
//
// This file is part of the SpeedPointEngine
// Copyright (c) 2011-2015, Pascal Rosenkranz
// ------------------------------------------------------------------------------
// Filename:	Vector3.h
// Created:	1/1/2011 by Pascal Rosenkranz
// Description:
// -------------------------------------------------------------------------------
// History:
//
//////////////////////////////////////////////////////////////////////////////////

#pragma once
#include "SAPI.h"
#include <cmath>

// use namespace here to prevent loading SPrerequisites.h unnecessarily

namespace SpeedPoint
{



	// sometimes we need a simple 3-Dimensional structure for the components in a unit
	// The SVector3 structure has a custom default constructor, that's why it does not work with units
	struct S_API SXYZ
	{
		float x, y, z;
	};



	// template math functions:

	inline float finv(const float& f) { return 1.0f / f; }
	inline double finv(const double& d) { return 1.0 / d; }
	inline float sqrt_t(const float& f) { return sqrtf(f); }
	inline double sqrt_t(const double& d) { return sqrt(d); }

	// SpeedPoint 3-Dimensional Vector
	template<typename F> struct S_API Vec3
	{
		F x, y, z;

		Vec3() : x(0), y(0), z(0) {}
		Vec3(F k) : x(k), y(k), z(k) {}
		Vec3(F xx, F yy, F zz) : x(xx), y(yy), z(zz) {}
		Vec3(const Vec3<F>& v) : x(v.x), y(v.y), z(v.z) {}

		Vec3<F> operator +(const Vec3<F>& v) const { return Vec3<F>(x + v.x, y + v.y, z + v.z); }
		Vec3<F> operator +(const F& k) const { return Vec3<F>(x + k, y + k, z + k); }
		Vec3<F>& operator +=(const Vec3<F>& v) { x += v.x; y += v.y; z += v.z; return *this; }
		Vec3<F>& operator +=(const F& k) { x += k; y += k; z += k; return *this; }

		Vec3<F> operator -(const Vec3<F>& v) const { return Vec3<F>(x - v.x, y - v.y, z - v.z); }
		Vec3<F> operator -(const F& k) const { return Vec3<F>(x - k, y - k, z - k); }
		Vec3<F>& operator -=(const Vec3<F>& v) { x -= v.x; y -= v.y; z -= v.z; return *this; }
		Vec3<F>& operator -=(const F& k) { x -= k; y -= k; z -= k; return *this; }

		Vec3<F> operator *(const Vec3<F>& v) const { return Vec3<F>(x * v.x, y * v.y, z * v.z); }
		Vec3<F> operator *(const F& k) const { return Vec3<F>(x * k, y * k, z * k); }
		Vec3<F>& operator *=(const Vec3<F>& v) { x *= v.x; y *= v.y; z *= v.z; return *this; }
		Vec3<F>& operator *=(const F& k) { x *= k; y *= k; z *= k; return *this; }

		Vec3<F> operator /(const Vec3<F>& v) const { return Vec3<F>(x / v.x, y / v.y, z / v.z); }
		Vec3<F> operator /(const F& k) const { F k_inv = finv(k); / k; return Vec3<F>(x * k_inv, y * k_inv, z * k_inv); }
		Vec3<F>& operator /=(const Vec3<F>& v) { x /= v.x; y /= v.y; z /= v.z; return *this; }
		Vec3<F>& operator /=(const F& k) { F k_inv = finv(k); x *= k_inv; y *= k_inv; z *= k_inv; return *this; }

		inline F Dot(const Vec3<F>& v) const
		{
			return x * v.x + y * v.y + z * v.z;
		}

		inline Vec3<F> Cross(const Vec3<F>& v) const
		{
			return Vec3<F>(y * v.z - z * v.y,
				z * v.x - x * v.z,
				x * v.y - y * v.x);
		}

		inline F Length() const
		{
			return sqrt_t(x * x + y * y + z * z);
		}

		inline F LengthSq() const
		{
			return x * x + y * y + z * z;
		}

		inline Vec3<F>& Normalized() const
		{
			F invLn = finv(Length());
			return Vec3<F>(x * invLn, y * invLn, z * invLn);
		}

		inline Vec3<F>& CheckMin(const Vec3<F>& v)
		{
			if (v.x < x) x = v.x;
			if (v.y < y) y = v.y;
			if (v.z < z) z = v.z;
			return *this;
		}

		inline Vec3<F>& CheckMax(const Vec3<F>& v)
		{
			if (v.x > x) x = v.x;
			if (v.y > y) y = v.y;
			if (v.z > z) z = v.z;
			return *this;
		}
	};

	template<typename F>
	S_API inline F Vec3Dot(const Vec3<F>& v1, const Vec3<F>& v2)
	{
		return v1.Dot(v2);
	}

	template<typename F>
	S_API inline Vec3<F> Vec3Cross(const Vec3<F>& v1, const Vec3<F>& v2)
	{
		return v1.Cross(v2);
	}

	template<typename F>
	S_API inline Vec3<F> Vec3Normalize(const Vec3<F>& v)
	{
		return v.Normalized();
	}


	// specific vectors

	typedef Vec3<float> S_API Vec3f;
	typedef Vec3<double> S_API Vec3d;









	// SpeedPoint 3 Dimensional Vector
	struct S_API SVector3
	{
		float x;	// X Value of the vector
		float y;	// Y Value of the vector
		float z;	// Z Value of the vector

		// ---

		SVector3() : x(0), y(0), z(0) {};

		SVector3(float aa) : x(aa), y(aa), z(aa) {};

		SVector3(float xx, float yy, float zz) : x(xx), y(yy), z(zz) {};

		SVector3(const SVector3& v) : x(v.x), y(v.y), z(v.z) {};

		// ---

		SVector3& operator += (const SVector3& v) { this->x += v.x; this->y += v.y; this->z += v.z; return *this; }

		SVector3& operator -= (const SVector3& v) { this->x -= v.x; this->y -= v.y; this->z -= v.z; return *this; }

		SVector3& operator *= (const SVector3& v) { this->x *= v.x; this->y *= v.y; this->z *= v.z; return *this; }

		SVector3& operator /= (const SVector3& v) { this->x /= v.x; this->y /= v.y; this->z /= v.z; return *this; }

		SVector3 operator - () { return SVector3(-x, -y, -z); }

		// ---

		// conversion operator to a simple 3-dimensional object
		operator SXYZ() const
		{
			SXYZ out;
			out.x = x; out.y = y; out.z = z;
			return out;
		}

		// ---

		// Cross product with another vector
		SVector3 Cross(const SVector3& o) const
		{
			SVector3 res;
			res.x = y * o.z - z * o.y;
			res.y = z * o.x - x * o.z;
			res.z = x * o.y - y * o.x;
			return res;
		}

		// Dot product with another vector
		float Dot(const SVector3& o)
		{
			return x * o.x + y * o.y + z * o.z;
		}

		// Length of this vector
		float Length()
		{
			return sqrtf(x*x + y*y + z*z);
		}

		// Square of the length
		float Square()
		{
			return x*x + y*y + z*z;
		}

		void CheckMin(const SVector3& v)
		{
			if (v.x < x) x = v.x;
			if (v.y < y) y = v.y;
			if (v.z < z) z = v.z;
		}

		void CheckMax(const SVector3& v)
		{
			if (v.x > x) x = v.x;
			if (v.y > y) y = v.y;
			if (v.z > z) z = v.z;
		}

		// Get unit vector of this vector
		SVector3& Normalize(float* pLength = 0)
		{
			float l = this->Length();
			this->x /= l; this->y /= l; this->z /= l;

			if (pLength != 0) *pLength = l;
			return *this;
		}
	};

	typedef struct S_API SVector3 float3;

	inline bool operator == (const SVector3& v1, const SVector3& v2) { return (v1.x == v2.x && v1.y == v2.y && v1.z == v2.z); }
	inline bool operator != (const SVector3& v1, const SVector3& v2) { return (v1.x != v2.x || v1.y != v2.y || v1.z != v2.z); }
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

	////////////
	// Following vector manipulation methods were kept to prevent the rest of the source from being invalid
	// As it takes a lot of work to change all those occurances of these functions, we'll do this later.

	// TODO: Refactor all occurencies of following functions to the corresponding member functions
	inline SVector3 SVector3Cross(const SVector3& va, const SVector3& vb)
	{
		return SVector3(va.Cross(vb));
	}

	inline float SVector3Dot(const SVector3& va, const SVector3& vb)
	{
		SVector3 a(va);
		return a.Dot(vb);
	}

	inline float SVector3Length(const SVector3& v)
	{
		SVector3 a(v);
		return a.Length();
	}

	inline SVector3 SVector3Normalize(const SVector3& v)
	{
		SVector3 a(v);
		a.Normalize();
		return a;
	}
	/////////////

#define S_DEFAULT_VEC3 SpeedPoint::SVector3()
#define SPoint3 SpeedPoint::SVector3



}