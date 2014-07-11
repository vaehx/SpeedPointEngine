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
	struct S_API IRendererSettings
	{
		// Set the renderer belonging to these settings
		virtual SResult SetRenderer( IRenderer* pRenderer ) = 0;

		// --
		
		virtual SResult SetAlphaColorKey( SColor colKey ) = 0;
		virtual SColor& GetAlphaColorKey() = 0;
		
		virtual SResult SetMSAACount( EMSAACount type ) = 0;
		virtual EMSAACount& GetMSAACount() = 0;
		
		virtual SResult SetMSAAQuality( EMSAAQuality quality ) = 0;
		virtual EMSAAQuality& GetMSAAQuality() = 0;
		
		virtual SResult SetProcessingMode( S_PROCESSING_MODE mode ) = 0;
		virtual S_PROCESSING_MODE& GetProcessingMode() = 0;

		virtual void SetClearColor(const SColor& clearColor) = 0;
		virtual SColor& GetClearColor() = 0;
	};
}