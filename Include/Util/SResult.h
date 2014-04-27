// ********************************************************************************************

//	This file is part of the SpeedPoint Game Engine

//	(c) 2011-2014 Pascal R. alias iSmokiieZz
//	All rights reserved.

// ********************************************************************************************

#pragma once
#include "SAPI.h"
#include "SWindowsSpecific.h"
#include <cstdio> // for assertion

namespace SpeedPoint
{
	// Old Result enumeration
	/*
	// SpeedPoint Result enumeration
	enum S_API SResult
	{
		S_ERROR = 0
		, S_SUCCESS = 1
		, S_ABORTED = 2
		, S_INFO = 3
		, S_FULL = 4
		, S_NOTFOUND = 5
		, S_WARN = 6
		, S_INVALIDPARAM = 7
		, S_NOTINIT = 8
	};
	*/

	// Result type for a Result
	enum S_API SResultType
	{
		S_ERROR = 0,
		S_SUCCESS,
		S_ABORTED,
		S_INFO,
		S_FULL,
		S_NOTFOUND,
		S_WARN,
		S_WARNING = S_WARN,
		S_INVALIDPARAM,
		S_INVALIDSTAGE,
		S_NOTIMPLEMENTED,
		S_NOTINIT,
		S_NOTINITED = S_NOTINIT
	};

	// Required conversion functions from other types to ResultType
	
	inline SResultType ValResultType(bool bVal)
	{
		return (bVal) ? S_SUCCESS : S_ERROR;
	}

	inline SResultType ValResultType(const SResultType& rt)
	{
		return rt;
	}

	inline SResultType ValResultType() // especially required for Assertion macros
	{
		return S_ERROR;
	}


	// Interface to handle ThrowExceptions, although SSpeedPointEngine is not included yet
	class S_API IExceptionProxy
	{
	public:
		virtual void HandleException(char* msg) = 0;
	};

	// New SpeedPoint Result class
	class S_API SResult
	{
	public:
		enum SExceptionType
		{
			eEX_ASSERTION
		};
		
		SResultType result;

		SResult()
			: result(S_SUCCESS)
		{
		}

		// constructor with type
		SResult(const SResultType& type)
			: result(type)
		{
		}

		bool operator == (SResultType type) const
		{
			return result == type;
		}

		bool operator != (const SResultType& type) const
		{
			return result != type;
		}

		bool operator != (const SResult& res) const
		{
			return res.result != result;
		}

		// throw an exception with given parameters and output information useful for debugging
		static void ThrowExceptionAssertion(const char* function,
			int line,
			const char* file,
			const char* msg)
		{
			return ThrowExceptionEng(0, SResult::eEX_ASSERTION, function, line, file, msg);
		}

		// throw an exception with given parameters and output information useful for debugging
		static void ThrowException(const SResult::SExceptionType& type,
			const char* function,
			int line,
			const char* file,
			const char* msg)
		{
			return ThrowExceptionEng(0, type, function, line, file, msg);
		}		

		// throw an exception with given parameters and output information useful for debugging
		static void ThrowExceptionEng(IExceptionProxy* pExProxy,
			const SResult::SExceptionType& type,
			const char* function,
			int line,
			const char* file,
			const char* msg,
			const SResultType resType = S_ERROR)
		{
			char* pOutput = new char[500];

			sprintf_s(pOutput, 500, "Assertion failed!\n" \
				"  File: %s\n" \
				"  Function: %s\n" \
				"  Line: %d\n" \
				"  Message: %s\n" \
				"  Return Value: %d\n",
				file, function, line, msg, (unsigned int)resType);

			if (pExProxy) pExProxy->HandleException(pOutput);

			printf(pOutput);
		}

		// Throw an exception message with dump instead of msg
		static void ThrowExceptionDump(const SResult::SExceptionType& type,
			const char* function,
			int line,
			const char* file,
			const char* dump)
		{
			return ThrowExceptionDumpEng(0, type, function, line, file, dump, S_ERROR);
		}

		static void ThrowExceptionDumpEng(IExceptionProxy* pExProxy,
			const SResult::SExceptionType& type,
			const char* function,
			int line,
			const char* file,
			const char* dump)
		{
			return ThrowExceptionDumpEng(pExProxy, type, function, line, file, dump, S_ERROR);
		}

		// Throw an exception with dump instead of msg and put entry into log if proxy given
		static void ThrowExceptionDumpEng(IExceptionProxy* pExProxy,			
			const SResult::SExceptionType& type,
			const char* function,
			int line,
			const char* file,
			const char* dump,
			const SResultType resType)
		{
			char* pOutput = new char[500];

			sprintf_s(pOutput, 500, "Assertion failed!\n" \
				"  File: %s\n" \
				"  Function: %s\n" \
				"  Line: %d\n" \
				"  Dump: %s\n" \
				"  Return Value: %d\n",
				file, function, line, dump, (unsigned int)resType);

			if (pExProxy) pExProxy->HandleException(pOutput);

			printf(pOutput);
		}
	};

	static bool Success(const SResult& r) { return (r == S_SUCCESS || r == S_INFO || r == S_WARN); }
	static bool Failure(const SResult& r) { return !(r == S_SUCCESS || r == S_INFO || r == S_WARN); }
	static bool Aborted(const SResult& r) { return r == S_ABORTED; }
	static bool FullPool(const SResult& r) { return r == S_FULL; }
	static bool NotFound(const SResult& r) { return r == S_NOTFOUND; }
	static bool NotInitialized(const SResult& r) { return r == S_NOTINIT; }

	// Support HRESULT

	static bool Failure(const HRESULT& r) { return r < 0; }

} // namespace SpeedPoint
