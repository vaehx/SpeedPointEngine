// **************************************************************************************

//	SpeedPoint FrameBuffer Object

//	This is a virtual class. You cannot instantiate it.
//	Please use Specific Implementation to instantiate.

// **************************************************************************************

#pragma once

#include <SPrerequisites.h>

namespace SpeedPoint
{
	enum S_API EFBOType
	{
		// G-Buffer FBOs. See documentation for mapping of these values
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
	};



	// SpeedPoint FrameBufferObject (abstract)
	class S_API IFBO
	{
	public:
		// Summary:
		//	Initialize with given renderer
		// Arguments:
		//	nW / nH - (default 0) resolution of the buffer. set to 0 or omit to use FBOType-Default
		virtual SResult Initialize(EFBOType type, SSpeedPointEngine* pEngine, IRenderer* pRenderer, unsigned int nW = 0, unsigned int nH = 0) = 0;

		// Initialize 
		//	Initialize the Depth Stencil Buffer and its View
		// Return value:
		//	S_NOTINIT - Initialize() was not called or not successful
		virtual SResult InitializeDSV() = 0;

		virtual bool IsInitialized() = 0;

		// Get the handling engine
		virtual SSpeedPointEngine* GetEngine() = 0;

		// Get the handling renderer
		virtual IRenderer* GetRenderer() = 0;

		// Clear buffers
		virtual void Clear(void) = 0;		
	};


	typedef class S_API IFBO IFrameBufferObject;
}