/////////////////////////////////////////////////////////////////////////////////////////////////
//
//	This file is part of the SpeedPoint Engine.
//	Copyright (c) 2014-2015, Pascal Rosenkranz.
//
/////////////////////////////////////////////////////////////////////////////////////////////////

#pragma once

#include "SPrerequisites.h"
#include <vector>
#include <iostream>
#include <mutex>

using std::mutex;
using std::vector;

SP_NMSPACE_BEG

enum S_API ELogLevel
{
	eLOGLEVEL_ERROR = 0,
	eLOGLEVEL_WARN,
	eLOGLEVEL_INFOS,
	eLOGLEVEL_DEBUG
};

struct S_API ILogListener
{
	virtual void OnLog(SResult res, const string& msg) = 0;
};

////////////////////////////////////////////////////////////////////////////////////////////////
// Static Log
////////////////////////////////////////////////////////////////////////////////////////////////

// Outputs to stdout by default
// Additional listeners can be registered to add more log handlers
class S_API CLogIntrnl
{
private:
	vector<ILogListener*> m_Listeners;
	mutex m_Lock;
	ELogLevel m_LogLevel;

public:
	CLogIntrnl()
		: m_LogLevel(eLOGLEVEL_INFOS)
	{
	}

	void RegisterListener(ILogListener* pListener)
	{
		m_Listeners.push_back(pListener);
	}

	void UnregisterListener(ILogListener* pListener)
	{
		for (auto itListener = m_Listeners.begin(); itListener != m_Listeners.end();)
		{
			if (*itListener == pListener)
				itListener = m_Listeners.erase(itListener);
			else
				itListener++;
		}
	}

	void SetLogLevel(ELogLevel logLevel)
	{
		m_LogLevel = logLevel;
	}

	ELogLevel GetLogLevel() const
	{
		return m_LogLevel;
	}

	SResult LogString(SResult res, const string& msg)
	{
		if (res == S_ERROR && m_LogLevel < eLOGLEVEL_ERROR) return res;
		else if (res == S_WARN && m_LogLevel < eLOGLEVEL_WARN) return res;
		else if (res == S_INFO && m_LogLevel < eLOGLEVEL_INFOS) return res;
		else if (res == S_DEBUG && m_LogLevel < eLOGLEVEL_DEBUG) return res;

		string formatted;
		switch (res)
		{
		case S_ERROR: formatted = "[ERROR] "; break;
		case S_WARN: formatted = "[Warning] "; break;
		}

		formatted += msg + "\n";

		m_Lock.lock();

		if (m_Listeners.size() > 0)
		{
			for (auto itListener = m_Listeners.begin(); itListener != m_Listeners.end(); itListener++)
			{
				if (IS_VALID_PTR(*itListener))
					(*itListener)->OnLog(res, formatted);
			}
		}

		std::cout << formatted;

		m_Lock.unlock();
		return res;
	}	

	SResult Log(SResult res, const char* fmt, va_list args)
	{
		static char out[300];
		vsnprintf_s(out, 300, fmt, args);		

		return LogString(res, out);
	}

	SResult Log(SResult res, const char* fmt, ...)
	{
		va_list args;
		va_start(args, fmt);
		Log(res, fmt, args);
		va_end(args);

		return res;
	}
};

S_API extern CLogIntrnl g_LogIntrnl;

class S_API CLog
{
public:
	static void RegisterListener(ILogListener* pListener)
	{		
		g_LogIntrnl.RegisterListener(pListener);
	}

	static void UnregisterListener(ILogListener* pListener)
	{
		g_LogIntrnl.UnregisterListener(pListener);
	}

	static void SetLogLevel(ELogLevel logLevel)
	{
		g_LogIntrnl.SetLogLevel(logLevel);
	}

	static ELogLevel GetLogLevel()
	{
		return g_LogIntrnl.GetLogLevel();
	}

	static SResult Log(SResult res, const string& msg) { return g_LogIntrnl.LogString(res, msg); }
	static SResult LogInfo(const string& msg) { return g_LogIntrnl.LogString(S_INFO, msg); }
	static SResult LogError(const string& msg) { return g_LogIntrnl.LogString(S_ERROR, msg); }
	static SResult LogDebug(const string& msg) { return g_LogIntrnl.LogString(S_DEBUG, msg); }

	static SResult Log(SResult res, const char* fmt, ...)
	{
		va_list args;
		va_start(args, fmt);
		g_LogIntrnl.Log(res, fmt, args);
		va_end(args);

		return res;
	}

	static SResult LogVargs(SResult res, const char* fmt, va_list args)
	{
		g_LogIntrnl.Log(res, fmt, args);
		return res;
	}
};


SP_NMSPACE_END