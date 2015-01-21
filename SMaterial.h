// ********************************************************************************************

//	SpeedPoint Material that describes the geometry only

// ********************************************************************************************

#pragma once

#include <SPrerequisites.h>
#include "SColor.h"

namespace SpeedPoint
{

	// SpeedPoint Geometry-Material
	struct S_API SMaterial
	{
		SColor		colDiffuse;
		SP_UNIQUE	iTexture;
		float		fEmissivePower;
		float		fShininess;		

		// ---

		// Standard Constructor
		SMaterial() 
			: colDiffuse( SColor( 0.0f, 0.0f, 0.0f, 0.0f ) ),
			iTexture( 0 ),
			fEmissivePower( 0 ),
			fShininess( 0 ) {}

		// Copy constructor
		SMaterial( const SMaterial& mat )
			: colDiffuse( mat.colDiffuse ),
			iTexture( mat.iTexture ),
			fEmissivePower( mat.fEmissivePower ),
			fShininess( mat.fShininess ) {}
	};

}