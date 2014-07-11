// ********************************************************************************************

//	SpeedPoint Basic Animation Bundle

// ********************************************************************************************

#pragma once
#include <SPrerequisites.h>
#include <Util\SPool.h>
#include <Abstract\IAnimationBundle.h>
#include "AnimationSequence.h"


SP_NMSPACE_BEG

struct S_API ISolid;
struct S_API IAnimationJoint;


// SpeedPoint Animation Bundle
class S_API AnimationBundle : public IAnimationBundle
{
public:
	SPool<AnimationSequence*> plSequences;		
	ISolid*				pSolid;
	SP_ID				iRunningAnimationSequence;
	SString				cName;

	// Default constructor
	AnimationBundle() : pSolid( NULL ) {}

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


SP_NMSPACE_END