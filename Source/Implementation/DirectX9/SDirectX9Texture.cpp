// SpeedPoint DirectX9 Texture

#include <Implementation\DirectX9\SDirectX9Texture.h>
#include <SpeedPoint.h>
#include <Implementation\DirectX9\SDirectX9Renderer.h>

namespace SpeedPoint
{

	// **********************************************************************************

	S_API SResult SDirectX9Texture::Initialize( SpeedPointEngine* eng, const SString& spec )
	{
		return Initialize( eng, spec, false );
	}

	// **********************************************************************************

	S_API SResult SDirectX9Texture::Initialize( SpeedPointEngine* eng, const SString& spec, bool bDyn )
	{
		if( eng == NULL ) return S_ABORTED;

		Clear();

		pEngine = eng;
		bDynamic = bDyn;
		sSpecification = spec;

		return S_SUCCESS;
	}

	// **********************************************************************************

	S_API SResult SDirectX9Texture::LoadFromFile( int w, int h, int mipLevels, char* cFileName )
	{
		if( pEngine == NULL ) return S_ABORTED;

		// We need the renderer to create the texture
		SDirectX9Renderer* pDXRenderer = (SDirectX9Renderer*)pEngine->GetRenderer();

		tType = S_TEXTURE_FILE;

		// Now create the texture
		if( FAILED( D3DXCreateTextureFromFileEx( 
			pDXRenderer->pd3dDevice,
			cFileName,
			w, h, 5,
			( bDynamic ) ? D3DUSAGE_DYNAMIC : 0,
			D3DFMT_UNKNOWN,
			( bDynamic ) ? D3DPOOL_MANAGED : D3DPOOL_DEFAULT,
			D3DX_FILTER_NONE,
			D3DX_DEFAULT,
			0,
			NULL,
			NULL,
			&pTexture ) ) )			
		{
			return S_ERROR;
		}

		// Set the mipmap-auto generation filter
////// TODO: Check device capabilities here to check for the best filter
		pTexture->SetAutoGenFilterType( D3DTEXF_ANISOTROPIC );		

		return S_SUCCESS;
	}

	// **********************************************************************************

	S_API SResult SDirectX9Texture::CreateEmpty( int w, int h, int mipLevels, S_TEXTURE_TYPE type, SColor clearcolor )
	{
		if( pEngine == NULL ) return S_ABORTED;

		// We need the renderer to create the texture
		SDirectX9Renderer* pDXRenderer = (SDirectX9Renderer*)pEngine->GetRenderer();

		tType = type;

		// Now create the texture
		if( FAILED( pDXRenderer->pd3dDevice->CreateTexture(
			w, h, mipLevels,
			D3DUSAGE_DYNAMIC,

/////// TODO: Check Device capabilites to create maximum bitrate (we now use 32bit)
			( type == S_TEXTURE_RGBA ) ? D3DFMT_A8B8G8R8 : D3DFMT_X8B8G8R8,

			D3DPOOL_DEFAULT,
			&pTexture,
			NULL ) ) )
		{
			return S_ERROR;
		}		

		// Now fill the texture
		D3DLOCKED_RECT lRect;
		if( SUCCEEDED( pTexture->LockRect( 0, &lRect, NULL, D3DLOCK_DISCARD ) ) )
		{
			DWORD* data = (DWORD*)lRect.pBits;

			// Get max size
			D3DSURFACE_DESC dsc;
			pTexture->GetLevelDesc( 0, &dsc );

			for( int xy=0; xy < (int)( dsc.Height * dsc.Width ); xy++ )
				data[xy] = D3DXCOLOR( clearcolor.r, clearcolor.g, clearcolor.b, clearcolor.a );

			pTexture->UnlockRect( 0 );			
		}
		else
		{
			return S_ERROR;
		}

		return S_SUCCESS;
	}

	// **********************************************************************************

	S_API S_TEXTURE_TYPE SDirectX9Texture::GetType( void )
	{
		return tType;
	}

	// **********************************************************************************

	S_API SResult SDirectX9Texture::GetSize( int* pW, int* pH )
	{
		if( pTexture == NULL ) return S_ABORTED;

		D3DSURFACE_DESC dscSurface;
		if( FAILED( pTexture->GetLevelDesc( 0, &dscSurface ) ) )
		{
			return S_ERROR;
		}

		if( pW != NULL ) *pW = (int)dscSurface.Width;
		if( pH != NULL ) *pH = (int)dscSurface.Height;

		return S_SUCCESS;
	}

	// **********************************************************************************

	S_API SString SDirectX9Texture::GetSpecification( void )
	{
		return sSpecification;
	}

	// **********************************************************************************

	S_API SResult SDirectX9Texture::Clear( void )
	{
		if( pTexture ) pTexture->Release(); pTexture = NULL;
		bDynamic = false;

		return S_SUCCESS;
	}

}