// ********************************************************************************************

//	SpeedPoint Particle System

// ********************************************************************************************

#pragma once
#include <SPrerequisites.h>
#include <Abstract\SParticleSystem.h>
#include <SPool.h>
#include "SBasicParticle.h"

namespace SpeedPoint
{
	// SpeedPoint Particle System
	class S_API SBasicParticleSystem : public SParticleSystem
	{
	public:
		SPool<SBasicParticle> plParticles;

		//// TODO implement missing functions
	};
}