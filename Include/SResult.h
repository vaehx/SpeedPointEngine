// ********************************************************************************************

//	SpeedPoint Result

// ********************************************************************************************

#pragma once

#include "SAPI.h"

namespace SpeedPoint
{
	// SpeedPoint Result enumeration
	enum S_API SResult
	{
		S_ERROR = 0,
		S_SUCCESS = 1,		
		S_ABORTED = 2,
		S_INFO = 3,
		S_FULL = 4,
		S_NOTFOUND = 5
	};

	static bool Success( const SResult& r ) { return (r == S_SUCCESS || r == S_INFO); }
	static bool Failure( const SResult& r ) { return !(r == S_SUCCESS || r == S_INFO); }
	static bool Aborted( const SResult& r ) { return r == S_ABORTED; }
	static bool FullPool( const SResult& r ) { return r == S_FULL; }
	static bool NotFound( const SResult& r ) { return r == S_NOTFOUND; }
}