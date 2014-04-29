// ********************************************************************************************

//	SpeedPoint AI Basic Bot

// ********************************************************************************************

#pragma once
#include <SPrerequisites.h>
#include <Abstract\IBot.h>

namespace SpeedPoint
{
	// SpeedPoint Bot
	class S_API SBasicBot : public IBot
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