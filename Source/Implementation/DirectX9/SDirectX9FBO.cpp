// SpeedPoint DirectX9 Frame Buffer source

#include <Implementation\DirectX9\SDirectX9FBO.h>
#include <Implementation\DirectX9\SDirectX9Renderer.h>
#include <SpeedPoint.h>

namespace SpeedPoint
{

	// **********************************************************************************

	S_API SResult SDirectX9FrameBuffer::Initialize( SSpeedPointEngine* eng, IRenderer* pRenderer, int nW, int nH )
	{
		SP_ASSERTR( (pEngine = eng), S_INVALIDPARAM );
		SP_ASSERTR( pRenderer && pRenderer->GetType() == S_DIRECTX9, S_INVALIDPARAM );		

		if( FAILED( ( (SDirectX9Renderer*)pEngine->GetRenderer() )->pd3dDevice->CreateTexture(
			nW, nH,
			0,
			D3DUSAGE_RENDERTARGET,
			D3DFMT_X8R8G8B8,	// we probably want to change the format someday ;)
			D3DPOOL_DEFAULT,
			&pTexture,
			NULL ) ) )
		{
			return pEngine->LogReport( S_ERROR, "Failed Create FrameBuffer object" );
		}

		if( FAILED( pTexture->GetSurfaceLevel( 0, &pSurface ) ) )
		{
			return pEngine->LogReport( S_ERROR, "Failed Get zero-Surface level of recently created FrameBuffer Object" );
		}

		return S_SUCCESS;
	}

	// **********************************************************************************

	S_API bool SDirectX9FrameBuffer::IsInitialized()
	{
		return (pTexture && pSurface && pEngine);
	}

	// **********************************************************************************

	S_API void SDirectX9FrameBuffer::Clear( void )
	{
		if( pTexture != NULL )
			pTexture->Release();

		pSurface = NULL;
		pEngine = NULL;
	}

}