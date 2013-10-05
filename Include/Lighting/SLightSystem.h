#pragma once
#include <SPrerequisites.h>
#include "SLight.h"
#include <SPool.h>

namespace SpeedPoint
{
	class S_API SLightSystem
	{
	public:	
		SPool<SLight> plLights;

		// --

		// Add a new Light
		SResult AddLight( SLight* pLight );
		
		// Get a Light Pointer
		SLight* GetLight( SP_ID iUID );

		// Get a light by its specification
		SLight* GetLightBySpecification( SString cSpec );

		// Get Light Specification
		SString GetLightSpecification( SP_ID iUID );
		
		// Get Light Unique ID
		SP_ID GetLightUID( SString spec );

		// remove Light
		SResult RemoveLight( SP_ID iUID );

		// ForEach Light
		SResult	ForEachLight( void (*iterationFunc)(SLight*, const SP_ID&) );

		// Get lights in the range of a radius		
		SResult GetLightsInRange( float fRadius, SP_ID** pLightIds );

		// Clearout all lights stored in this lightsystem
		SResult Clear( void );
	};
}