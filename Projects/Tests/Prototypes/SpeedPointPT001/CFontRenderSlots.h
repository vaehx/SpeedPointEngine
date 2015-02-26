
#include <SpeedPoint.h>

#pragma once

class CFontRenderSlots
{
public:
	SpeedPoint::IGameEngine* m_pEngine;
	SpeedPoint::SFontRenderSlot* m_pCamStats;
	SpeedPoint::SFontRenderSlot* m_pFPS;

	void UpdateCamStats(SpeedPoint::SCamera* pCamera);
	void UpdateFPS(double fps);
};
