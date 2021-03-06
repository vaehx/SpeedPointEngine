// ********************************************************************************************

//	This file is part of the SpeedPoint Game Engine

//	(c) 2011-2014 Pascal R. alias iSmokiieZz
//	All rights reserved.

// ********************************************************************************************

#pragma once
#include "SAPI.h"
#include <cstdio> // for assertion
#include <Windows.h>

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
		S_DEBUG,
		S_FULL,
		S_NOTFOUND,
		S_WARN,
		S_WARNING = S_WARN,
		S_INVALIDPARAM,
		S_INVALIDSTAGE,
		S_INVALIDGEOMSTRATEGY,
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


	// We need to forward declare the Failure function as it is used in the SResult class itself
	class S_API SResult;
	static bool Failure(const SResult& r);


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

		bool operator !() const
		{
			return Failure(result);
		}

		operator SResultType() const { return result; }

		// WARNING: out needs to be initialized with a minimum of 16 chars when passed to this function!
		static void GetResultTypeDesc(const SResultType& type, char* out)
		{
			if (!out) return;			
			switch (type)
			{
			case S_INFO: sprintf_s(out, 16, "S_INFO"); break;
			case S_DEBUG: sprintf_s(out, 16, "S_DEBUG"); break;
			case S_ERROR: sprintf_s(out, 16, "S_ERROR"); break;
			case S_INVALIDPARAM: sprintf_s(out, 16, "S_INVALIDPARAM"); break;
			case S_ABORTED: sprintf_s(out, 16, "S_ABORTED"); break;
			case S_NOTIMPLEMENTED: sprintf_s(out, 16, "S_NOTIMPL"); break;
			case S_NOTFOUND: sprintf_s(out, 16, "S_NOTFOUND"); break;
			case S_NOTINIT: sprintf_s(out, 16, "S_NOTINIT"); break;
			case S_INVALIDSTAGE: sprintf_s(out, 16, "S_INVSTAGE"); break;
			}
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
	static bool Success(const HRESULT& r) { return !(r < 0); }

} // namespace SpeedPoint
