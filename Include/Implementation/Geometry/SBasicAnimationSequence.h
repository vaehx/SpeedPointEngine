// ********************************************************************************************

//	SpeedPoint Basic Animation Bundle

// ********************************************************************************************

#pragma once
#include <SPrerequisites.h>
#include <Util\SPool.h>
#include <Abstract\IAnimationSequence.h>
#include "SBasicAnimationKey.h"

namespace SpeedPoint
{
	// SpeedPoint Basic Animation Sequence
	class S_API SBasicAnimationSequence : public IAnimationSequence
	{
	public:
		SPool<SBasicAnimationKey>	plKeys;
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
}