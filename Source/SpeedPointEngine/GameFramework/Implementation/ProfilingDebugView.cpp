//////////////////////////////////////////////////////////////////////////////////////////////////
//
//	SpeedPoint Game Engine
//	Copyright (c) 2011-2017 Pascal Rosenkranz, All rights reserved.
//
//////////////////////////////////////////////////////////////////////////////////////////////////

#include "ProfilingDebugView.h"
#include "SpeedPointEngine.h"
#include <Renderer\IRenderer.h>
#include <Common\ProfilingSystem.h>
#include <sstream>
#include <iomanip>
#include <limits>

using namespace std;

SP_NMSPACE_BEG

// Initializes only if it was not initialized yet
void InitFontRenderSlot(SFontRenderSlot** pSlot, bool alignRight, bool keep, const SColor& color,
	unsigned int x, unsigned int y, EFontSize fontSize = eFONTSIZE_NORMAL)
{
	if (!IS_VALID_PTR(pSlot))
		return;

	if (!IS_VALID_PTR(*pSlot))
	{
		*pSlot = SpeedPointEnv::GetEngine()->GetRenderer()->GetFontRenderSlot();
		SpeedPoint::SFontRenderSlot* pFRS = *pSlot;
		pFRS->keep = keep;
		pFRS->alignRight = alignRight;
		pFRS->color = color;
		pFRS->screenPos[0] = x;
		pFRS->screenPos[1] = y;
		pFRS->text = "";
		pFRS->fontSize = fontSize;
	}
}

//////////////////////////////////////////////////////////////////////////////////////////////////

S_API ProfilingDebugView::ProfilingDebugView()
	: m_pCamStatus(0),
	m_pFPS(0)
{
}

S_API void ProfilingDebugView::Update(IRenderer* renderer)
{
	static stringstream ss;
	ss << setprecision(3);

	ProfilingSystemIntrnl* profilingSystem = ProfilingSystem::Get();

	// Cam status
	ss.str("");
	InitFontRenderSlot(&m_pCamStatus, true, true, SColor(1.0f, 1.0f, 1.0f), 0, 0);
	SCamera* camera = renderer->GetTargetViewport()->GetCamera();
	if (IS_VALID_PTR(camera))
		ss << "CAM: pos=" << camera->position << " fwd=" << camera->GetForward();
	else
		ss << "CAM: undefined";

	m_pCamStatus->text = ss.str();

	// FPS
	ss.str("");
	InitFontRenderSlot(&m_pFPS, true, true, SColor(1.0f, 1.0f, 0.3f), 0, 37, eFONTSIZE_MEDIUM);
	ss	<< "FPS " << (1. / profilingSystem->GetLastFrameDuration())
		<< " (" << (1. / profilingSystem->GetMaxRecentFrameDuration())
		<< ".." << (1. / profilingSystem->GetMinRecentFrameDuration()) << ")";
	m_pFPS->text = ss.str();

	// Terrain info
	InitFontRenderSlot(&m_pTerrain, true, true, SpeedPoint::SColor(1.f, 1.f, 1.f), 0, 65);
	SpeedPoint::STerrainRenderDesc* pTerrainRenderDesc = renderer->GetTerrainRenderDesc();
	ss.str("");
	ss << "Terrain: " << pTerrainRenderDesc->nDrawCallDescs << " DCs";
	m_pTerrain->text = ss.str();

	// Profiling sections
	ss << std::fixed;
	SFontRenderSlot* frs = 0;
	unsigned int line = 0;
	const vector<ProfilingSection>* sections = profilingSystem->GetLastFrameSections();
	for (auto itSection = sections->begin(); itSection != sections->end(); ++itSection, ++line)
	{
		frs = 0;
		InitFontRenderSlot(&frs, false, false, SColor(1.f, 1.f, 1.f), 10, line * 13);
		ss.str("");
		ss << itSection->timer.GetDuration() * 1000.0 << "ms    ";
		for (int i = 0; i < itSection->level; ++i)
			ss << "    ";

		ss << itSection->name;
		frs->text = ss.str();
	}
}

SP_NMSPACE_END
