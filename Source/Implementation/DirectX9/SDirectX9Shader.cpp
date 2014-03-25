// SpeedPoint DirectX9 Shader

#include <Implementation\DirectX9\SDirectX9Shader.h>
#include <Implementation\DirectX9\SDirectX9Renderer.h>
#include <SpeedPoint.h>

namespace SpeedPoint
{
	// **********************************************************************************

	S_API SResult SDirectX9Shader::Initialize( SpeedPointEngine* engine, char* cFilename )
	{
		Clear();

		if( engine == NULL || cFilename == NULL ) return S_ABORTED;

		pEngine = engine;
		SDirectX9Renderer* pDXRenderer = (SDirectX9Renderer*)pEngine->GetRenderer();

		// Create Effect
//~~~~~~~~~~~~
// TODO: Setup custom compilation and catch errors and throw them into the Logging Stream!
		if( FAILED( D3DXCreateEffectFromFile( pDXRenderer->pd3dDevice, cFilename, NULL, NULL, 0, NULL, &pEffect, NULL ) ) )
		{
			return S_ERROR;
		}
//~~~~~~~~~~~~

		// Set first technique we can find
		if( FAILED( pEffect->SetTechnique( pEffect->GetTechnique( 0 ) ) ) )
		{
			return S_ERROR;
		}

		return S_SUCCESS;
	}

	// **********************************************************************************

	S_API SResult SDirectX9Shader::SetTechnique( char* cTechnique )
	{
		if( pEffect == NULL ) return S_ABORTED;

		if( FAILED( pEffect->SetTechnique( cTechnique ) ) )
		{
			return S_ERROR;
		}

		return S_SUCCESS;
	}

	// **********************************************************************************

	
	S_API SResult SDirectX9Shader::Clear( void )
	{
		if( pEffect ) pEffect->Release(); pEffect = NULL;
		pEngine = NULL;		

		return S_SUCCESS;
	}

}