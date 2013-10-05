// SpeedPoint Light

#include <Lighting\SLight.h>

namespace SpeedPoint
{

	S_API bool SLight::IsInRange( float fRange, const SVector3& pos )
	{
		// Definetly in directional light
		if( tyLightType == S_LIGHT_DIRECTIONAL ) return true;

		// Check if in range with surrounding sphere
		float fSurroundRadius = fRadius;

		if( tyLightType == S_LIGHT_SPOT && fLength > fRadius )
		{
			fSurroundRadius = fLength;
		}

		// Check sphere-sphere hitting
		float fMinDistance = fRange + fSurroundRadius;
		
		SVector3 vCenterDistance = ( pos - vPosition );

		if( vCenterDistance.x * vCenterDistance.x + 
			vCenterDistance.y * vCenterDistance.y +
			vCenterDistance.z * vCenterDistance.z
			<= fMinDistance * fMinDistance )
		{
			return true;
		}

		return false;
	}

}