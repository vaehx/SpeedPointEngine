// ******************************************************************************************

//	SpeedPoint Solid System - A collection of solids

// ******************************************************************************************

#pragma once
#include <SPrerequisites.h>
#include <Abstract\SSolidSystem.h>
#include <SPool.h>
#include "SBasicSolid.h"

namespace SpeedPoint
{
	// SpeedPoint Solid System
	class S_API SBasicSolidSystem : public SSolidSystem
	{
	public:
		SpeedPointEngine*	pEngine;
		SPool<SBasicSolid>	plSolids;		

		SResult Initialize( SpeedPointEngine* engine );
		//SP_ID AddSolid( /*???*/ );
		SSolid* GetSolid( SP_ID id );
		SResult Clear( void );
	};
}