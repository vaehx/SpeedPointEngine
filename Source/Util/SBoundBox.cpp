// ********************************************************************************************

//	SpeedPoint Bound Box

// ********************************************************************************************

#include <Util\SBoundBox.h>

namespace SpeedPoint
{
	// ********************************************************************************************
	S_API void SBoundBox::MoveRelatively( const SVector3& vec )
	{
		vMin += vec;
		vMax += vec;
	}

	// ********************************************************************************************

	S_API bool SBoundBox::HitsOtherBB( SBoundBox* pOther )
	{		
		SVector3 vOMin = pOther->vMin;
		SVector3 vOMax = pOther->vMax;

		if( vOMin.x <= vMax.x && vOMin.y <= vMax.y && vOMin.z <= vMax.z &&
			vOMax.x >= vMin.x && vOMax.y >= vMin.y && vOMax.z >= vMin.z )
		{
			return true;
		}

		// try the other way round
		if( vMin.x <= vOMax.x && vMin.y <= vOMax.y && vMin.z <= vOMax.z &&
			vMax.x >= vOMin.x && vMax.y >= vOMin.y && vMax.z >= vOMin.z )
		{
			return true;
		}

		return false;
	}
}