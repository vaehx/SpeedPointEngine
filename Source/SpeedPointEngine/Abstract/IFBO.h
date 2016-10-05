// **************************************************************************************

//	SpeedPoint FrameBuffer Object

//	This is a virtual class. You cannot instantiate it.
//	Please use Specific Implementation to instantiate.

// **************************************************************************************

#pragma once

#include "SPrerequisites.h"

SP_NMSPACE_BEG

struct S_API IGameEngine;
struct S_API IRenderer;


enum S_API EFBOType
{
	eFBO_R8G8B8A8,
	eFBO_F32 // 32bit Red Channel


/*	// G-Buffer FBOs. See documentation for mapping of these values
	eFBO_GBUFFER_ALBEDO,	// (deferred-shading)
	eFBO_GBUFFER_NORMALS,	// (deferred-shading)
	eFBO_GBUFFER_TANGENTS,	// (deferred-shading)
	eFBO_GBUFFER_POSITION,	// WS-pos (deferred-shading)

	eFBO_GBUFFER_SRT,	// single G-Buffer render target for deferred-rendering

	// Lighting buffer
	eFBO_LIGHT,		// (deferred-shading)
	eFBO_LIGHT_SPECULAR,	// (deferred-rendering)
	eFBO_LIGHT_DIFFUSE,	// (deferred-rendering)

	eFBO_BACKBUFFER
	*/
};



// SpeedPoint FrameBufferObject (abstract)
struct S_API IFBO
{
public:
	virtual ~IFBO()
	{
	}

	// Summary:
	//	Initialize with given renderer
	// Arguments:
	//	nW / nH - (default 0) resolution of the buffer. set to 0 or omit to use FBOType-Default
	virtual SResult Initialize(EFBOType type, IRenderer* pRenderer, unsigned int nW = 0, unsigned int nH = 0) = 0;		

	// Initialize 
	//	Initialize the Depth Stencil Buffer and its View
	// Return value:
	//	S_NOTINIT - Initialize() was not called or not successful
	virtual SResult InitializeDSV() = 0;

	// Summary:
	//	Mark this FBO to later be used as a texture, so generate the Shader resource view
	virtual SResult InitializeSRV() = 0;

	virtual bool IsInitialized() = 0;

	// Get the handling renderer
	virtual IRenderer* GetRenderer() = 0;

	// Clear buffers
	virtual void Clear(void) = 0;		
};


typedef struct S_API IFBO IFrameBufferObject;


SP_NMSPACE_END