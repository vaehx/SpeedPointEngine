// *****************************************************************************************

// This file is part of the SpeedPoint Game Engine (Physics Engine Component)!

// *****************************************************************************************

#include <SPrerequisites.h>
#include "SPhysicalSolid.h"
#include <Abstract\ISolidSystem.h>

namespace SpeedPoint
{
	// SPhysicalSolidSystem - another implementation of the abstract ISolidSystem class
	class S_API SPhysicalSolidSystem : public ISolidSystem
	{
	private:
		SPool<SPhysicalSolid>	m_PhysicalSolids;
		SpeedPointEngine*	m_pEngine;

	public:
		// Initialize this system
		virtual SResult Initialize(SpeedPointEngine* pEngine);

		// Get the engine, this solid system is using
		virtual SpeedPointEngine* GetEngine();

		// Add a new solid and return id of it
		virtual SP_ID AddSolid(void);

		// Get a pointer to a solid by its id
		virtual ISolid* GetSolid(SP_ID id);

		// Get count of solids
		virtual unsigned int GetSolidCount();

		// Clearout this system including all stored solids
		virtual SResult Clear(void);
	};
}