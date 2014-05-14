//////////////////////////////////////////////////////////////////////////////////////////////////
//
//	This file is part of the SpeedPoint Game Engine
//
//	written by Pascal R. aka iSmokiieZz
//	(c) 2011-2014, All rights reserved.
//
//////////////////////////////////////////////////////////////////////////////////////////////////

#include <Abstract\IRenderer.h>
#include <Implementation\DirectX11\DirectX11Settings.h>

namespace SpeedPoint
{	
	S_API SResult DirectX11Settings::SetRenderer(IRenderer* pRenderer)
	{
		if (pRenderer == 0 || pRenderer->GetType() != S_DIRECTX11)
			return S_INVALIDPARAM;

		m_pDXRenderer = (DirectX11Renderer*)pRenderer;
		return S_SUCCESS;
	}

	S_API SResult DirectX11Settings::SetAlphaColorKey(SColor colKey)
	{
		m_AlphaColorKey = colKey;

		return S_SUCCESS;
	}

	S_API SColor& DirectX11Settings::GetAlphaColorKey()
	{
		return m_AlphaColorKey;
	}

	S_API SResult DirectX11Settings::SetMultiSampleType(S_MULTISAMPLE_TYPE type)
	{
		m_MSType = type;




		// TODO: UPDATE DEVICE SETTINGS!




		return S_SUCCESS;
	}

	S_API S_MULTISAMPLE_TYPE& DirectX11Settings::GetMultiSampleType()
	{
		return m_MSType;
	}
	
	S_API SResult DirectX11Settings::SetMultiSampleQuality(S_MULTISAMPLE_QUALITY quality)
	{
		m_MSQuality = quality;



		// TODO: UPDATE DEVICE SETTINGS!



		return S_SUCCESS;
	}

	S_API S_MULTISAMPLE_QUALITY& DirectX11Settings::GetMultiSampleQuality()
	{
		return m_MSQuality;
	}

	S_API SResult DirectX11Settings::SetProcessingMode(S_PROCESSING_MODE mode)
	{
		m_ProcessingMode = mode;


		// TODO: UPDATE DEVICE SETTINGS!



		return S_SUCCESS;
	}

	S_API S_PROCESSING_MODE& DirectX11Settings::GetProcessingMode()
	{
		return m_ProcessingMode;
	}
}