#include "Entity.h"
#include <Implementation\Geometry\EntitySystem.h>
#include <Abstract\IGameEngine.h>

SP_NMSPACE_BEG

S_API CEntitySystem::CEntitySystem(IGameEngine* pEngine)
	: m_pEngine(pEngine)
{
}

S_API IEntity* CEntitySystem::CreateEntity() const
{
	return new CEntity(this);
}

SP_NMSPACE_END