// ********************************************************************************************

//	SpeedPoint Physical Object

//	This is a virtual class. You cannot instantiate it.
//	Please use Specific Implementation to instantiate.

//	NOTE: The implementation of this class can be found in the Onion Physics engine

// ********************************************************************************************

#pragma once

#include <SPrerequisites.h>

namespace SpeedPoint
{
	// A SpeedPoint Object with Physical properties
	class S_API IPhysicalObject
	{
	public:
		// Enable the physics calculation
		virtual void EnablePhysics(void) = 0;

		// Disable the physics calculation
		virtual void DisablePhysics(void) = 0;

		// Get a pointer to the current damage state
		virtual float* Damage(void) = 0;

		// Apply damage to this physical and return resulting damage
		virtual float* Damage(const float& dmg) = 0;

		// Get pointer to the current heat
		virtual float* Heat(void) = 0;

		// Apply heat to the physical and return resulting heat
		virtual float* Heat(const float& heat) = 0;

		// Tick the physics calculation. Will not to anything when physics disabled
		virtual SResult TickPhysics(float fTime) = 0;
	};
}