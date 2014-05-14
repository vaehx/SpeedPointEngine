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
		eFBO_GBUFFER_ALBEDO,
		eFBO_GBUFFER_NORMALS,
		eFBO_GBUFFER_TANGENTS,
		eFBO_GBUFFER_POSITION,	// world positions

		// Lighting buffer
		eFBO_LIGHT,

		eFBO_BACKBUFFER
	};

	// SpeedPoint FrameBufferObject (abstract)
	class S_API IFBO
	{
	public:
		// initialize with given engine
		virtual SResult Initialize(EFBOType type, SSpeedPointEngine* pEngine, IRenderer* pRenderer, unsigned int nW, unsigned int nH) = 0;
		virtual bool IsInitialized() = 0;

		// Get the handling engine
		virtual SSpeedPointEngine* GetEngine() = 0;

		// Get the handling renderer
		virtual IRenderer* GetRenderer() = 0;

		// Clear buffers
		virtual void Clear(void) = 0;		

		////// TODO - WHAT IS TODO?
	};
	typedef class S_API IFBO IFrameBufferObject;
}