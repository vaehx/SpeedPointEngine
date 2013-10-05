// ********************************************************************************************

//	SpeedPoint DirectX9 Shader

// ********************************************************************************************

#pragma once
#include <SPrerequisites.h>
#include <Abstract\SShader.h>
#include <d3dx9.h>

namespace SpeedPoint
{
	// SpeedPoint DirectX9 Shader
	class S_API SDirectX9Shader : public SShader
	{
	public:
		SpeedPointEngine*	pEngine;
		LPD3DXEFFECT		pEffect;

		// Default constructor
		SDirectX9Shader() : pEngine( NULL ), pEffect( NULL ) {};

		// Copy constructor
		SDirectX9Shader( const SDirectX9Shader& o ) : pEngine( o.pEngine ), pEffect( o.pEffect ) {};

		// Initialize this shader with engine and load shader data from given file
		// It will also compile the shader to HLSL
		SResult Initialize( SpeedPointEngine* pEngine, char* cFilename );

		// Set current technique
		SResult SetTechnique( char* cTechnique );

		// Clearout this shader
		SResult Clear( void );
	};
}