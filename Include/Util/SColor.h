// ********************************************************************************************

//	SpeedPoint Color

// ********************************************************************************************

#pragma once
#include "SAPI.h"

namespace SpeedPoint
{
	
	// SpeedPoint Color
	struct S_API SColor
	{
		float r;	// Red value
		float g;	// green value
		float b;	// blue value
		float a;	// Alpha value 0 means fully visible, 1 = totally transparent

		SColor() : r(0), g(0), b(0), a(0) {};
		
		SColor( int rr, int gg, int bb ) 
			: r((float)rr / 255.0f), g((float)gg / 255.0f), b((float)bb / 255.0f), a(0) {};

		SColor( int rr, int gg, int bb, int aa )
			: r((float)rr / 255.0f), g((float)gg / 255.0f), b((float)bb / 255.0f), a((float)aa / 255.0f) {};
		
		SColor( float rr, float gg, float bb ) : r(rr), g(gg), b(bb), a(0) {};

		SColor( float rr, float gg, float bb, float aa ) : r(rr), g(gg), b(bb), a(aa) {};

		SColor( const SColor& other ) : r(other.r), g(other.g), b(other.b), a(other.a) {};
	};

}