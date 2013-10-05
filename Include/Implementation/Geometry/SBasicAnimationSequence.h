// ********************************************************************************************

//	SpeedPoint Basic Animation Bundle

// ********************************************************************************************

#pragma once
#include <SPrerequisites.h>
#include <SPool.h>
#include <Abstract\SAnimationSequence.h>
#include "SBasicAnimationKey.h"

namespace SpeedPoint
{
	// SpeedPoint Basic Animation Sequence
	class S_API SBasicAnimationSequence : public SAnimationSequence
	{
	public:
		SPool<SBasicAnimationKey>	plKeys;
		SAnimationBundle*		pBundle;
		SString				cName;

		SResult Initialize( SAnimationBundle* bundle );
		SAnimationBundle* GetBundle( void );
		void SetName( const SString& name );
		SString* GetName( void );
		SP_ID AddKey( float fTimecode, SVector3 vPosition );
		SAnimationKey* GetKey( SP_ID id );
		void Clear( void );
	};
}