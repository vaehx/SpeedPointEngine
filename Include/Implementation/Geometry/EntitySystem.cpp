#include "Entity.h"
#include <Implementation\Geometry\EntitySystem.h>
#include <Implementation\Geometry\RenderableComponent.h>
#include <Abstract\IGameEngine.h>
#include <Abstract\I3DEngine.h>

SP_NMSPACE_BEG

S_API CEntitySystem::CEntitySystem(IGameEngine* pEngine)
	: m_pEngine(pEngine)
{
	m_pEngine->Get3DEngine()->CreateRenderObjectPool<CRenderableComponent>();
}

S_API IEntity* CEntitySystem::CreateEntity()
{
	return new CEntity(this);
}

S_API IRenderableComponent* CEntitySystem::CreateRenderableComponent() const
{
	return (CRenderableComponent*)m_pEngine->Get3DEngine()->GetRenderObject();	
}

SP_NMSPACE_END