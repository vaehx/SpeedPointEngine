#pragma once

#include "..\IEntitySystem.h"
#include <map>

using std::map;

SP_NMSPACE_BEG

class S_API EntitySystem : public IEntitySystem
{
private:
	map<string, IEntityComponentFactory*> m_Factories;

protected:
	virtual void RegisterComponentFactory(IEntityComponentFactory* pFactory);
	virtual IEntityComponentFactory* FindComponentFactory(const string& name) const;
};

SP_NMSPACE_END
