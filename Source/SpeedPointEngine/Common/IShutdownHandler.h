#pragma once

#include "SPrerequisites.h"

SP_NMSPACE_BEG

// Summary:
//	An object that keeps a pointer to the engine or the renderer can implement this interface
//	to get informed about the shutdown. This prevents dangling objects during shutdown and allows
//	for resources to be properly released before shutdown.
struct S_API IShutdownHandler
{
	virtual SResult HandleShutdown() = 0;
	virtual string GetShutdownHandlerDesc() const { return "unknown"; }
};

struct S_API IShutdownManagerIntrl
{

};

class S_API ShutdownManager
{
private:
	static vector<IShutdownHandler*> m_Handlers;

public:
	static void RegisterShutdownHandler(IShutdownHandler* pShutdownHandler)
	{
		if (!pShutdownHandler)
			return;

		m_Handlers.push_back(pShutdownHandler);
	}

	static void UnregisterShutdownHandler(IShutdownHandler* pShutdownHandler)
	{
		for (auto itHandler = m_Handlers.begin(); itHandler != m_Handlers.end();)
		{
			if (*itHandler == pShutdownHandler)
				itHandler = m_Handlers.erase(itHandler);
			else
				++itHandler;
		}
	}

	static vector<IShutdownHandler*>* GetHandlers()
	{
		return &m_Handlers;
	}
};

SP_NMSPACE_END
