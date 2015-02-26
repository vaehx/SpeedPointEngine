#include "CDebugInfo.h"

// -----------------------------------------------------------------------------
void CDebugInfo::InitFontRenderSlot(SpeedPoint::SFontRenderSlot** ppFRS,
	bool bRightAlign, bool keep, const SpeedPoint::SColor& color, unsigned int x, unsigned int y)
{
	if (!IS_VALID_PTR(ppFRS))
		return;

	if (!IS_VALID_PTR(*ppFRS))
	{
		*ppFRS = m_pEngine->GetRenderer()->GetFontRenderSlot();
		SpeedPoint::SFontRenderSlot* pFRS = *ppFRS;
		pFRS->keep = keep;
		pFRS->alignRight = bRightAlign;
		pFRS->color = color;
		pFRS->screenPos[0] = x;
		pFRS->screenPos[1] = y;
		pFRS->text = new char[200];
	}
}

// -----------------------------------------------------------------------------
void CDebugInfo::Update(SpeedPoint::SCamera* pCamera, double fps, const SFrameDebugInfo& fdi)
{
	// CamStat
	InitFontRenderSlot(&m_pCamStats, true, true, SpeedPoint::SColor(1.0f, 1.0f, 1.0f), 0, 0);		
	SpeedPoint::SPSPrintf(m_pCamStats->text, 200, "cam(%.2f %.2f %.f | %.2f %.2f %.2f)",
		pCamera->position.x, pCamera->position.y, pCamera->position.z,
		pCamera->rotation.x, pCamera->rotation.y, pCamera->rotation.z);

	// FPS
	InitFontRenderSlot(&m_pFPS, true, true, SpeedPoint::SColor(1.0f, 1.0f, 0.3f), 0, 18);		
	SpeedPoint::SPSPrintf(m_pFPS->text, 200, "FPS %.3f", fps);

	// Frame Timers
	InitFontRenderSlot(&m_pFrameTimes, true, true, SpeedPoint::SColor(1.0f, 1.0f, 1.0f), 0, 36);	
	SpeedPoint::SPSPrintf(m_pFrameTimes->text, 200, "Render: %.2f Tick: %.2f Frame: %.2f",
		fdi.renderTimer.GetDuration() * 1000.0, fdi.tickTimer.GetDuration() * 1000.0, fdi.frameTimer.GetDuration() * 1000.0);
}