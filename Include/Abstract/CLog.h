/////////////////////////////////////////////////////////////////////////////////////////////////
//
//	This file is part of the SpeedPoint Engine.
//	Copyright (c) 2014-2015, Pascal Rosenkranz.
//
/////////////////////////////////////////////////////////////////////////////////////////////////

#pragma once

#include <SPrerequisites.h>
#include <Util\SString.h>
#include <vector>

SP_NMSPACE_BEG


struct S_API ILogListener
{
	virtual void OnLog(SResult res, const SString& msg) = 0;
};

////////////////////////////////////////////////////////////////////////////////////////////////
// Static Log
////////////////////////////////////////////////////////////////////////////////////////////////

class S_API CLogIntrnl
{
private:
	std::vector<ILogListener*> m_LogListeners;
	bool m_bLogLocked;
public:
	CLogIntrnl()
		: m_bLogLocked(false)
	{
	}

	void RegisterListener(ILogListener* pListener)
	{
		m_LogListeners.push_back(pListener);
	}

	void UnregisterListener(ILogListener* pListener)
	{
		for (auto itListener = m_LogListeners.begin(); itListener != m_LogListeners.end();)
		{
			if (*itListener == pListener)
				itListener = m_LogListeners.erase(itListener);
			else
				itListener++;
		}
	}

	SResult Log(SResult res, const SString& msg)
	{
		char* resDsc = "";
		switch (res)
		{
		case S_ERROR:
			resDsc = "Error: ";
			break;
		}

		unsigned int msgln = msg.GetLength() + 20;
		char* msgstr = new char[msgln];
		SPSPrintf(msgstr, msgln, "%s%s\n", resDsc, (char*)msg);

		while (m_bLogLocked)
		{
			Sleep(1);
		}

		m_bLogLocked = true;

		if (m_LogListeners.size() > 0)
		{
			for (auto itListener = m_LogListeners.begin(); itListener != m_LogListeners.end(); itListener++)
			{
				if (IS_VALID_PTR(*itListener))
					(*itListener)->OnLog(res, msgstr);
			}
		}

		printf("%s", msgstr);

		m_bLogLocked = false;
		return res;
	}	

	SResult Log(SResult res, const char* fmt, va_list args)
	{
		char out[300];		
		//SPSPrintf(out, 500, fmt, args);
		vsnprintf_s(out, 300, fmt, args);		

		return Log(res, SString(out));
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

//S_API extern std::vector<ILogListener*> g_LogListeners;
//S_API extern bool g_bLogLocked;

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

	static SResult Log(SResult res, const SString& msg)
	{
		return g_LogIntrnl.Log(res, msg);
	}

	static SResult Log(SResult res, const char* fmt, ...)
	{
		va_list args;
		va_start(args, fmt);
		g_LogIntrnl.Log(res, fmt, args);
		va_end(args);

		return res;
	}
};


SP_NMSPACE_END