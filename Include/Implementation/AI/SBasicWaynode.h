// ********************************************************************************************

//	SpeedPoint AI Waynet

// ********************************************************************************************

#pragma once
#include <SPrerequisites.h>
#include "SBasicWaynode.h"
#include <SVector3.h>

namespace SpeedPoint
{
	// SpeedPoint WayNode
	struct S_API SWayNode
	{
		SVector3		vPosition;
		SWayNode**		pConnected;
	};
}