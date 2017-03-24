#pragma once

#include "SAssert.h"
#include "SResult.h"
#include "CLog.h"
#include <sstream>
#include <Windows.h>

using SpeedPoint::CLog;
using SpeedPoint::SResult;
using namespace std;

inline void AssembleAssertionMessage(string& msg, const char* condition, const char* file, const char* func, unsigned int line, const char* message)
{
	// += operator is faster than stringstream
	msg += "Assertion failed!";
	
	msg += "\n  Condition: ";
	msg += condition;
	
	msg += "\n  File: ";
	msg += file;
	msg += " @L";
	msg += to_string(line);
	
	msg += "\n Function: ";
	msg += func;
	
	msg += "\n Message: ";
	msg += message;

	msg += "\n";
}

void SPAssertLog(SResult result, const char* condition, const char* file, const char* func, unsigned int line, const char* message, ...)
{
	string out;
	AssembleAssertionMessage(out, condition, file, func, line, message);

	CLog::Log(result, out);
}

void SPAssertTrace(const char* condition, const char* file, const char* func, unsigned int line, const char* message, ...)
{
	string out;
	AssembleAssertionMessage(out, condition, file, func, line, message);

	CLog::Log(SpeedPoint::S_ERROR, out);

	MessageBoxA(nullptr, out.c_str(), "Assertion failed", MB_ICONERROR | MB_OK);
}
