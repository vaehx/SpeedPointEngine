// ********************************************************************************************

//	SpeedPoint Basic Animation Joint

// ********************************************************************************************

#pragma once
#include <SPrerequisites.h>
#include <Util\SPool.h>
#include <Abstract\IAnimationJoint.h>


SP_NMSPACE_BEG


struct S_API ISolid;


// SpeedPoint Animation Joint
class S_API AnimationJoint : public IAnimationJoint
{
public:
	ISolid*			pSolid;
	SPool<SP_UNIQUE>	plVertices;

	// --

	void SetSolid( ISolid* psol );
	void AssignVertex( SP_UNIQUE iVertex );
	void MoveVertices( const SVector3& vec );
	void Clear( void );
};



SP_NMSPACE_END