//////////////////////////////////////////////////////////////////////////////////////////////////
//
//	This file is part of the SpeedPoint Game Engine
//
//	written by Pascal R. aka iSmokiieZz
//	(c) 2011-2014, All rights reserved.
//
//////////////////////////////////////////////////////////////////////////////////////////////////

#pragma once

#include <SPrerequisites.h>
#include "DirectX11.h"
#include <Abstract\ITexture.h>

SP_NMSPACE_BEG

class S_API SpeedPointEngine;
class S_API IRenderer;

class S_API DirectX11Texture : ITexture
{
private:
	SString			m_Specification;
	SpeedPointEngine*	m_pEngine;
	S_TEXTURE_TYPE		m_Type;
	bool			m_bDynamic;
	ID3D11Texture2D*	m_pDXTexture;
	DirectX11Renderer*	m_pDXRenderer;

public:		
	DirectX11Texture();
	DirectX11Texture(const DirectX11Texture& o);

	~DirectX11Texture();




	// -- Initialize --

	// Initialize the texture
	virtual SResult Initialize(SpeedPointEngine* pEngine, const SString& spec);

	// Initialize the texture with extended params
	virtual SResult Initialize(SpeedPointEngine* pEngine, const SString& spec, bool bDynamic);

	// Load a texture from file
	virtual SResult LoadFromFile(int w, int h, int mipLevels, char* cFileName);

	// Initialize an empty texture with specified size and type
	virtual SResult CreateEmpty(int w, int h, int mipLevels, S_TEXTURE_TYPE type, SColor clearcolor);

	// Get the specification
	virtual SString GetSpecification(void);

	// ----

	// Get the type of this texture
	virtual S_TEXTURE_TYPE GetType(void);

	// Get the size of this texture
	virtual SResult GetSize(int* pW, int* pH);

	// Clear texture buffer
	virtual SResult Clear(void);
};

SP_NMSPACE_END