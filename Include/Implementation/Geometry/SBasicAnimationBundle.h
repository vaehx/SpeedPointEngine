// ********************************************************************************************

//	SpeedPoint Basic Animation Bundle

// ********************************************************************************************

#pragma once
#include <SPrerequisites.h>
#include <Util\SPool.h>
#include <Abstract\SAnimationBundle.h>
#include "SBasicAnimationSequence.h"

namespace SpeedPoint
{
	// SpeedPoint Animation Bundle
	class S_API SBasicAnimationBundle : public SAnimationBundle
	{
	public:
		SPool<SBasicAnimationSequence*> plSequences;		
		SSolid*				pSolid;
		SP_ID				iRunningAnimationSequence;
		SString				cName;

		// Default constructor
		SBasicAnimationBundle() : pSolid( NULL ) {}

		SResult Initialize( SSolid* pSolid );

		SResult Tick( float fTimeFactor );

		void SetName( const SString& name );
		SString* GetName( void );

		SAnimationJoint* AddJoint( SP_ID* pID );
		SAnimationJoint* GetJoint( SP_ID id );		

		SAnimationSequence* AddAnimationSequence( SP_ID* pID );
		SAnimationSequence* GetAnimationSequence( SP_ID id );
		SResult RunAnimationSequence( SP_ID id );
		SP_ID GetRunningAnimationSequence( void );
		void Clear( void );
	};
}