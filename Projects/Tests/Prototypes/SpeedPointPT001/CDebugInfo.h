
#include <SpeedPoint.h>
#include <chrono>

#pragma once

struct SFrameDebugTimer
{
	std::chrono::high_resolution_clock::time_point tp1, tp2;
	double dur;

	void Start()
	{
		tp1 = std::chrono::high_resolution_clock::now();
	}

	void Stop()
	{
		tp2 = std::chrono::high_resolution_clock::now();
		std::chrono::duration<double> d = tp2 - tp1;
		dur = d.count();
	}

	double GetDuration() const
	{
		return dur;
	}
};

struct SFrameDebugInfo
{
	SFrameDebugTimer frameTimer; // for the whole frame
	SFrameDebugTimer tickTimer; // for the tick function
	SFrameDebugTimer renderTimer; // for the render function
};

class CDebugInfo
{
public:
	SpeedPoint::IGameEngine* m_pEngine;
	SpeedPoint::SFontRenderSlot* m_pCamStats;
	SpeedPoint::SFontRenderSlot* m_pFPS;
	SpeedPoint::SFontRenderSlot* m_pFrameTimes;

	void Update(SpeedPoint::SCamera* pCamera, double fps, const SFrameDebugInfo& fdi);
	
	inline void InitFontRenderSlot(SpeedPoint::SFontRenderSlot** m_pFRS,
		bool bRightAlign, bool keep, const SpeedPoint::SColor& color, unsigned int x, unsigned int y);
};
