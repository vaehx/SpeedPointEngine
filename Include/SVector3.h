// ********************************************************************************************

//	SpeedPoint 3 Dimensional Vector

// ********************************************************************************************

#pragma once
#include "SAPI.h"
#include <math.h>

namespace SpeedPoint
{

	// sometimes we need a simple 3-Dimensional structure for the components in a unit
	// The SVector3 structure has a custom default constructor, that's why it does not work with units
	struct S_API SXYZ
	{
		float x, y, z;
	};

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
		
		SVector3& operator *= ( const SVector3& v ) { this->x *= v.x; this->y *= v.y; this->z *= v.z; return *this; }
		
		SVector3& operator /= ( const SVector3& v ) { this->x /= v.x; this->y /= v.y; this->z /= v.z; return *this; }		
		
		SVector3 operator - () { return SVector3( -x, -y, -z ); }
		
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
		SVector3& Cross ( const SVector3& o )
		{			
			this->x = y * o.z - z * o.y;
			this->y = z * o.x - x * o.z;
			this->z = x * o.y - y * o.x;
			return *this;		
		}
		
		// Dot product with another vector
		float Dot ( const SVector3& o )
		{		
			return x * o.x + y * o.y + z * o.z;		
		}
		
		// Length of this vector
		float Length ()
		{
			return sqrtf( x*x + y*y + z*z );	
		}
		
		// Square of the length
		float Square ()
		{
			return x*x + y*y + z*z;				
		}
		
		// Get unit vector of this vector
		SVector3& Normalize( float* pLength = 0 )
		{
			float l = this->Length();			
			this->x /= l; this->y /= l; this->z /= l;
			
			if( pLength != 0 ) *pLength = l;
			return *this;								
		}			
	};

	typedef struct S_API SVector3 float3;
	
	inline bool operator == ( const SVector3& v1, const SVector3& v2 ) { return ( v1.x == v2.x && v1.y == v2.y && v1.z == v2.z ); }
	inline bool operator != ( const SVector3& v1, const SVector3& v2 ) { return ( v1.x != v2.x || v1.y != v2.y || v1.z != v2.z ); }
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
	inline SVector3 SVector3Cross( const SVector3& va, const SVector3& vb )
	{	
		SVector3 a(va);
		return a.Cross(vb);
	}
	
	inline float SVector3Dot( const SVector3& va, const SVector3& vb )
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
		return a.Normalize();
	}
/////////////	
}

#define S_DEFAULT_VEC3 SpeedPoint::SVector3()
#define SPoint3 SpeedPoint::SVector3