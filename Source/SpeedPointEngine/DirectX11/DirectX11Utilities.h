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
#include "DirectX11.h"
#include <Abstract\ISettings.h>

SP_NMSPACE_BEG

// DirectX11 Utilities and helper functions

// -------------------------------------------------------------------------
static inline S_API DXGI_SAMPLE_DESC GetD3D11MSAADesc(
	DXGI_FORMAT backBufferFormat,
	ID3D11Device* pD3D11Device,
	const EMSAACount& count,
	const EMSAAQuality& quality)
{	
	DXGI_SAMPLE_DESC out;
	SP_ASSERT(pD3D11Device);
	if (!pD3D11Device)
	{
		// disable MSAA
		out.Count = 1;
		out.Quality = 0;
		return out;
	}

	out.Count = (unsigned int)count;

	unsigned int maxSampleQuality;
	if (Failure(pD3D11Device->CheckMultisampleQualityLevels(backBufferFormat, out.Count, &maxSampleQuality)))
	{
		// disable MSAA
		out.Count = 1;
		out.Quality = 0;
		return out;
	}

	// for the quality we divide the maximum (decreased by 1) by the count of available steps (3 for LOW, MID, HIGH)	
	unsigned int nQualityStep;
	nQualityStep = (unsigned int)((maxSampleQuality - 1) / 3);

	switch (quality)
	{
	case eMSAAQUALITY_LOW:
		out.Quality = nQualityStep;
		break;

	case eMSAAQUALITY_MEDIUM:
		out.Quality = 2 * nQualityStep;
		break;

	case eMSAAQUALITY_HIGH:
		out.Quality = maxSampleQuality - 1;	// due to rounding mistakes, we dont use nQualityStep here
		break;

	default:
		// disable MSAA
		out.Quality = 0;
		out.Count = 1;
	}

	return out;

}

// -------------------------------------------------------------------------


SP_NMSPACE_END