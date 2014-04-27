// ******************************************************************************************

//	SpeedPoint Solid System - A collection of solids

// ******************************************************************************************

#pragma once
#include <SPrerequisites.h>
#include <Abstract\SSolidSystem.h>
#include <SPool.h>
#include "SBasicSolid.h"

namespace SpeedPoint
{
	// SpeedPoint Solid System
	class S_API SBasicSolidSystem : public SSolidSystem
	{
	private:
		SpeedPointEngine*	m_pEngine;
		SPool<SBasicSolid>	m_plSolids;

	public:		
		// Initialize this system
		virtual SResult Initialize(SpeedPointEngine* pEngine);

		// Get pointer to the SpeedPoint Engine instance, this solidsystem is using
		virtual SpeedPointEngine* GetEngine();

		// Add a new solid and return id of it
		virtual SP_ID AddSolid(void);

		// Get a pointer to a solid by its id
		virtual SSolid* GetSolid(SP_ID id);

		// Get solid count
		virtual unsigned int GetSolidCount();

		// Clearout this system including all stored solids
		virtual SResult Clear(void);
	};
}