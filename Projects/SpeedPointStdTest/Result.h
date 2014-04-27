
#pragma once

enum SResultType
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
	S_NOTINIT
};

class SResult
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
};

static bool Success(const SResult& r) { return (r == S_SUCCESS || r == S_INFO || r == S_WARN); }
static bool Failure(const SResult& r) { return !(r == S_SUCCESS || r == S_INFO || r == S_WARN); }
static bool Aborted(const SResult& r) { return r == S_ABORTED; }
static bool FullPool(const SResult& r) { return r == S_FULL; }
static bool NotFound(const SResult& r) { return r == S_NOTFOUND; }
static bool NotInitialized(const SResult& r) { return r == S_NOTINIT; }

// Support HRESULT

//static bool Failure(const HRESULT& r) { return r < 0; }