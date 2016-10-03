// SpeedPoint Basic Animation Key

#include <Implementation\GameFacilities\AnimationKey.h>

namespace SpeedPoint
{
	// **********************************************************************************

	S_API void AnimationKey::SetTimecode( float timeCode )
	{
		if( timeCode > 0 )
			fTimeCode = timeCode;
	}

	// **********************************************************************************

	S_API void AnimationKey::SetJointPosition( SP_UNIQUE iJoint, const SVector3& vec )
	{
		SJointMovement jointMovement;
		jointMovement.iJoint = iJoint;
		jointMovement.vVector = vec;

		aMovements.AddItem( jointMovement, NULL );		
	}

	// **********************************************************************************
	S_API void AnimationKey::Clear( void )
	{
		aMovements.Clear();
	}

}