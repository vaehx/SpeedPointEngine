// ********************************************************************************************

//	SpeedPoint Animation Key

//	This is a virtual class. You cannot instantiate it.
//	Please use Specific Implementation to instantiate.

// ********************************************************************************************

#pragma once

#include <SPrerequisites.h>

namespace SpeedPoint
{
	// SpeedPoint Animation Key (abstract)
	class S_API IAnimationKey
	{
	public:
		// Set Timecode for this animation key.
		// Time is in Seconds, modified by SpeedPoint speed modifier
		virtual void SetTimecode( float fTimeCode ) = 0;

		// Set the Joint position to be moved to
		// This value is relative to the key before!
		// (If this is the default key the position equals the start position of the joint)
		virtual void SetJointPosition( SP_UNIQUE iJoint, const SVector3& vPosition ) = 0;
		
		// Clearout Vertexposition and timecode
		virtual void Clear( void ) = 0;
		
		//// TODO
	};
}