// ********************************************************************************************

//	SpeedPoint Particle System

// ********************************************************************************************

#pragma once
#include <SPrerequisites.h>
#include <Abstract\IParticleSystem.h>
#include <Util\SPool.h>
#include "SBasicParticle.h"

namespace SpeedPoint
{
	// SpeedPoint Particle System
	class S_API SBasicParticleSystem : public IParticleSystem
	{
	public:
		SPool<SBasicParticle> plParticles;

		//// TODO implement missing functions
	};
}