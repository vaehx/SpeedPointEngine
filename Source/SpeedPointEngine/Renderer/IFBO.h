// **************************************************************************************

//	SpeedPoint FrameBuffer Object

//	This is a virtual class. You cannot instantiate it.
//	Please use Specific Implementation to instantiate.

// **************************************************************************************

#pragma once

#include <Common\SPrerequisites.h>

SP_NMSPACE_BEG

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
	//	allowAsTexture - allows the FBO to be used as a texture
	virtual SResult Initialize(EFBOType type, IRenderer* pRenderer, unsigned int width, unsigned int height, bool allowAsTexture = false) = 0;

	// Summary:
	//	Creates a hardware depth buffer for this FBO
	// Arguments:
	//	allowAsTexture - allows the depth buffer to be used as a texture
	virtual SResult InitializeDepthBuffer(bool allowAsTexture = false) = 0;

	virtual bool IsInitialized() const = 0;

	// Get the handling renderer
	virtual IRenderer* GetRenderer() = 0;

	// Clear buffers
	virtual void Clear(void) = 0;		
};


typedef struct S_API IFBO IFrameBufferObject;


SP_NMSPACE_END