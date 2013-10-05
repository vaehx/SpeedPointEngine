// ********************************************************************************************

//	SpeedPoint HLSL Shader Effect

//	This is a virtual class. You cannot instantiate it.
//	Please use Specific Implementation to instantiate.

// ********************************************************************************************

#pragma once

#include <SPrerequisites.h>

namespace SpeedPoint
{
	// SpeedPoint HLSL Shader Effect (abstract)
	class S_API SShader
	{
	public:
		// Loads the shader from the file and compiles it
		// It will also select the first available technique as the default
		virtual SResult Initialize( SpeedPointEngine* pEngine, char* cFilename ) = 0;

		// Setup the current technique to be used
		virtual SResult SetTechnique( char* cTechnique ) = 0;

		// Apply this shader to the current state of the render pipeline
		//virtual SResult ApplyToRenderPipeline( SRenderPipeline* pPipeline ) = 0;	

		// Clearout this shader
		virtual SResult Clear( void ) = 0;
	};
}