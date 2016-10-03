// Common Math functions

#pragma once
#include <SPrerequisites.h>

SP_NMSPACE_BEG


// ----------------------------------------------------------------------

ILINE bool float_equal(float f1, float f2)
{
	return f1 > (f2 - FLT_EPSILON) && f1 < (f2 + FLT_EPSILON);
}
ILINE bool float_equal(double d1, double d2)
{
	return d1 >(d2 - DBL_EPSILON) && d1 < (d2 + DBL_EPSILON);
}


ILINE float float_abs(float f) { return fabsf(f); }
ILINE double float_abs(double d) { return fabs(d); }


// ----------------------------------------------------------------------

template<typename F>
ILINE F lerp(F a, F b, F k)
{
	return a + k * (b - a);
}



SP_NMSPACE_END