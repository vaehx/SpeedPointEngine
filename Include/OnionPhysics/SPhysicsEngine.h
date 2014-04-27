// *****************************************************************************************

// This file is part of the SpeedPoint Game Engine (Physics Engine Component)!

// *****************************************************************************************

#pragma once

#include <SPrerequisites.h>

namespace SpeedPoint
{
	// The Physics part of the SpeedPoint Game Engine
	// Notice: This Physics Engine used physics-specific implementations of the abstract classes SSolid and SSolidSystem.
	class S_API SPhysicsEngine
	{
	private:
		SpeedPointEngine*	m_pEngine;

	public:
		// default constructor
		SPhysicsEngine()
			: m_pEngine(0)
		{
		}

		// Initialize the Physics Engine
		SResult Initialize(SpeedPointEngine* pEngine)
		{
			m_pEngine = pEngine;
		}

		// Handle Physics of all solids with each solid in a solid system
		/*
		SResult HandlePhysics(SPhysicalSolidSystem* pPhysSolidSystem)
		{
			unsigned int nPhysSolids = pPhysSolidSystem->GetSolidCount();

			// Go through all solids found in this solid system and let it interact with all other solids		
			for (unsigned int iSolid1 = 0; iSolid1 < (nPhysSolids - 1); ++iSolid1)	// -1 cause we dont want the last solid to interact with itself
			{
				SPhysicalSolid* pSolid1 = (SPhysicalSolid*)pSolidSystem->GetSolid(iSolid);
				for (unsigned int iSolid2 = (iSolid1 + 1); iSolid2 < nPhysSolids; ++iSolid2)
				{
					SPhysicalSolid* pSolid2 = (SPhysicalSolid*)pSolidSystem->GetSolid(iSolid2);

					// to avoid frame overlapping, interpolate position over last frame time and check collision (fElapsedTime)				
					float fElapsedTime = pPhysSolidSystem->GetEngine()->GetElapsedTimeLastFrame();
					SPhysCollisionInfo collInfo = pSolid1->CheckCollision(pSolid2, fElapsedTime);
					if (collInfo.m_bCollided)
					{
						// ...
					}
				}
			}
		}
		*/
	};
}