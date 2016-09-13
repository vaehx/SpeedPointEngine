// ********************************************************************************************

//	SpeedPoint Basic Animation Bundle

// ********************************************************************************************

#pragma once
#include <SPrerequisites.h>
#include <Util\SPool.h>
#include <Abstract\IAnimationSequence.h>
#include "AnimationKey.h"


SP_NMSPACE_BEG

struct S_API IAnimationBundle;

// SpeedPoint Basic Animation Sequence
class S_API AnimationSequence : public IAnimationSequence
{
public:
	SPool<AnimationKey>	plKeys;
	IAnimationBundle*		pBundle;
	SString				cName;

	SResult Initialize( IAnimationBundle* bundle );
	IAnimationBundle* GetBundle( void );
	void SetName( const SString& name );
	SString* GetName( void );
	SP_ID AddKey( float fTimecode, SVector3 vPosition );
	IAnimationKey* GetKey( SP_ID id );
	void Clear( void );
};


SP_NMSPACE_END