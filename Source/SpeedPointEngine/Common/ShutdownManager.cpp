#include "IShutdownHandler.h"

SP_NMSPACE_BEG

vector<IShutdownHandler*> ShutdownManager::m_Handlers;

SP_NMSPACE_END
