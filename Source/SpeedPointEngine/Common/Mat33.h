#pragma once
#include "SAPI.h"
#include "Vector3.h"

namespace SpeedPoint
{
	struct S_API Mat33
	{
		union
		{
			struct
			{
				float _11, _12, _13;
				float _21, _22, _23;
				float _31, _32, _33;
			};
			float m[3][3];
		};

		static const Mat33 Identity;

		// Identity
		Mat33() :
			_11(1.0f), _12(0), _13(0),
			_21(0), _22(1.0f), _23(0),
			_31(0), _32(0), _33(1.0f) {}

		Mat33(const Mat33& m) :
			_11(m._11), _12(m._12), _13(m._13),
			_21(m._21), _22(m._22), _23(m._23),
			_31(m._31), _32(m._32), _33(m._33) {}

		Mat33(float f) :
			_11(f), _12(f), _13(f),
			_21(f), _22(f), _23(f),
			_31(f), _32(f), _33(f) {}

		Mat33(float m11, float m12, float m13,
			float m21, float m22, float m23,
			float m31, float m32, float m33)
			: _11(m11), _12(m12), _13(m13),
			_21(m21), _22(m22), _23(m23),
			_31(m31), _32(m32), _33(m33) {}

		inline static Mat33 FromColumns(const Vec3f& c1, const Vec3f& c2, const Vec3f& c3)
		{
			return Mat33(
				c1.x, c2.x, c3.x,
				c1.y, c2.y, c3.y,
				c1.z, c2.z, c3.z
			);
		}

		inline static Mat33 FromRows(const Vec3f& c1, const Vec3f& c2, const Vec3f& c3)
		{
			return Mat33(
				c1.x, c1.y, c1.z,
				c2.x, c2.y, c2.z,
				c3.x, c3.y, c3.z
			);
		}

		Mat33 operator + (const Mat33& m) const
		{
			return Mat33(
				_11 + m._11, _12 + m._12, _13 + m._13,
				_21 + m._21, _22 + m._22, _23 + m._23,
				_31 + m._31, _32 + m._32, _33 + m._33
			);
		}

		Mat33& operator += (const Mat33& m)
		{
			_11 += m._11; _12 += m._12; _13 += m._13;
			_21 += m._21; _22 += m._22; _23 += m._23;
			_31 += m._31; _32 += m._32; _33 += m._33;
			return *this;
		}

		Mat33 operator - (const Mat33& m) const
		{
			return Mat33(
				_11 - m._11, _12 - m._12, _13 - m._13,
				_21 - m._21, _22 - m._22, _23 - m._23,
				_31 - m._31, _32 - m._32, _33 - m._33
			);
		}

		Mat33& operator -= (const Mat33& m)
		{
			_11 -= m._11; _12 -= m._12; _13 -= m._13;
			_21 -= m._21; _22 -= m._22; _23 -= m._23;
			_31 -= m._31; _32 -= m._32; _33 -= m._33;
			return *this;
		}

		Mat33 operator * (float f) const
		{
			return Mat33(
				_11 * f, _12 * f, _13 * f,
				_21 * f, _22 * f, _23 * f,
				_31 * f, _32 * f, _33 * f
			);
		}

		Vec3f operator * (const Vec3f& v) const
		{
			return Vec3f(
				_11 * v.x + _12 * v.y + _13 * v.z,
				_21 * v.x + _22 * v.y + _23 * v.z,
				_31 * v.x + _32 * v.y + _33 * v.z
			);
		}

		Mat33 operator * (const Mat33& m) const
		{
			Mat33 out;
			out._11 = _11 * m._11 + _12 * m._21 + _13 * m._31;
			out._12 = _11 * m._12 + _12 * m._22 + _13 * m._32;
			out._13 = _11 * m._13 + _12 * m._23 + _13 * m._33;

			out._21 = _21 * m._11 + _22 * m._21 + _23 * m._31;
			out._22 = _21 * m._12 + _22 * m._22 + _23 * m._32;
			out._23 = _21 * m._13 + _22 * m._23 + _23 * m._33;

			out._31 = _31 * m._11 + _32 * m._21 + _33 * m._31;
			out._32 = _31 * m._12 + _32 * m._22 + _33 * m._32;
			out._33 = _31 * m._13 + _32 * m._23 + _33 * m._33;
			return out;
		}

		Mat33 Transposed() const
		{
			return Mat33(
				_11, _21, _31,
				_12, _22, _32,
				_13, _23, _33
			);
		}

		Mat33 Inverted() const
		{
			Mat33 C;
			C._11  = _22 * _33 - _23 * _32;
			C._21 -= _21 * _33 - _23 * _31;
			C._31  = _21 * _32 - _22 * _31;
			C._12 -= _12 * _33 - _13 * _32;
			C._22  = _11 * _33 - _13 * _31;
			C._32 -= _11 * _32 - _12 * _31;
			C._13  = _12 * _23 - _13 * _22;
			C._23 -= _11 * _23 - _13 * _21;
			C._33  = _11 * _22 - _12 * _21;
			
			float invdet = 1.0f / (_11 * _22 * _33 + _12 * _23 * _31 + _13 * _21 * _32 - _13 * _22 * _31 - _12 * _21 * _33 - _11 * _23 * _32);
			C._11 *= invdet;
			C._12 *= invdet;
			C._13 *= invdet;
			C._21 *= invdet;
			C._22 *= invdet;
			C._23 *= invdet;
			C._31 *= invdet;
			C._32 *= invdet;
			C._33 *= invdet;

			return C;
		}

		float Determinant() const
		{
			return _11 * _22 * _33 + _12 * _23 * _31 + _13 * _21 * _32 - _13 * _22 * _31 - _12 * _21 * _33 - _11 * _23 * _32;
		}

		float Trace() const
		{
			return _11 + _22 + _33;
		}
	};

	inline Mat33 operator *(float f, const Mat33& m)
	{
		return m * f;
	}

	// a * b^T
	static inline Mat33 Vec3MulT(const Vec3f& a, const Vec3f& b)
	{
		return Mat33(
			a.x * b.x, a.y * b.x, a.z * b.x,
			a.x * b.y, a.y * b.y, a.z * b.y,
			a.x * b.z, a.y * b.z, a.z * b.z
		);
	}
}
