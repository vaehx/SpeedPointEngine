#include "CFontRenderSlots.h"

// -----------------------------------------------------------------------------
void CFontRenderSlots::UpdateCamStats(SpeedPoint::SCamera* pCamera)
{
	if (!IS_VALID_PTR(m_pCamStats))
	{
		m_pCamStats = m_pEngine->GetRenderer()->GetFontRenderSlot();
		if (IS_VALID_PTR(m_pCamStats))
		{
			m_pCamStats->keep = true;
			m_pCamStats->alignRight = true;
			m_pCamStats->color = SpeedPoint::SColor(1.0f, 1.0f, 1.0f);
		}
	}

	// update the text
	m_pCamStats->text = new char[200];
	ZeroMemory(m_pCamStats->text, 200);
	SpeedPoint::SPSPrintf(m_pCamStats->text, 200, "cam(%.2f %.2f %.f | %.2f %.2f %.2f)",
		pCamera->position.x, pCamera->position.y, pCamera->position.z,
		pCamera->rotation.x, pCamera->rotation.y, pCamera->rotation.z);
}

// -----------------------------------------------------------------------------
void CFontRenderSlots::UpdateFPS(double fps)
{
	if (!IS_VALID_PTR(m_pFPS))
	{
		m_pFPS = m_pEngine->GetRenderer()->GetFontRenderSlot();
		if (IS_VALID_PTR(m_pFPS))
		{
			m_pFPS->keep = true;
			m_pFPS->alignRight = true;
			m_pFPS->color = SpeedPoint::SColor(1.0f, 1.0f, 0.2f);
			m_pFPS->screenPos[1] = 18;
		}
	}

	// update the text
	m_pFPS->text = new char[200];
	ZeroMemory(m_pFPS->text, 200);
	SpeedPoint::SPSPrintf(m_pFPS->text, 100, "FPS %.3f", fps);
}