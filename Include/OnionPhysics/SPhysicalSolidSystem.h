// *****************************************************************************************

// This file is part of the SpeedPoint Game Engine (Physics Engine Component)!

// *****************************************************************************************

#include <SPrerequisites.h>
#include <Abstract\SSolidSystem.h>

namespace SpeedPoint
{
	// SPhysicalSolidSystem - another implementation of the abstract SSolidSystem class
	class SPhysicalSolidSystem : public SSolidSystem
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
		virtual SSolid* GetSolid(SP_ID id);

		// Get count of solids
		virtual unsigned int GetSolidCount();

		// Clearout this system including all stored solids
		virtual SResult Clear(void);
	};
}