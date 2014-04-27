// ********************************************************************************************

//	SpeedPoint AI Basic Bot

// ********************************************************************************************

#pragma once
#include <SPrerequisites.h>
#include <Abstract\SBot.h>

namespace SpeedPoint
{
	// SpeedPoint Bot
	class S_API SBasicBot : public SBot
	{
	public:
		SString	cName;
		float	fInteractionRadius;

		// --

		virtual SResult InitScriptAttributes( void );
		void InitScriptEnvironment( RunPowderEnvironment* pEnv );		
		
		void SetInteractionRadius( float fRadius );
		float GetInteractionRadius( void );

		void SetName( const SString& src );
		SString* GetName( void );
	};
}