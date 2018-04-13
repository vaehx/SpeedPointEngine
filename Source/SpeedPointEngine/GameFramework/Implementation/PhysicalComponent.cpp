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
		m_Scale = m_pEntity->GetScale();
	}
}

// ------------------------------------------------------------------------------------------------------------
S_API void CPhysicalComponent::OnSimulationFinished()
{
	if (m_pEntity)
	{
		m_pEntity->SetPos(m_State.pos - m_State.centerOfMass);
		m_pEntity->SetRotation(m_State.rotation);
		// scale not modified by physics
	}
}

// ------------------------------------------------------------------------------------------------------------
S_API void CPhysicalComponent::OnIntersection(const geo::SIntersection& contact, const PhysObject* other)
{
}

// ------------------------------------------------------------------------------------------------------------
S_API void CPhysicalComponent::LoadProxyFromSPM(const string& proxyGeomFile)
{
	m_ProxyGeomFile = proxyGeomFile;
	
	SetProxyPtr(0);
	if (!proxyGeomFile.empty())
	{
		// Load proxy from spm file
		SPMManager* pSPMManager = SpeedPointEnv::GetEngine()->GetSPMManager();
		const CSPMLoader* pSPM = pSPMManager->Load(proxyGeomFile);
		if (pSPM)
		{
			const SSPMPhysInfo& pi = pSPM->GetFileUnmutable().physInfo;
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

// ------------------------------------------------------------------------------------------------------------
S_API void CPhysicalComponent::Serialize(ISerContainer* ser, bool serialize)
{
	if (serialize)
	{
		ser->SetFloat("mass", m_State.M);
		ser->SetString("proxyGeomFile", m_ProxyGeomFile);
		
		string behavior = "static";
		switch (GetBehavior())
		{
		case ePHYSOBJ_BEHAVIOR_LIVING:
			behavior = "living";
			break;
		case ePHYSOBJ_BEHAVIOR_RIGID_BODY:
			behavior = "rigid";
			break;
		case ePHYSOBJ_BEHAVIOR_STATIC:
		default:
			break;
		}

		if (behavior != "static")
			ser->SetString("behavior", behavior);
	}
	else
	{
		string behavior = strtolower(ser->GetString("behavior", "static"));
		if (behavior == "static")
			SetBehavior(ePHYSOBJ_BEHAVIOR_STATIC);
		else if (behavior == "rigid")
			SetBehavior(ePHYSOBJ_BEHAVIOR_RIGID_BODY);
		else if (behavior == "living")
			SetBehavior(ePHYSOBJ_BEHAVIOR_LIVING);

		m_State.M = ser->GetFloat("mass", m_State.M);
		m_State.Minv = 1.0f / m_State.M;

		LoadProxyFromSPM(ser->GetString("proxyGeomFile"));
	}
}

SP_NMSPACE_END
