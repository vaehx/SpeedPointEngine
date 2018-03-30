#include "..\PhysicalComponent.h"
#include "SPMManager.h"
#include "../IGameEngine.h"
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
		m_State.pos = m_pEntity->GetPos() + m_State.centerOfMass;
		m_State.rotation = m_pEntity->GetRotation();
	}
}

// ------------------------------------------------------------------------------------------------------------
S_API void CPhysicalComponent::OnSimulationFinished()
{
	if (m_pEntity)
	{
		m_pEntity->SetPos(m_State.pos - m_State.centerOfMass);
		m_pEntity->SetRotation(m_State.rotation);
	}
}

// ------------------------------------------------------------------------------------------------------------
S_API void CPhysicalComponent::OnIntersection(const geo::SIntersection& contact, const PhysObject* other)
{
}

// ------------------------------------------------------------------------------------------------------------
S_API void CPhysicalComponent::Serialize(ISerContainer* ser, bool serialize)
{
	if (serialize)
	{
		ser->SetFloat("mass", m_State.M);

		// TODO: Serialize collision shape, ...
	}
	else
	{
		m_State.M = ser->GetFloat("mass", m_State.M);
		m_State.Minv = 1.0f / m_State.M;

		string proxyGeomFile = ser->GetString("proxyGeomFile");
		if (!proxyGeomFile.empty())
		{
			SPMManager* pSPMManager = SpeedPointEnv::GetEngine()->GetSPMManager();
			const CSPMLoader* pSPM = pSPMManager->Load(proxyGeomFile);
			if (pSPM)
			{
				const SSPMPhysInfo& pi = pSPM->GetPhysInfo();
				SetMass(pi.mass);
				if (!pi.proxyShapes.empty())
				{
					const SSPMColShape* pSPMColShape = pi.proxyShapes.at(0);
					geo::shape* pshape = SPMManager::ConvertSPMColShapeToGeoShape(pSPMColShape);
					if (pshape)
						SetProxyPtr(pshape);
				}
			}
		}
	}
}

SP_NMSPACE_END
