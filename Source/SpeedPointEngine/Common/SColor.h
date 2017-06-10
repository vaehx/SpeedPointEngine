// ********************************************************************************************

//	SpeedPoint Color

// ********************************************************************************************

#pragma once
#include "SAPI.h"
#include "Vector3.h"

namespace SpeedPoint
{

	// --------------------------------------------------------------------------
	// SpeedPoint Color
	struct S_API SColor
	{
		float r;	// Red value
		float g;	// green value
		float b;	// blue value
		float a;	// Alpha value 0 means fully visible, 1 = totally transparent

		SColor() : r(0), g(0), b(0), a(0) {};

		SColor(int rr, int gg, int bb)
			: r((float)rr / 255.0f),
			g((float)gg / 255.0f),
			b((float)bb / 255.0f), a(0)
		{
		};

		SColor(int rr, int gg, int bb, int aa)
			: r((float)rr / 255.0f),
			g((float)gg / 255.0f),
			b((float)bb / 255.0f),
			a((float)aa / 255.0f)
		{
		};

		SColor(float rr, float gg, float bb)
			: r(rr), g(gg), b(bb), a(0)
		{
		};

		SColor(float rr, float gg, float bb, float aa)
			: r(rr), g(gg), b(bb), a(aa)
		{
		};

		SColor(const SColor& other)
			: r(other.r), g(other.g), b(other.b), a(other.a)
		{
		};

		// r = vec.x, g = vec.y, b = vec.z, a = 0
		SColor(const float3& vec)
			: r(vec.x), g(vec.y), b(vec.z), a(0)
		{
		};

		inline float3 ToFloat3() const
		{
			return float3(r, g, b);
		}

		// Returns color in 0xAARRGGBB format
		inline unsigned int ToInt_ARGB() const
		{
			return (int)(a * 255.999f) << 24 |
				(int)(r * 255.999f) << 16 |
				(int)(g * 255.999f) << 8 |
				(int)(b * 255.999f);
		}

		inline unsigned int ToInt_RGBA() const
		{
			return (int)(r * 255.999f) << 24 |
				(int)(g * 255.999f) << 16 |
				(int)(b * 255.999f) << 8 |
				(int)(a * 255.999f);
		}

		static SColor Black()	{ return SColor(0.f, 0.f, 0.f); }
		static SColor White()	{ return SColor(1.f, 1.f, 1.f); }
		static SColor Red()		{ return SColor(1.f, 0.f, 0.f); }
		static SColor Green()	{ return SColor(0.f, 1.f, 0.f); }
		static SColor Blue()	{ return SColor(0.f, 0.f, 1.f); }
		static SColor Yellow()	{ return SColor(1.f, 1.f, 0.f); }
		static SColor Purple()	{ return SColor(1.f, 0.f, 1.f); }
		static SColor Turqouise(){ return SColor(0.f, 1.f, 1.f); }
	};

	// --------------------------------------------------------------------------
	// pFloatArr - a 4 element array (r, g, b, a)
	static inline S_API void SPGetColorFloatArray(float* pFloatArr, const SColor& color)
	{
		if (pFloatArr)
		{
			pFloatArr[0] = color.r;
			pFloatArr[1] = color.g;
			pFloatArr[2] = color.b;
			pFloatArr[3] = color.a;
		}
	}

	// --------------------------------------------------------------------------
	static inline S_API void SPGetColorFloatArray(float* pFloatArr, unsigned int intColorARGB)
	{
		if (pFloatArr)
		{
			pFloatArr[0] = (float)((intColorARGB & 0x00ff0000) >> 16) / 255.0f;
			pFloatArr[1] = (float)((intColorARGB & 0x0000ff00) >> 8) / 255.0f;
			pFloatArr[2] = (float)((intColorARGB & 0x000000ff)) / 255.0f;
			pFloatArr[3] = (float)((intColorARGB & 0xff000000) >> 24) / 255.0f;
		}
	}

	static inline S_API void SPGetColorFloat3(float3* pVec, unsigned int intColorARGB)
	{
		if (pVec)
		{
			pVec->x = (float)((intColorARGB & 0x00ff0000) >> 16) / 255.0f;
			pVec->y = (float)((intColorARGB & 0x0000ff00) >> 8) / 255.0f;
			pVec->z = (float)((intColorARGB & 0x000000ff)) / 255.0f;
		}
	}


}