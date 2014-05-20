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
//#include <Implementation\DirectX11\DirectX11Renderer.h>


SP_NMSPACE_BEG



// -------------------------------------------------------------------------
DirectX11Settings::DirectX11Settings()
{
	m_pDXRenderer = 0;	
	m_MSAACount = eMSAA_NONE;
	m_MSAAQuality = eMSAAQUALITY_LOW;	
}

// -------------------------------------------------------------------------
DirectX11Settings::~DirectX11Settings()
{
}

// -------------------------------------------------------------------------
S_API SResult DirectX11Settings::SetRenderer(IRenderer* pRenderer)
{
	if (pRenderer == 0 || pRenderer->GetType() != S_DIRECTX11)
		return S_INVALIDPARAM;

	m_pDXRenderer = (DirectX11Renderer*)pRenderer;
	return S_SUCCESS;
}


// -------------------------------------------------------------------------
S_API SResult DirectX11Settings::SetAlphaColorKey(SColor colKey)
{
	m_AlphaColorKey = colKey;

	return S_SUCCESS;
}
S_API SColor& DirectX11Settings::GetAlphaColorKey()
{
	return m_AlphaColorKey;
}


// -------------------------------------------------------------------------
S_API SResult DirectX11Settings::SetMSAACount(EMSAACount count)
{
	m_MSAACount = count;




	// TODO: UPDATE DEVICE SETTINGS!




	return S_SUCCESS;
}
S_API EMSAACount& DirectX11Settings::GetMSAACount()
{
	return m_MSAACount;
}

	
// -------------------------------------------------------------------------
S_API SResult DirectX11Settings::SetMSAAQuality(EMSAAQuality quality)
{
	m_MSAAQuality = quality;



	// TODO: UPDATE DEVICE SETTINGS!



	return S_SUCCESS;
}
S_API EMSAAQuality& DirectX11Settings::GetMSAAQuality()
{
	return m_MSAAQuality;
}


// -------------------------------------------------------------------------
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


// -------------------------------------------------------------------------
S_API void DirectX11Settings::SetClearColor(const SColor& clearColor)
{
	SPGetColorFloatArray(m_ClearColor, clearColor);
	m_clClearColor = clearColor;
}
S_API SColor& DirectX11Settings::GetClearColor()
{
	return m_clClearColor;
}


SP_NMSPACE_END