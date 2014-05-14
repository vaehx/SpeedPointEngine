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
#include <Util\SColor.h>
#include <Abstract\IRendererSettings.h>

namespace SpeedPoint
{
	class S_API DirectX11Settings : public IRendererSettings
	{
	private:
		DirectX11Renderer*	m_pDXRenderer;
		SColor			m_AlphaColorKey;
		S_MULTISAMPLE_TYPE	m_MSType;	// multisampletype
		S_MULTISAMPLE_QUALITY	m_MSQuality;	// multisamplequality
		S_PROCESSING_MODE	m_ProcessingMode;	// ?

	public:	
		// Set the renderer belonging to these settings
		virtual SResult SetRenderer(IRenderer* pRenderer);

		// --

		virtual SResult SetAlphaColorKey(SColor colKey);
		virtual SColor& GetAlphaColorKey();

		virtual SResult SetMultiSampleType(S_MULTISAMPLE_TYPE type);
		virtual S_MULTISAMPLE_TYPE& GetMultiSampleType();

		virtual SResult SetMultiSampleQuality(S_MULTISAMPLE_QUALITY quality);
		virtual S_MULTISAMPLE_QUALITY& GetMultiSampleQuality();

		virtual SResult SetProcessingMode(S_PROCESSING_MODE mode);
		virtual S_PROCESSING_MODE& GetProcessingMode();
	};
}