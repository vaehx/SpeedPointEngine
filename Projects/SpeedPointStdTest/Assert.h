// This is a file of the SpeedPoint Standard Func Test Project

#pragma once

#include <cstdio> // we need the standard io stuff

namespace SpeedPoint
{
	//////////////////////////////////////////////////////////////////////////

	void ThrowException(char* file, char* func, int line, char* msg)		
	{
		printf("Assertion failed!\n");
		printf("  func: %s\n", func);
		printf("  line: %d\n", line);
		printf("  msg: %s\n", msg);
	}

	//////////////////////////////////////////////////////////////////////////

	void ThrowExceptionDump(char* file, char* func, int line, char* dump)
	{
		printf("Assertion failed!\n");
		//printf("  file: %s\n", file);
		printf("  func: %s\n", func);
		printf("  line: %d\n", line);
		printf("  dump: %s\n", dump);
	}

}

//////////////////////////////////////////////////////////////////////////

// Extremely cool assertion macros

#define RET_NONE // use that if you want to return nothing in an assertion

#define TEST_ASSERT(cond, ...) \
	do { \
	if((cond)) { \
	char* cExMsg = new char[256]; \
	SpeedPoint::String::CopyCharS(cExMsg, 256, __VA_ARGS__); \
	SpeedPoint::ThrowException(__FILE__, __FUNCDNAME__, __LINE__, cExMsg); \
	delete[] cExMsg; \
	} \
	} while (0)

#define TEST_ASSERTR(cond, ret, ...) \
	do { \
	if((cond)) { \
	char* cExMsg = new char[256]; \
	SpeedPoint::String::CopyCharS(cExMsg, 256, __VA_ARGS__); \
	SpeedPoint::ThrowException(__FILE__, __FUNCDNAME__, __LINE__, cExMsg); \
	delete[] cExMsg; \
	return ret; \
	} \
	} while (0)

#define TEST_ASSERTD(cond, format, ...) \
	do { \
	if ((cond)) { \
	char* cExMsg = new char[256]; \
	sprintf_s(cExMsg, 256, format, __VA_ARGS__); \
	SpeedPoint::ThrowExceptionDump(__FILE__, __FUNCDNAME__, __LINE__, cExMsg); \
	delete[] cExMsg; \
	} \
	} while (0)

#define TEST_ASSERTDR(cond, ret, format, ...) \
	do { \
	if ((cond)) { \
	char* cExMsg = new char[256]; \
	sprintf_s(cExMsg, 256, format, __VA_ARGS__); \
	SpeedPoint::ThrowExceptionDump(__FILE__, __FUNCDNAME__, __LINE__, cExMsg); \
	delete[] cExMsg; \
	return ret; \
	} \
	} while (0)