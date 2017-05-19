#include "..\PhysicalComponent.h"
#include <Common\SerializationTools.h>

SP_NMSPACE_BEG

S_API CPhysicalComponent::CPhysicalComponent()
	: IComponent()
{
}

// ------------------------------------------------------------------------------------------------------------
S_API void CPhysicalComponent::OnRelease()
{
	PhysObject::Clear();
}

// ------------------------------------------------------------------------------------------------------------
S_API void CPhysicalComponent::OnSimulationPrepare()
{
	if (m_pEntity)
	{
		m_State.pos = m_pEntity->GetPos() + m_State.centerOffs;		
		m_State.rotation = m_pEntity->GetRotation();
	}
}

// ------------------------------------------------------------------------------------------------------------
S_API void CPhysicalComponent::OnSimulationFinished()
{
	if (m_pEntity)
	{
		m_pEntity->SetPos(m_State.pos - m_State.centerOffs);
		m_pEntity->SetRotation(m_State.rotation);
	}
}

// ------------------------------------------------------------------------------------------------------------
S_API void CPhysicalComponent::OnIntersection(const geo::SIntersection& contact, const PhysObject* other)
{
}

SP_NMSPACE_END
