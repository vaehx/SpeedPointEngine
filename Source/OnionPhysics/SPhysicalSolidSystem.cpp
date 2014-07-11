// ********************************************************************************************

//	This file is part of the SpeedPoint Game Engine

//	(c) 2011-2014 Pascal Rosenkranz aka iSmokiieZz
//	All rights reserved.

// ********************************************************************************************

#include <OnionPhysics\SPhysicalSolidSystem.h>

namespace SpeedPoint
{
	S_API SResult SPhysicalSolidSystem::Initialize(SpeedPointEngine* pEngine)
	{
		SP_ASSERTR(pEngine, S_INVALIDPARAM, "Given engine is zero!");

		m_pEngine = pEngine;

		return S_SUCCESS;
	}

	// ********************************************************************************************

	S_API SpeedPointEngine* SPhysicalSolidSystem::GetEngine()
	{
		return m_pEngine;
	}

	// ********************************************************************************************
	
	S_API SP_ID SPhysicalSolidSystem::AddSolid(void)
	{
		SP_ID id;
		m_PhysicalSolids.AddItem(SPhysicalSolid(), &id);		

		return id;
	}

	// ********************************************************************************************

	S_API ISolid* SPhysicalSolidSystem::GetSolid(SP_ID id)
	{
		Solid* pBasicSolid = m_PhysicalSolids.GetItemByUID(id);

		return (ISolid*)pBasicSolid;
	}

	// ********************************************************************************************
	
	S_API unsigned int SPhysicalSolidSystem::GetSolidCount()
	{
		return m_PhysicalSolids.GetUsageSize();
	}
	
	// ********************************************************************************************

	S_API SResult SPhysicalSolidSystem::Clear(void)
	{
		for (usint32 iSolid = 0; iSolid < m_PhysicalSolids.GetSize(); iSolid++)
		{
			Solid* pBasicSolid = m_PhysicalSolids.GetItemByIndirectionIndex(iSolid);
			if (pBasicSolid)
				pBasicSolid->Clear();
		}

		m_PhysicalSolids.Clear();
		m_PhysicalSolids.ResetCounts();

		return S_SUCCESS;
	}
}