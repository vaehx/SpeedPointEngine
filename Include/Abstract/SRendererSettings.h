// ********************************************************************************************

//	SpeedPoint Renderer Settings

//	This is a virtual class. You cannot instantiate it.
//	Please use Specific Implementation to instantiate.

// ********************************************************************************************

#pragma once

#include <SPrerequisites.h>

namespace SpeedPoint
{
	// SpeedPoint Renderer Settings (abstract)
	class S_API IRendererSettings
	{
	public:
		// Set the renderer belonging to these settings
		virtual SResult SetRenderer( SRenderer* pRenderer ) = 0;

		// --

		// Set the colorkey for alpha textures
		virtual SResult SetAlphaColorKey( SColor colKey ) = 0;

		// Set the multi sampling type
		virtual SResult SetMultiSampleType( S_MULTISAMPLE_TYPE type ) = 0;

		// Set the multi sampling quality
		virtual SResult SetMultiSampleQuality( S_MULTISAMPLE_QUALITY quality ) = 0;

		// Set the processing mode
		virtual SResult SetProcessingMode( S_PROCESSING_MODE mode ) = 0;

		// -- Capabilities --

		////// TODO
	};
}