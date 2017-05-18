//////////////////////////////////////////////////////////////////////////////////
//
// This file is part of the SpeedPointEngine
// Copyright (c) 2011-2015, Pascal Rosenkranz
// ------------------------------------------------------------------------------
// Filename:	Quaternion.h
// Created:	23.9.2015 by Pascal Rosenkranz
// Description:
// -------------------------------------------------------------------------------
// History:
//
//////////////////////////////////////////////////////////////////////////////////

#pragma once

#include "Vector3.h"
#include "Mat33.h"
#include "Mat44.h"
#include "SPrerequisites.h"

#include <cmath>

SP_NMSPACE_BEG

struct S_API Quat
{
	Vec3f v;
	float w;

	Quat() : w(1.0f) {} // Identity = (0, 0, 0, w=1.0)

	Quat(const Quat& q) : v(q.v), w(q.w) {}
	Quat(float ww, const Vec3f& vv) : v(vv), w(ww) {}
	
	// w = q0, v = (q1, q2, q3)
	Quat(float q0, float q1, float q2, float q3) : v(q1, q2, q3), w(q0) {}

	/*
	template<typename F> void Quat_tpl<F>::SetRotationV0V1(const Vec3_tpl<F>& v0, const Vec3_tpl<F>& v1)
	{
		assert(v0.IsUnit(0.01f));
		assert(v1.IsUnit(0.01f));
		real dot = v0.x*v1.x + v0.y*v1.y + v0.z*v1.z + 1.0;
		if (dot > real(0.0001f))
		{
			real vx = v0.y*v1.z - v0.z*v1.y;
			real vy = v0.z*v1.x - v0.x*v1.z;
			real vz = v0.x*v1.y - v0.y*v1.x;
			real d = isqrt_tpl(dot*dot + vx*vx + vy*vy + vz*vz);
			w = F(dot*d);	v.x = F(vx*d); v.y = F(vy*d); v.z = F(vz*d);
			return;
		}
		w = 0; v = v0.GetOrthogonal().GetNormalized();
	*/

	// Rotation from v1 to v2
	static Quat FromVectors(const Vec3f& v1, const Vec3f& v2)
	{		
		float dot = Vec3Dot(v1, v2) + 1.0f;
		if (dot > 0.0001f)
		{					
			Quat q;
			q.v = Vec3Cross(v1, v2);
			float d = sqrtf(dot * dot + Vec3Dot(q.v, q.v));
			q.v *= d;
			q.w = dot * d;
			return q;
		}
		
		Vec3f vn = v1.Normalized();
		return Quat(0, Vec3Cross(Vec3f(vn.y, vn.z, vn.x), vn));
	}

	// Rotation about X-Axis
	static Quat FromRotationX(float rad)
	{
		float ha = rad * 0.5f;
		return Quat(cosf(ha), Vec3f(sinf(ha), 0, 0));
	}

	// Rotation about Y-Axis
	static Quat FromRotationY(float rad)
	{
		float ha = rad * 0.5f;
		return Quat(cosf(ha), Vec3f(0, sinf(ha), 0));
	}

	// Rotation about Z-Axis
	static Quat FromRotationZ(float rad)
	{
		float ha = rad * 0.5f;
		return Quat(cosf(ha), Vec3f(0, 0, sinf(ha)));
	}

	static Quat FromEuler(const Vec3f& rad)
	{
		Quat qu1 = Quat::FromAxisAngle(Vec3f(1.0f, 0, 0), rad.x);
		Quat qu2 = Quat::FromAxisAngle(Vec3f(0, 1.0f, 0), rad.y);
		Quat qu3 = Quat::FromAxisAngle(Vec3f(0, 0, 1.0f), rad.z);
		return qu1 * qu2 * qu3;
		/*
		float sx = sinf(rad.x * 0.5f), cx = cosf(rad.x * 0.5f);
		float sy = sinf(rad.y * 0.5f), cy = cosf(rad.y * 0.5f);
		float sz = sinf(rad.z * 0.5f), cz = cosf(rad.z * 0.5f);
		Quat q = Quat(cx * cy * cz + sx * sy * sz,
			sx * cy * cz - cx * sy * sz,
			cx * sy * cz + sx * cy * sz,
			cx * cy * sz - sx * sy * cz
			).Normalized();
		return q;
		*/		
	}

