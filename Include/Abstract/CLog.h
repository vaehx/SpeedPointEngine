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

S_API extern std::vector<ILogListener*> g_LogListeners;

class S_API CLog
{
public:
	static void RegisterListener(ILogListener* pListener)
	{
		g_LogListeners.push_back(pListener);
	}

	static void UnregisterListener(ILogListener* pListener)
	{
		for (auto itListener = g_LogListeners.begin(); itListener != g_LogListeners.end();)
		{
			if (*itListener == pListener)
				itListener = g_LogListeners.erase(itListener);
			else
				itListener++;
		}
	}

	static SResult Log(SResult res, const SString& msg)
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

		if (g_LogListeners.size() > 0)
		{
			for (auto itListener = g_LogListeners.begin(); itListener != g_LogListeners.end(); itListener++)
			{
				if (IS_VALID_PTR(*itListener))
					(*itListener)->OnLog(res, msgstr);
			}
		}

		printf("%s", msgstr);
		return res;
	}

	static SResult Log(SResult res, const char* fmt, ...)
	{
		char out[300];
		va_list args;
		va_start(args, fmt);		
		//SPSPrintf(out, 500, fmt, args);
		vsnprintf_s(out, 300, fmt, args);
		va_end(args);

		return Log(res, SString(out));
	}
};


SP_NMSPACE_END