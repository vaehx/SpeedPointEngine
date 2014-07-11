// ********************************************************************************************

//	SpeedPoint Basic Animation Key

// ********************************************************************************************

#pragma once
#include <SPrerequisites.h>
#include <Util\SPool.h>
#include <Abstract\IAnimationKey.h>
#include <Util\SVector3.h>


SP_NMSPACE_BEG



// SpeedPoint Joint Position
struct S_API SJointMovement
{
	SP_UNIQUE	iJoint;
	SVector3	vVector;

	// Default constructor
	SJointMovement() {};

	// Copy constructor
	SJointMovement( const SJointMovement& other )
		: iJoint( other.iJoint ),
		vVector( other.vVector ) {}
};

// SpeedPoint Basic Animation Key
class S_API AnimationKey : public IAnimationKey
{
public:
	float			fTimeCode;
	SPool<SJointMovement>	aMovements;

	// Default constructor
	AnimationKey()
		: fTimeCode( 0 ) {};

	// Copy constructor
	AnimationKey( const AnimationKey& other )
		: fTimeCode( other.fTimeCode )
		/* , aMovements( other.aMovement ) // on hold for SPool to support copy */ {};

	void SetTimecode( float timeCode );
	void SetJointPosition( SP_UNIQUE iJoint, const SVector3& vec );
	void Clear( void );
};


SP_NMSPACE_END