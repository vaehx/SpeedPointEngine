#pragma once

#include <Util\SAssert.h>
#include <Util\SResult.h>
#include <sstream>
#include <Windows.h>
#include "CLog.h"

using SpeedPoint::CLog;
using namespace std;

void SPAssertTrace(const char* condition, const char* file, const char* func, unsigned int line, const char* message)
{
	stringstream outstream;
	outstream << "Assertion failed!" << endl;
	outstream << "Condition: " << condition << endl;
	outstream << "File: " << file << endl;
	outstream << "Line: " << line << endl;
	outstream << "Function: " << func << endl;
	outstream << "Message: " << message << endl;

	string out = outstream.str();

	CLog::Log(SpeedPoint::S_ERROR, out.c_str());

	MessageBoxA(nullptr, out.c_str(), "Assertion failed", MB_ICONERROR | MB_OK);
}
