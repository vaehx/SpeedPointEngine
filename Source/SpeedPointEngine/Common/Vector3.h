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
#include <ostream>

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
	template<typename F>
	struct S_API Vec3
	{
		F x, y, z;

		Vec3() : x(0), y(0), z(0) {}
		Vec3(F k) : x(k), y(k), z(k) {}
		Vec3(F xx, F yy, F zz) : x(xx), y(yy), z(zz) {}
		Vec3(const Vec3<F>& v) : x(v.x), y(v.y), z(v.z) {}

		inline Vec3<F> operator -() const
		{
			return Vec3<F>(-x, -y, -z);
		}

		inline Vec3<F> operator +(const Vec3<F>& v) const { return Vec3<F>(x + v.x, y + v.y, z + v.z); }
		inline Vec3<F> operator +(const F& k) const { return Vec3<F>(x + k, y + k, z + k); }
		inline Vec3<F>& operator +=(const Vec3<F>& v) { x += v.x; y += v.y; z += v.z; return *this; }
		inline Vec3<F>& operator +=(const F& k) { x += k; y += k; z += k; return *this; }

		inline Vec3<F> operator -(const Vec3<F>& v) const { return Vec3<F>(x - v.x, y - v.y, z - v.z); }
		inline Vec3<F> operator -(const F& k) const { return Vec3<F>(x - k, y - k, z - k); }
		inline Vec3<F>& operator -=(const Vec3<F>& v) { x -= v.x; y -= v.y; z -= v.z; return *this; }
		inline Vec3<F>& operator -=(const F& k) { x -= k; y -= k; z -= k; return *this; }

		inline Vec3<F> operator *(const Vec3<F>& v) const { return Vec3<F>(x * v.x, y * v.y, z * v.z); }
		inline Vec3<F> operator *(const F& k) const { return Vec3<F>(x * k, y * k, z * k); }
		inline Vec3<F>& operator *=(const Vec3<F>& v) { x *= v.x; y *= v.y; z *= v.z; return *this; }
		inline Vec3<F>& operator *=(const F& k) { x *= k; y *= k; z *= k; return *this; }

		inline Vec3<F> operator /(const Vec3<F>& v) const { return Vec3<F>(x / v.x, y / v.y, z / v.z); }
		inline Vec3<F> operator /(const F& k) const { F k_inv = finv(k); return Vec3<F>(x * k_inv, y * k_inv, z * k_inv); }
		inline Vec3<F>& operator /=(const Vec3<F>& v) { x /= v.x; y /= v.y; z /= v.z; return *this; }
		inline Vec3<F>& operator /=(const F& k) { F k_inv = finv(k); x *= k_inv; y *= k_inv; z *= k_inv; return *this; }

		inline F& operator [](unsigned int i) { return ((F*)this)[i]; }
		inline const F& operator [](unsigned int i) const { return ((F*)this)[i]; }

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

		inline Vec3<F> Normalized() const
		{
			F invLn = finv(Length());
			return Vec3<F>(x * invLn, y * invLn, z * invLn);
		}

		inline Vec3<F> GetOrthogonal() const
		{
			// Source: CryEngine
			return (F(0.9) * F(0.9)) * (x * x + y * y + z * z) - x * x < 0 ? Vec3<F>(-z, 0, x) : Vec3<F>(0, z, -y);
		}

		inline Vec3<F> Abs() const
		{
			return Vec3f(fabsf(x), fabsf(y), fabsf(z));
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

		inline operator SXYZ() const
		{
			SXYZ r;
			r.x = x; r.y = y; r.z = z;
			return r;
		}
	};

	template<typename F>
	S_API inline F Vec3Dot(const Vec3<F>& v1, const Vec3<F>& v2)
	{
		return v1.Dot(v2);
	}

	template<typename F>
	S_API inline F operator | (const Vec3<F>& v1, const Vec3<F>& v2)
	{
		return v1.Dot(v2);
	}

	template<typename F>
	S_API inline Vec3<F> Vec3Cross(const Vec3<F>& v1, const Vec3<F>& v2)
	{
		return v1.Cross(v2);
	}

	template<typename F>
	S_API inline Vec3<F> operator ^(const Vec3<F>& v1, const Vec3<F>& v2)
	{
		return v1.Cross(v2);
	}

	template<typename F>
	S_API inline Vec3<F> Vec3Normalize(const Vec3<F>& v)
	{
		return v.Normalized();
	}

	template<typename F>
	S_API inline F Vec3Length(const Vec3<F>& v)
	{
		return v.Length();
	}

	template<typename F>
	S_API inline Vec3<F> Vec3Min(const Vec3<F>& a, const Vec3<F>& b)
	{
		return Vec3<F>(
			a.x < b.x ? a.x : b.x,
			a.y < b.y ? a.y : b.y,
			a.z < b.z ? a.z : b.z);
	}

	template<typename F>
	S_API inline Vec3<F> Vec3Max(const Vec3<F>& a, const Vec3<F>& b)
	{
		return Vec3<F>(
			a.x > b.x ? a.x : b.x,
			a.y > b.y ? a.y : b.y,
			a.z > b.z ? a.z : b.z);
	}

	template<typename F> S_API inline Vec3<F> operator *(F f, const Vec3<F>& v)
	{
		return v * f;
	}

	template<typename F>
	inline std::ostream& operator <<(std::ostream& ss, const Vec3<F>& v)
	{
		ss << "(" << v.x << ", " << v.y << ", " << v.z << ")";
		return ss;
	}



	// specific vectors

	typedef struct Vec3<float> S_API Vec3f;
	typedef struct Vec3<float> S_API SVector3;
	typedef struct Vec3<float> S_API float3;
	typedef struct Vec3<double> S_API Vec3d;


	template<typename F>
	static inline void Vec3Mul(const Vec3<F>& a, const Vec3<F>& b, Vec3<F>& r)
	{
		r.x = a.x * b.x;
		r.y = a.y * b.y;
		r.z = a.z * b.z;
	}
}
