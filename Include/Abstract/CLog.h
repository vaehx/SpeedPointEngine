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


struct ILogHandle
{
	virtual void OnLog(SResult res, const SString& msg) = 0;
};


class CLog
{
private:
	static std::vector<ILogHandle*> m_LogHandles;

public:

	static SResult Log(SResult res, const SString& msg)
	{
		char* resDsc = "";
		switch (res)
		{
		case S_ERROR:
			resDsc = "Error: ";
			break;
		}

		unsigned int msgln = msg.GetLength + 20;
		char* msgstr = new char[msgln];
		SPSPrintf(msgstr, msgln, "%s%s", resDsc, (char*)msg);

		for (auto itLogHandle = m_LogHandles.begin(); itLogHandle != m_LogHandles.end(); itLogHandle++)
		{
			if (IS_VALID_PTR(*itLogHandle))
				(*itLogHandle)->OnLog(res, msgstr);
		}

		printf("%s", msgstr);
		return res;
	}

	SResult Log(SResult res, const char* fmt, ...)
	{
		// TODO
		return res;
	}
};


SP_NMSPACE_END