// ********************************************************************************************

//	SpeedPoint Color

// ********************************************************************************************

#pragma once
#include "SAPI.h"
#include <Abstract\Vector3.h>

SP_NMSPACE_BEG

// --------------------------------------------------------------------------
// SpeedPoint Color
struct S_API SColor
{
	float r;	// Red value
	float g;	// green value
	float b;	// blue value
	float a;	// Alpha value 0 means fully visible, 1 = totally transparent

	SColor() : r(0), g(0), b(0), a(0) {};
		
	SColor( int rr, int gg, int bb ) 
		: r((float)rr / 255.0f),
		g((float)gg / 255.0f),
		b((float)bb / 255.0f), a(0)
	{
	};

	SColor( int rr, int gg, int bb, int aa )
		: r((float)rr / 255.0f),
		g((float)gg / 255.0f),
		b((float)bb / 255.0f),
		a((float)aa / 255.0f)
	{
	};
		
	SColor( float rr, float gg, float bb )
		: r(rr), g(gg), b(bb), a(0)
	{
	};

	SColor( float rr, float gg, float bb, float aa )
		: r(rr), g(gg), b(bb), a(aa)
	{
	};

	SColor( const SColor& other )
		: r(other.r), g(other.g), b(other.b), a(other.a)
	{
	};

	inline float3 ToFloat3() const
	{
		return float3(r, g, b);
	}
};

// --------------------------------------------------------------------------
// pFloatArr - a 4 element array (r, g, b, a)
static inline S_API void SPGetColorFloatArray(float* pFloatArr, const SColor& color)
{
	SP_ASSERT(pFloatArr);
	pFloatArr[0] = color.r;
	pFloatArr[1] = color.g;
	pFloatArr[2] = color.b;
	pFloatArr[3] = color.a;
}

// --------------------------------------------------------------------------

// Converts the given SColor to an unsigned int coded color
// 0xRRGGBBAA
static inline S_API unsigned int GetColorUINT(const SColor& color)
{
	return (char)(color.r * 255.0f) << 24 |
		(char)(color.g * 255.0f) << 16 |
		(char)(color.b * 255.0f) << 8 |
		(char)(color.a * 255.0f);
}


SP_NMSPACE_END