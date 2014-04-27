// ********************************************************************************************

//	SpeedPoint Basic Animation Joint

// ********************************************************************************************

#pragma once
#include <SPrerequisites.h>
#include <Util\SPool.h>
#include <Abstract\SAnimationJoint.h>

namespace SpeedPoint
{
	// SpeedPoint Animation Joint
	class S_API SBasicAnimationJoint : public SAnimationJoint
	{
	public:
		SSolid*			pSolid;
		SPool<SP_UNIQUE>	plVertices;

		// --

		void SetSolid( SSolid* psol );
		void AssignVertex( SP_UNIQUE iVertex );
		void MoveVertices( const SVector3& vec );
		void Clear( void );
	};
}