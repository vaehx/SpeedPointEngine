// SpeedPoint DirectX9 Frame Buffer source

#include <Implementation\DirectX9\SDirectX9FrameBuffer.h>
#include <Implementation\DirectX9\SDirectX9Renderer.h>
#include <SpeedPoint.h>

namespace SpeedPoint
{

	// **********************************************************************************

	S_API SResult SDirectX9FrameBuffer::Initialize( SpeedPointEngine* engine, int nW, int nH )
	{
		if( NULL == ( pEngine = engine ) )
		{
			( (SDirectX9Renderer*)pEngine->GetRenderer() )->pd3dDevice->CreateTexture(
				nW, nH, 1, D3DUSAGE_RENDERTARGET, D3DFMT_X8R8G8B8, D3DPOOL_DEFAULT, &pTexture, NULL );

			if( FAILED( pTexture->GetSurfaceLevel( 0, &pSurface ) ) )
				return S_ERROR;

			return S_SUCCESS;
		}
		
		return S_ERROR;
	}

	// **********************************************************************************

	S_API void SDirectX9FrameBuffer::Clear( void )
	{
		if( pTexture != NULL ) pTexture->Release();
		pSurface = NULL;
		pEngine = NULL;
	}

}