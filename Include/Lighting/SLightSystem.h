// ********************************************************************************************

//	This file is part of the SpeedPoint Game Engine!

// ********************************************************************************************

#pragma once
#include <SPrerequisites.h>
#include <Util\SPool.h>
#include "SLight.h"

namespace SpeedPoint
{
	// SpeedPoint Light System contains a bunch of light source instances
	class S_API SLightSystem
	{
	private:
		SPool<SLight> m_plLights;

	public:
		// Default destructor
		~SLightSystem();

		// Add a new Light
		SResult AddLight(SLight* pLight);
		
		// Get the count of all stored lights
		unsigned int GetLightCount();

		// Get a Light Pointer
		SLight* GetLight(SP_ID iUID);

		// Get a light by its specification
		SLight* GetLightBySpecification(SString cSpec);

		// Get Light Specification
		SString GetLightSpecification(SP_ID iUID);
		
		// Get Light Unique ID
		SP_ID GetLightUID(SString spec);

		// remove Light
		SResult RemoveLight(SP_ID iUID);

		// ForEach Light
		SResult	ForEachLight(void(*iterationFunc)(SLight*, const SP_ID&));

		// Get lights in the range of a radius
		SResult GetLightsInRange(SVector3 vPosition, f32 fRadius, SP_ID** pLightIds, usint32* pnIDs);

		// Clearout all lights stored in this lightsystem
		SResult Clear(void);
	};
}