// ********************************************************************************************

//	SpeedPoint DirectX9 Texture Resource

// ********************************************************************************************

#pragma once

#include <Abstract\STexture.h>
#include <d3d9.h>

namespace SpeedPoint
{
	// SpeedPoint DirectX9 Texture Resource
	class S_API SDirectX9Texture : public STexture
	{
	public:
		SString			sSpecification;
		SpeedPointEngine*	pEngine;
		LPDIRECT3DTEXTURE9	pTexture;
		S_TEXTURE_TYPE		tType;
		bool			bDynamic;

		// Default constructor
		SDirectX9Texture() : pEngine( NULL ), pTexture( NULL ) {};

		// Copy constructor
		SDirectX9Texture( const SDirectX9Texture& o ) : pEngine( o.pEngine ), pTexture( o.pTexture ) {};


		// -- Initialize --

		// Initialize the texture
		SResult Initialize( SpeedPointEngine* pEngine, const SString& spec );

		// Initialize the texture with extended params
		SResult Initialize( SpeedPointEngine* pEngine, const SString& spec, bool bDyn );

		// Load a texture from file
		SResult LoadFromFile( int w, int h, int mipLevels, char* cFileName );

		// Initialize an empty texture with specified size and type
		SResult CreateEmpty( int w, int h, int mipLevels, S_TEXTURE_TYPE type, SColor clearcolor );

		// ----

		// Get the specification
		SString GetSpecification( void );

		// Get the type of this texture
		S_TEXTURE_TYPE GetType( void );

		// Get the size of this texture
		SResult GetSize( int* pW, int* pH );

		// Clear texture buffer
		SResult Clear( void );
	};
}