// ********************************************************************************************

//	SpeedPoint Basic Animation Key

// ********************************************************************************************

#pragma once
#include <SPrerequisites.h>
#include <Util\SPool.h>
#include <Abstract\IAnimationKey.h>
#include <Util\SVector3.h>

namespace SpeedPoint
{
	// SpeedPoint Joint Position
	struct SJointMovement
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
	class S_API SBasicAnimationKey : public IAnimationKey
	{
	public:
		float			fTimeCode;
		SPool<SJointMovement>	aMovements;

		// Default constructor
		SBasicAnimationKey()
			: fTimeCode( 0 ) {};

		// Copy constructor
		SBasicAnimationKey( const SBasicAnimationKey& other )
			: fTimeCode( other.fTimeCode )
			/* , aMovements( other.aMovement ) // on hold for SPool to support copy */ {};

		void SetTimecode( float timeCode );
		void SetJointPosition( SP_UNIQUE iJoint, const SVector3& vec );
		void Clear( void );
	};
}