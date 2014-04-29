// ********************************************************************************************

//	SpeedPoint Basic Animation Bundle

// ********************************************************************************************

#pragma once
#include <SPrerequisites.h>
#include <Util\SPool.h>
#include <Abstract\IAnimationBundle.h>
#include "SBasicAnimationSequence.h"

namespace SpeedPoint
{
	// SpeedPoint Animation Bundle
	class S_API SBasicAnimationBundle : public IAnimationBundle
	{
	public:
		SPool<SBasicAnimationSequence*> plSequences;		
		ISolid*				pSolid;
		SP_ID				iRunningAnimationSequence;
		SString				cName;

		// Default constructor
		SBasicAnimationBundle() : pSolid( NULL ) {}

		SResult Initialize( ISolid* pSolid );

		SResult Tick( float fTimeFactor );

		void SetName( const SString& name );
		SString* GetName( void );

		IAnimationJoint* AddJoint( SP_ID* pID );
		IAnimationJoint* GetJoint( SP_ID id );		

		IAnimationSequence* AddAnimationSequence( SP_ID* pID );
		IAnimationSequence* GetAnimationSequence( SP_ID id );
		SResult RunAnimationSequence( SP_ID id );
		SP_ID GetRunningAnimationSequence( void );
		void Clear( void );
	};
}