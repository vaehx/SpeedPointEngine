#include <Implementation\Geometry\PhysicalComponent.h>

SP_NMSPACE_BEG

S_API CPhysicalComponent::CPhysicalComponent()
	: IComponent(),
	m_bTrash(false)
{
}

S_API void CPhysicalComponent::Release()
{
	m_bTrash = true;
	
	if (m_pEntity)
	{
		m_pEntity->ReleaseComponent(this);
		m_pEntity = 0;
	}
}

S_API bool CPhysicalComponent::IsTrash() const
{
	return m_bTrash;
}

// -----------------------------------------------------------------------------

S_API void CPhysicalComponent::OnEntityTransformed()
{





	//TODO







}

SP_NMSPACE_END
