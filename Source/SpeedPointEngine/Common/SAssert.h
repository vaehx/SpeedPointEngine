// ********************************************************************************************

//	This file is part of the SpeedPoint Game Engine

//	(c) 2011-2014 Pascal R. aka iSmokiieZz
//	All rights reserved.

// ********************************************************************************************

#pragma once

#include <iostream>

#include "SResult.h"	// for ThrowException~ -Functions

// also check against wrong debug values if in debug mode
#ifdef _DEBUG
#define IS_VALID_PTR(ptr) (ptr && (unsigned int)ptr != 0xC0000005 && (unsigned int)ptr != 0xCDCDCDCD && (unsigned int)ptr != 0xCCCCCCCC && (unsigned int)ptr != 0xFEEEFEEE)
#else
#define IS_VALID_PTR(ptr) (ptr)
#endif

///////////////////////////////////////////////////////////////////////////////////////////////////

void SPAssertLog(SpeedPoint::SResult result, const char* condition, const char* file, const char* func, unsigned int line, const char* message, ...);
static void SPAssertLog(SpeedPoint::SResult result, const char* condition, const char* file, const char* func, unsigned int line, ...)
{
	return SPAssertLog(result, condition, file, func, line, "");
}

// Logs an assertion and returns given return value
#define SP_ASSERTR(cond, ret, ...) if (!(cond)) { SPAssertLog(SpeedPoint::S_ERROR, #cond, __FILE__, __FUNCTION__, __LINE__, __VA_ARGS__); return ret; }

///////////////////////////////////////////////////////////////////////////////////////////////////

#define DEBUG_BREAK _asm { int 3 }

void SPAssertTrace(const char* condition, const char* file, const char* func, unsigned int line, const char* message, ...);
static void SPAssertTrace(const char* condition, const char* file, const char* func, unsigned int line, ...)
{
	SPAssertTrace(condition, file, func, line, "");
}

#undef assert_trace
#define assert_trace(cond, format, ...) \
	do { \
		if (!(cond)) { \
			char* pAssertMsg = new char[256]; \
			sprintf_s(pAssertMsg, 256, format, __VA_ARGS__); \
			SPAssertTrace(#cond, __FILE__, __FUNCTION__, __LINE__, pAssertMsg); \
			delete[] pAssertMsg; \
			DEBUG_BREAK; \
		} \
	} while (0)

#undef assert	// make sure no header already defined assert / included vc's assert.h
#define assert(cond, ...) \
	do { \
		if (!(cond)) { \
			SPAssertTrace(#cond, __FILE__, __FUNCTION__, __LINE__, __VA_ARGS__); \
			DEBUG_BREAK; \
		} \
	} while (0)
