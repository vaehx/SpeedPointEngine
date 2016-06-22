#include "Entity.h"
#include <Implementation\Geometry\EntitySystem.h>
#include <Implementation\Geometry\RenderableComponent.h>
#include <Abstract\IGameEngine.h>
#include <Abstract\I3DEngine.h>
#include <Abstract\IPhysics.h>
#include <Implementation\Geometry\PhysicalComponent.h>

SP_NMSPACE_BEG

S_API CEntitySystem::CEntitySystem(IGameEngine* pEngine)
	: m_pEngine(pEngine)
{
	m_pEngine->Get3DEngine()->CreateRenderObjectPool<CRenderableComponent>();
	m_pEngine->GetPhysics()->CreateRenderObjectPool<PhysicalComponent>();
}

S_API IEntity* CEntitySystem::CreateEntity()
{
	return new CEntity(this);
}

S_API IRenderableComponent* CEntitySystem::CreateRenderableComponent() const
{
	return (CRenderableComponent*)m_pEngine->Get3DEngine()->GetRenderObject();
}

S_API IPhysicalComponent* CEntitySystem::CreatePhysicalComponent() const
{
	return (PhysicalComponent*)m_pEngine->GetPhysics()->GetPhysObjects();
}



S_API void CEntitySystem::RemoveRenderableComponent(IRenderableComponent* renderable) const
{
	m_pEngine->Get3DEngine()->ReleaseRenderObject((IRenderObject**)&renderable);
}

S_API void CEntitySystem::RemovePhysicalComponent(IPhysicalComponent* physical) const
{
	m_pEngine->GetPhysics()->ReleasePhysObject((IPhysObject**)&physical);
}

SP_NMSPACE_END