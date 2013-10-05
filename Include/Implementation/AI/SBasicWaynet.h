// ********************************************************************************************

//	SpeedPoint AI Waynet

// ********************************************************************************************

#pragma once
#include <SPrerequisites.h>
#include "SBasicWaynode.h"

namespace SpeedPoint
{
	// SpeedPoint AI Waynet
	class S_API SBasicWaynet
	{
	private:
		SpeedPointEngine*	pEngine;
		SPool<SWaynode>		aWayNodes;

	public:
		SResult Initialize( SpeedPointEngine* pEng );
		SResult AddWayNode( const SVector3& vPosition );
		SResult Clear( void );
	};
}