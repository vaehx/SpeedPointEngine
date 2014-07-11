// ********************************************************************************************

//	SpeedPoint HLSL Shader Effect

//	This is a virtual class. You cannot instantiate it.
//	Please use Specific Implementation to instantiate.

// ********************************************************************************************

#pragma once

#include <SPrerequisites.h>

SP_NMSPACE_BEG

class S_API SpeedPointEngine;





// SpeedPoint HLSL Shader Effect (abstract)
//~~~~~~~~~~~~~~
// TODO: Refactor this class to IRendererEffect. See implementations also!


class S_API IShader

	
//~~~~~~~~~~~~~~~
{
public:
	// Loads the shader from the file and compiles it
	// It will also select the first available technique as the default
	virtual SResult Initialize( SpeedPointEngine* pEngine, char* cFilename ) = 0;

	// Check if the shader is initialized
	virtual bool IsInitialized() = 0;

	// Setup the current technique to be used
	virtual SResult SetTechnique( char* cTechnique ) = 0;

	// Summary:
	//	 Enable this shader
	// Description:
	//	Will set the VS and PS of this effect as currently active ones
	virtual SResult Enable() = 0;

	// Apply this shader to the current state of the render pipeline
	//virtual SResult ApplyToRenderPipeline( IRenderPipeline* pPipeline ) = 0;	

	// Clearout this shader
	virtual SResult Clear( void ) = 0;
};

SP_NMSPACE_END