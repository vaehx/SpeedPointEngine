// **************************************************************************************

//	SpeedPoint SolidSystem as collection of solids

//	This is a virtual class. You cannot instantiate it.
//	Please use Specific Implementation to instantiate.

// **************************************************************************************

#pragma once

#include <SPrerequisites.h>
#include <SPool.h>
#include "SSolid.h"

namespace SpeedPoint
{
	// SpeedPoint Solid Collection (abstract)
	class S_API SSolidSystem
	{
	public:
		// Initialize this system
		virtual SResult Initialize( SpeedPointEngine* pEngine ) = 0;

		// Add a new solid and return id of it
		virtual SP_ID AddSolid( /*???*/ ) = 0;

		// Get a pointer to a solid by its id
		virtual SSolid* GetSolid( SP_ID id ) = 0;

		// Clearout this system including all stored solids
		virtual SResult Clear( void ) = 0;
	};
};