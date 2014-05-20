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


SP_NMSPACE_BEG



//////////////////////////////////////////////////////////////////////////////////////////////////

class DirectX11Renderer;

class S_API DirectX11Settings : public IRendererSettings
{
private:
	DirectX11Renderer* m_pDXRenderer;
	SColor m_AlphaColorKey;
	EMSAACount m_MSAACount;
	EMSAAQuality m_MSAAQuality;

	// Do we still need this in DX11 ?
	S_PROCESSING_MODE m_ProcessingMode;

	// We dont want to convert the clear color from SColor to th DX11 FLOAT array every frame.
	// So we put this into here
	float m_ClearColor[4];
	SColor m_clClearColor;


	DirectX11Settings();
	~DirectX11Settings();


public:	
	float* GetClearColorFloatArr()
	{
		return m_ClearColor;
	}



	// Set the renderer belonging to these settings
	virtual SResult SetRenderer(IRenderer* pRenderer);

	// --

	virtual SResult SetAlphaColorKey(SColor colKey);
	virtual SColor& GetAlphaColorKey();

	virtual SResult SetMSAACount(EMSAACount cnt);
	virtual EMSAACount& GetMSAACount();

	virtual SResult SetMSAAQuality(EMSAAQuality quality);
	virtual EMSAAQuality& GetMSAAQuality();

	virtual SResult SetProcessingMode(S_PROCESSING_MODE mode);
	virtual S_PROCESSING_MODE& GetProcessingMode();

	virtual void SetClearColor(const SColor& clearColor);
	virtual SColor& GetClearColor();
};



SP_NMSPACE_END