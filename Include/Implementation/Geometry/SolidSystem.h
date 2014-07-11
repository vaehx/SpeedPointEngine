// ******************************************************************************************

//	SpeedPoint Solid System - A collection of solids

// ******************************************************************************************

#pragma once
#include <SPrerequisites.h>
#include <Abstract\ISolidSystem.h>
#include <Util\SPool.h>
#include "Solid.h"


SP_NMSPACE_BEG

class S_API SpeedPointEngine;


// SpeedPoint Solid System
class S_API SolidSystem : public ISolidSystem
{
private:
	SpeedPointEngine*	m_pEngine;
	SPool<Solid>	m_plSolids;

public:		
	// Initialize this system
	virtual SResult Initialize(SpeedPointEngine* pEngine);

	// Get pointer to the SpeedPoint Engine instance, this solidsystem is using
	virtual SpeedPointEngine* GetEngine();

	// Add a new solid and return id of it
	virtual SP_ID AddSolid(void);

	// Get a pointer to a solid by its id
	virtual ISolid* GetSolid(SP_ID id);

	// Get solid count
	virtual unsigned int GetSolidCount();

	// Clearout this system including all stored solids
	virtual SResult Clear(void);
};



SP_NMSPACE_END