// ******************************************************************************************

//	SpeedPoint API definition file

// ******************************************************************************************

#pragma once


#ifdef SP_UNITTEST

#define S_API

#else

#ifdef SP_EXPORT

#define S_API __declspec(dllexport)

#else

#define S_API __declspec(dllimport)

#endif

#endif