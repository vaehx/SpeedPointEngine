#pragma once

#include <Abstract\IEntity.h>

SP_NMSPACE_BEG

struct S_API IGameEngine;

class S_API CEntitySystem : public IEntitySystem
{
private:
	IGameEngine* m_pEngine;

public:
	CEntitySystem(IGameEngine* pEngine);

	// IEntitySystem:
	virtual IEntity* CreateEntity() const;
};

SP_NMSPACE_END