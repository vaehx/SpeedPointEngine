// ********************************************************************************************

//	SpeedPoint Bound Box

// ********************************************************************************************

#pragma once
#include <SPrerequisites.h>
#include <Abstract\Vector3.h>
#include "SPlane.h"


SP_NMSPACE_BEG


// SpeedPoint BoundBox is always axis aligned
class S_API SBoundBox
{
public:
	SVector3	vMin;
	SVector3	vMax;
	SPlane		aPlanes[6];

	// Move the boundbox relatively
	void MoveRelatively( const SVector3& vec );

	// Does it collide with another bound box
	bool HitsOtherBB( SBoundBox* pOther );
};

SP_NMSPACE_END