	// axis must be normalized direction vector
	static Quat FromAxisAngle(const Vec3f& axis, float rad)
	{
		float ha = rad * 0.5f;
		return Quat(cosf(ha), axis * sinf(ha));
	}

	static Quat FromRotationMatrix(const Mat44& m)
	{
		Quat q;
		q.w = sqrtf(max(0, 1.0f + m._11 + m._22 + m._33)) * 0.5f;
		q.v.x = sqrtf(max(0, 1.0f + m._11 - m._22 - m._33)) * 0.5f;
		q.v.y = sqrtf(max(0, 1.0f - m._11 + m._22 - m._33)) * 0.5f;
		q.v.z = sqrtf(max(0, 1.0f - m._11 - m._22 + m._33)) * 0.5f;
		
		q.v.x = _copysignf(q.v.x, m._32 - m._23);
		q.v.y = _copysignf(q.v.y, m._13 - m._31);
		q.v.z = _copysignf(q.v.z, m._21 - m._12);
		return q;
	}


	inline float Length() const;
	inline Quat Normalized() const;
	inline Quat Inverted() const;
	inline Quat operator !() const;

	inline Quat operator *(const Quat& r) const;

	inline Quat operator *(float f) const
	{
		return Quat(w * f, v.x * f, v.y * f, v.z * f);
	}

	inline Mat33 ToRotationMatrix33() const
	{
		Quat n = Normalized();
		const float &a = n.w, &b = n.v.x, &c = n.v.y, &d = n.v.z;
		return Mat33(
			a*a + b*b - c*c - d*d, 2.f*(b*c - a*d), 2.f*(b*d + a*c),
			2.f*(b*c + a*d), a*a - b*b + c*c - d*d, 2.f*(c*d - a*b),
			2.f*(b*d - a*c), 2.f*(c*d + a*b), a*a - b*b - c*c + d*d
		);
	}

	inline Mat44 ToRotationMatrix() const
	{
		return Mat44(ToRotationMatrix33());
	}

	inline Vec3f GetForward() const
	{
		Quat n = Normalized();
		const float &a = n.w, &b = n.v.x, &c = n.v.y, &d = n.v.z;
		return Vec3f(
			2.f*(b*d + a*c),
			2.f*(c*d - a*b),
			a*a - b*b - c*c + d*d
			);
	}
};

inline float Quat::Length() const
{
	return sqrtf(w*w + v.x*v.x + v.y*v.y + v.z*v.z);
}

inline Quat Quat::Normalized() const
{
	float invln = 1.0f / sqrtf(w*w + v.x*v.x + v.y*v.y + v.z*v.z);
	return Quat(w * invln, v.x * invln, v.y * invln, v.z * invln);
}

inline Quat Quat::operator !() const
{
	return Quat(w, -v.x, -v.y, -v.z);
}
inline Quat Quat::Inverted() const
{
	return !(*this);
}

// Hamilton product
inline Quat Quat::operator *(const Quat& r) const
{	
	const float &a1 = w, &b1 = v.x, &c1 = v.y, &d1 = v.z;
	const float &a2 = r.w, &b2 = r.v.x, &c2 = r.v.y, &d2 = r.v.z;
	return Quat(a1*a2 - b1*b2 - c1*c2 - d1*d2,
		a1*b2 + b1*a2 + c1*d2 - d1*c2,
		a1*c2 - b1*d2 + c1*a2 + d1*b2,
		a1*d2 + b1*c2 - c1*b2 + d1*a2
		);
}

inline Vec3f operator *(const Quat& q, const Vec3f& p)
{
	// TODO: multiply this out to save flops

	Quat p2 = q * Quat(0, p) * q.Inverted();
	return p2.v;
}

// Uses quaternions to calculate the rotation of the given vector
inline Vec3f RotateVector(const Vec3f& vec, const Vec3f& axis, float rad)
{
	return Quat::FromAxisAngle(axis, rad) * vec;
}


// Build TRS: Scale -> Rotation -> Translation
inline static void MakeTransformationTRS(const Vec3f& translation, const Quat& rotation, const Vec3f& scale, Mat44* pMat)
{
	MakeTransformationTRS(translation, rotation.ToRotationMatrix(), scale, pMat);
}


inline std::ostream& operator <<(std::ostream& ss, const Quat& q)
{
	ss << "(" << q.v.x << ", " << q.v.y << ", " << q.v.z << " w=" << q.w << ")";
	return ss;
};

SP_NMSPACE_END
