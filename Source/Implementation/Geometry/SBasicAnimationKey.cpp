// SpeedPoint Basic Animation Key

#include <Implementation\Geometry\SBasicAnimationKey.h>

namespace SpeedPoint
{
	// **********************************************************************************

	S_API void SBasicAnimationKey::SetTimecode( float timeCode )
	{
		if( timeCode > 0 )
			fTimeCode = timeCode;
	}

	// **********************************************************************************

	S_API void SBasicAnimationKey::SetJointPosition( SP_UNIQUE iJoint, const SVector3& vec )
	{
		SJointMovement jointMovement;
		jointMovement.iJoint = iJoint;
		jointMovement.vVector = vec;

		aMovements.AddItem( jointMovement, NULL );		
	}

	// **********************************************************************************
	S_API void SBasicAnimationKey::Clear( void )
	{
		aMovements.Clear();
	}

}