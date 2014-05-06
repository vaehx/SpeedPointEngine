// ********************************************************************************************

//	This file is part of the SpeedPoint Game Engine

//	(c) 2011-2014 Pascal R. aka iSmokiieZz
//	All rights reserved.

// ********************************************************************************************

#pragma once

#include <cstdio> // for sprintf_s

#include "SResult.h"	// for ThrowException~ -Functions

#define RET_NONE // use that if you want to return nothing in an assertion



// Assertion macros

// X: Logging handled by proxy
// R: Return value given
// D: Dump sprintf-format and vars given instead of possible message
// -> possible combinations: X, XR, XD, R, RD, D, XRD




/////
//
//
//
// NOTICE: You might want to rewrite these assertions. They are really crappy :/
//
// See below for definitions of "assert" and "assert_trace" macros
//
//
//
////






// Introduce exception proxy
#define SP_ASSERTX(cond, eng, ...) \
	do { \
		if (!(cond) && eng) { \
			char* _pAssertMsg = new char[256]; \
			SpeedPoint::SString::CopyCharS(_pAssertMsg, 256, __VA_ARGS__); \
			SpeedPoint::SResult::ThrowExceptionEng( \
				(SpeedPoint::IExceptionProxy*)eng, \
				SpeedPoint::SResult::eEX_ASSERTION, \
				__FUNCTION__, __LINE__, __FILE__, _pAssertMsg); \
			delete[] _pAssertMsg; \
		} \
	} while (0)


#define SP_ASSERT(cond, ...) \
	do { \
		if (!(cond)) { \
			char* _pAssertMsg = new char[256]; \
			SpeedPoint::SString::CopyCharS(_pAssertMsg, 256, __VA_ARGS__); \
			SpeedPoint::SResult::ThrowExceptionEng( \
				0, \
				SpeedPoint::SResult::eEX_ASSERTION, \
				__FUNCTION__, __LINE__, __FILE__, _pAssertMsg, \
				SpeedPoint::S_ERROR); \
			delete[] _pAssertMsg; \
		} \
	} while (0)




// Introduce Return value
#define SP_ASSERTXR(cond, ret, eng, ...) \
	do { \
		if (!(cond) && eng) { \
			char* _pAssertMsg = new char[256]; \
			SpeedPoint::SString::CopyCharS(_pAssertMsg, 256, __VA_ARGS__); \
			SpeedPoint::SResult::ThrowExceptionEng( \
				(SpeedPoint::IExceptionProxy*)eng, \
				SpeedPoint::SResult::eEX_ASSERTION, \
				__FUNCTION__, __LINE__, __FILE__, _pAssertMsg, \
				SpeedPoint::ValResultType(ret)); \
			delete[] _pAssertMsg; \
			return ret; \
		} \
	} while (0)


#define SP_ASSERTRX(cond, ret, eng, ...) SP_ASSERTXR(cond, ret, eng, __VA_ARGS__)



#define SP_ASSERTR(cond, ret, ...) \
	/*SP_ASSERTR_INTERNAL(cond, 1, ret, __VA_ARGS__) // 1 is just a placeholder, that the args array is not empty*/ \
	do { \
		if (!(cond)) { \
			char* _pAssertMsg = new char[256]; \
			SpeedPoint::SString::CopyCharS(_pAssertMsg, 256, __VA_ARGS__); \
			SpeedPoint::SResult::ThrowExceptionEng( \
				0, \
				SpeedPoint::SResult::eEX_ASSERTION, \
				__FUNCTION__, __LINE__, __FILE__, _pAssertMsg, \
				SpeedPoint::ValResultType(ret)); \
			delete[] _pAssertMsg; \
			return ret; \
		} \
	} while (0)




// Introduce dump
#define SP_ASSERTXRD(cond, ret, eng, format, ...) \
	do { \
		if (!(cond) && eng) { \
			char* _pAssertMsg = new char[256]; \
			sprintf_s(_pAssertMsg, 256, format, __VA_ARGS__); \
			SpeedPoint::SResult::ThrowExceptionDumpEng( \
				(SpeedPoint::IExceptionProxy*)eng, \
				SpeedPoint::SResult::eEX_ASSERTION, \
				__FUNCTION__, __LINE__, __FILE__, _pAssertMsg, \
				SpeedPoint::ValResultType(ret)); \
			delete[] _pAssertMsg; \
			return ret; \
		} \
	} while (0)


#define SP_ASSERTDXR(cond, ret, eng, format, ...) SP_ASSERTXRD(cond, ret, eng, format, __VA_ARGS__)


#define SP_ASSERTRD(cond, ret, format, ...) \
	do { \
		if (!(cond)) { \
			char* _pAssertMsg = new char[256]; \
			sprintf_s(_pAssertMsg, 256, format, __VA_ARGS__); \
			SpeedPoint::SResult::ThrowExceptionDumpEng( \
				0, \
				SpeedPoint::SResult::eEX_ASSERTION, \
				__FUNCTION__, __LINE__, __FILE__, _pAssertMsg, \
				SpeedPoint::ValResultType(ret)); \
			delete[] _pAssertMsg; \
			return ret; \
		} \
	} while (0)



#define SP_ASSERTXD(cond, eng, format, ...) \
	do { \
		if (!(cond) && eng) { \
			char* _pAssertMsg = new char[256]; \
			sprintf_s(_pAssertMsg, 256, format, __VA_ARGS__); \
			SpeedPoint::SResult::ThrowExceptionDumpEng( \
				(SpeedPoint::IExceptionProxy*)eng, \
				SpeedPoint::S_ERROR, \
				SpeedPoint::SResult::eEX_ASSERTION, \
				__FUNCTION__, __LINE__, __FILE__, _pAssertMsg); \
			delete[] _pAssertMsg; \
		} \
	} while (0)


#define SP_ASSERTDX(cond, eng, format, ...) SP_ASSERTXD(cond, eng, format, __VA_ARGS__)



#define SP_ASSERTD(cond, format, ...) \
	do { \
		if (!(cond)) { \
			char* _pAssertMsg = new char[256]; \
			sprintf_s(_pAssertMsg, 256, format, __VA_ARGS__); \
			SpeedPoint::SResult::ThrowExceptionDump( \
				SpeedPoint::SResult::eEX_ASSERTION, \
				__FUNCTION__, __LINE__, __FILE__, _pAssertMsg); \
			delete[] _pAssertMsg; \
		} \
	} while (0)






#undef assert	// make sure no header already defined assert / included vc's assert.h
#define assert SP_ASSERT



#undef assert_trace
#define assert_trace SP_ASSERTD