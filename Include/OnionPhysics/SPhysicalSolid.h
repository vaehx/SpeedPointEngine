// *****************************************************************************************

// This file is part of the SpeedPoint Game Engine (Physics Engine Component)!

// *****************************************************************************************

#pragma once

#include <SPrerequisites.h>
#include <Implementation\Geometry\SBasicSolid.h>
#include "SPhysIntersectInfo.h"

namespace SpeedPoint
{
	// Implementation of the ISolid for Physics based on the Basic Implementation
	class SPhysicalSolid : public SBasicSolid
	{
	private:
		SVector3	m_Velocity;	// current velocity

	public:
		// Get the current velocity (direction and speed) of this solid
		SVector3 GetVelocity() { return m_Velocity; }

		// Apply a momentum to this solid and get resulting velocity
		SVector3 ApplyMomentum(SVector3 momentum);

		// Check collision between this and another Physical Solid
		SPhysIntersectInfo CheckCollision(SPhysicalSolid* pOtherPhysicalSolid, float fTimeDiff);

		//~~~~~~~
		// TODO: Further methods
		// ...
		//~~~~~~~
	};
}