// ********************************************************************************************

//	SpeedPoint Basic Entity

// ********************************************************************************************

#pragma once
#include <SPrerequisites.h>
#include <Util\SString.h>
#include <Abstract\IEntity.h>

#define S_DEFAULT_INTERACTIONRADIUS 20.0f

namespace SpeedPoint
{
	// SpeedPoint Entity
	class S_API SBasicEntity : public IEntity
	{
	public:
		SString			cName;
		float			fInteractionRadius;
		RunPowderEnvironment*	pScriptEnvironment;

		// Default constructor
		SBasicEntity()
			: cName( NULL ),
			fInteractionRadius( S_DEFAULT_INTERACTIONRADIUS ),
			pScriptEnvironment( NULL ) {}

		// Copy Constructor
		SBasicEntity( const SBasicEntity& other )
			: fInteractionRadius( other.fInteractionRadius ),
			cName( other.cName ),
			pScriptEnvironment( other.pScriptEnvironment ) {}

		SResult InitScriptAttributes( void );
		void InitScriptEnvironment( RunPowderEnvironment* pEnv );		
		
		void SetInteractionRadius( float fRadius );
		float GetInteractionRadius( void );

		void SetName( const SString& src );
		SString* GetName( void );
	};
}