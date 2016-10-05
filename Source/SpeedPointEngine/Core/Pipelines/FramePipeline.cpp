// ******************************************************************************************

// This file is part of the SpeedPoint Engine

// ******************************************************************************************

#include "FramePipeline.h"
#include "..\SpeedPointEngine.h"
#include <Abstract\IRenderPipeline.h>
#include <Abstract\IRenderer.h>
#include <Abstract\IApplication.h>
#include <Abstract\IPhysics.h>
#include <sstream>

using std::stringstream;

SP_NMSPACE_BEG

std::ostream& operator <<(std::ostream& ss, const Vec3f& v)
{
	ss << "(" << v.x << ", " << v.y << ", " << v.z << ")";
	return ss;
}
std::ostream& operator <<(std::ostream& ss, const Quat& q)
{
	ss << "(" << q.v.x << ", " << q.v.y << ", " << q.v.z << " w=" << q.w << ")";
	return ss;
};


S_API CDebugInfo::CDebugInfo()
: m_pCamStats(0),
m_pEngine(0),
m_pFPS(0),
m_pFrameTimes(0),
m_pTerrain(0)
{
}

// -----------------------------------------------------------------------------
S_API void CDebugInfo::InitFontRenderSlot(SFontRenderSlot** ppFRS, bool bRightAlign, bool keep,
	const SColor& color, unsigned int x, unsigned int y, EFontSize fontSize)
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
		pFRS->text = "";
		pFRS->fontSize = fontSize;
	}
}

// -----------------------------------------------------------------------------
S_API void CDebugInfo::Update(SpeedPoint::SCamera* pCamera, double fps, const SFrameDebugInfo& fdi)
{
	stringstream ss;

	// Cam stats
	InitFontRenderSlot(&m_pCamStats, true, true, SpeedPoint::SColor(1.0f, 1.0f, 1.0f), 0, 0);
	ss << "CAM: pos=" << pCamera->position << " fwd=" << pCamera->GetForward();
	m_pCamStats->text = ss.str();

	// Frame Timers
	InitFontRenderSlot(&m_pFrameTimes, true, true, SpeedPoint::SColor(1.0f, 1.0f, 1.0f), 0, 18);
	ss.str("");
	ss	<< "Render: " << fdi.renderTimer.GetDuration() * 1000.0 << "ms "
		<< "Tick: " << fdi.tickTimer.GetDuration() * 1000.0 << "ms "
		<< "Frame: " << fdi.frameTimer.GetDuration() * 1000.0 << "ms";
	m_pFrameTimes->text = ss.str();

	// FPS
	InitFontRenderSlot(&m_pFPS, true, true, SpeedPoint::SColor(1.0f, 1.0f, 0.3f), 0, 37, SpeedPoint::eFONTSIZE_MEDIUM);
	ss.str("");
	ss << "FPS " << fdi.lastFrameCounter << " (" << (unsigned int)(1.0 / fdi.lastMaxFrameTime) << ".." << (unsigned int)(1.0 / fdi.lastMinFrameTime);
	m_pFPS->text = ss.str();

	// Terrain info
	InitFontRenderSlot(&m_pTerrain, true, true, SpeedPoint::SColor(1.f, 1.f, 1.f), 0, 65);
	SpeedPoint::STerrainRenderDesc* pTerrainRenderDesc = m_pEngine->GetRenderer()->GetTerrainRenderDesc();
	ss.str("");
	ss << "Terrain: " << pTerrainRenderDesc->nDrawCallDescs << " DCs";
	m_pTerrain->text = ss.str();


	// Budget timers
	unsigned int i = 0;
	const vector<SFrameDebugTimer>& stagedBudgetTimers = fdi.GetStagedBudgetTimers();
	for (auto itBudgetTimer = stagedBudgetTimers.begin(); itBudgetTimer != stagedBudgetTimers.end(); ++itBudgetTimer, ++i)
	{
		SFontRenderSlot* pFRS = 0;
		InitFontRenderSlot(&pFRS, false, false, SColor(1.f, 1.f, 1.f), 10, 0 + i * 13);
		ss.str("");
		ss << itBudgetTimer->GetDuration() * 1000.0 << "\t" << itBudgetTimer->name;
		pFRS->text = ss.str();
	}
}

// -----------------------------------------------------------------------------
S_API void CDebugInfo::Clear()
{
	m_pCamStats = 0;
	m_pEngine = 0;
	m_pFPS = 0;
	m_pFrameTimes = 0;
	m_pTerrain = 0;	
}








///////////////////////////////////////////////////////////////////////////////////////////////////////////////////////////////
///////////////////////////////////////////////////////////////////////////////////////////////////////////////////////////////
///////////////////////////////////////////////////////////////////////////////////////////////////////////////////////////////






S_API FramePipeline::FramePipeline()
: m_pEngine(0),
m_bStartedFPSTimer(false),
m_dLastFPS(0),
m_fLastFPS(0),
m_nUsedFramepipelineSections(0),
m_pCurrentSection(nullptr)
{
}


// -------------------------------------------------------------------------------------
S_API void FramePipeline::Initialize(IGameEngine* pEngine)
{
	assert(IS_VALID_PTR(pEngine));
			
	m_pEngine = pEngine;
	m_bStartedFPSTimer = false;	

	// Initialize the timestamp pointers		
	m_pFrameBeginTimestamp = new std::chrono::time_point<std::chrono::high_resolution_clock>();
	m_pFrameEndTimestamp = new std::chrono::time_point<std::chrono::high_resolution_clock>();
	m_pFPSBeginTimestamp = new std::chrono::time_point<std::chrono::high_resolution_clock>();
	m_pdLastFrameDuration = new std::chrono::duration<double>();


	// Initialize debug info
	m_DebugInfo.m_pEngine = pEngine;	
}

// -------------------------------------------------------------------------------------
S_API void FramePipeline::Clear()
{
	SResult res = S_SUCCESS;	

	// Clear the FPS timer timestamps
	if (m_pFrameBeginTimestamp) delete m_pFrameBeginTimestamp;
	if (m_pFrameEndTimestamp) delete m_pFrameEndTimestamp;
	if (m_pdLastFrameDuration) delete m_pdLastFrameDuration;
	if (m_pFPSBeginTimestamp) delete m_pFPSBeginTimestamp;
	m_pFrameBeginTimestamp = 0;
	m_pFrameEndTimestamp = 0;
	m_pdLastFrameDuration = 0;
	m_pFPSBeginTimestamp = 0;		

	m_pEngine = 0;
	m_bStartedFPSTimer = false;

	for (unsigned int iSection = 0; iSection < m_nUsedFramepipelineSections; ++iSection)
	{
		m_pSections[iSection].Clear();
	}
}



// -------------------------------------------------------------------------------------
S_API SResult FramePipeline::RegisterSection(IFramePipelineSection* pSection)
{
	assert(pSection);
	assert(m_pSections);

	if (m_nUsedFramepipelineSections == MAX_FRAMEPIPELINE_SECTIONS)
		return S_ERROR;
	
	pSection->SetFramePipeline(this);

	SFramePipeSectionPtr& ptr = m_pSections[m_nUsedFramepipelineSections];
	ptr.pSection = pSection;
	ptr.ref = 0;

	m_nUsedFramepipelineSections++;

	return S_SUCCESS;
}


// -------------------------------------------------------------------------------------
S_API SResult FramePipeline::ExecuteSections(usint32 iSkippedSections)
{
	assert(m_pSections);


	///////////////////////////// Frame Start //////////////////////////////

	m_FrameDebugInfo.frameTimer.Start();

	// Application specific frame updates
	m_pEngine->GetApplication()->Update((float)m_FrameDebugInfo.lastFrameDuration);

	m_FrameDebugInfo.budgetTimerIndent = 0;
	unsigned int frameBudgetTimer = StartBudgetTimer("FramePipeline::ExecuteSections()");

	if (m_nUsedFramepipelineSections > 0)
	{
		for (unsigned int iSection = 0; iSection < m_nUsedFramepipelineSections; ++iSection)		
		{
			SFramePipeSectionPtr& pSection = m_pSections[iSection];
			if (iSkippedSections & pSection->GetType())
				continue;

			if (pSection->GetType() == eFPSEC_RENDERING)
				m_FrameDebugInfo.renderTimer.Start();


			SResult sr = pSection->Run();
			if (Failure(sr))
				return S_ERROR;



			// does not do anything if not running
			m_FrameDebugInfo.renderTimer.Stop();
		}
	}


	// Do physics
	m_pEngine->GetPhysics()->Update();



	StopBudgetTimer(frameBudgetTimer);
	m_FrameDebugInfo.frameTimer.Stop();
	m_FrameDebugInfo.lastFrameDuration = m_FrameDebugInfo.frameTimer.GetDuration();

	
	// Swap budget timer arrays	
	m_FrameDebugInfo.stagedBudgetTimers = (m_FrameDebugInfo.stagedBudgetTimers + 1) % 2;	
	m_FrameDebugInfo.GetUnstagedBudgetTimers().clear();

	///////////////////////////// Frame End //////////////////////////////	


	return S_SUCCESS;
}

// -------------------------------------------------------------------------------------
S_API unsigned int FramePipeline::StartBudgetTimer(const char* name)
{
	vector<SFrameDebugTimer>& unstagedTimers = m_FrameDebugInfo.GetUnstagedBudgetTimers();
	
	unstagedTimers.push_back(SFrameDebugTimer());	
	
	SFrameDebugTimer& timer = unstagedTimers.back();
	timer.id = unstagedTimers.size() - 1; // using actual IDs, becoming invalid next frame
	
	stringstream timerName;
	for (unsigned int i = 0; i < m_FrameDebugInfo.budgetTimerIndent; ++i)
		timerName << "  ";

	timerName << name;

	timer.name = timerName.str();
	timer.Start();
	
	m_FrameDebugInfo.budgetTimerIndent++;

	return timer.id;
}

// -------------------------------------------------------------------------------------
S_API void FramePipeline::ResumeBudgetTimer(unsigned int timerId)
{	
	vector<SFrameDebugTimer>& unstagedTimers = m_FrameDebugInfo.GetUnstagedBudgetTimers();

	if (timerId >= unstagedTimers.size())
		return;

	unstagedTimers[timerId].Resume();
	m_FrameDebugInfo.budgetTimerIndent++;			

	// cannot resume - not found
}

// -------------------------------------------------------------------------------------
S_API void FramePipeline::StopBudgetTimer(unsigned int timerId)
{
	vector<SFrameDebugTimer>& unstagedTimers = m_FrameDebugInfo.GetUnstagedBudgetTimers();

	if (timerId >= unstagedTimers.size())
		return;

	unstagedTimers[timerId].Stop();

	if (m_FrameDebugInfo.budgetTimerIndent > 0)
		m_FrameDebugInfo.budgetTimerIndent--;
}

// -------------------------------------------------------------------------------------
S_API void FramePipeline::RenderDebugInfo()
{
	// Only update shown values when certain time elapsed, so it can be read better.
	m_FrameDebugInfo.frameTimeAcc += m_FrameDebugInfo.lastFrameDuration;
	if (m_FrameDebugInfo.frameTimeAcc >= 1.0)
	{
		m_FrameDebugInfo.lastFrameCounter = m_FrameDebugInfo.frameCounter;
		m_FrameDebugInfo.lastMinFrameTime = m_FrameDebugInfo.minFrameTime;
		m_FrameDebugInfo.lastMaxFrameTime = m_FrameDebugInfo.maxFrameTime;
		m_FrameDebugInfo.frameTimeAcc = 0;
		m_FrameDebugInfo.frameCounter = 0;
		m_FrameDebugInfo.minFrameTime = DBL_MAX;
		m_FrameDebugInfo.maxFrameTime = DBL_MIN;
	}

	m_FrameDebugInfo.frameCounter++;

	if (m_FrameDebugInfo.lastFrameDuration < m_FrameDebugInfo.minFrameTime)
		m_FrameDebugInfo.minFrameTime = m_FrameDebugInfo.lastFrameDuration;

	if (m_FrameDebugInfo.lastFrameDuration > m_FrameDebugInfo.maxFrameTime)
		m_FrameDebugInfo.maxFrameTime = m_FrameDebugInfo.lastFrameDuration;

	IRenderer* pRenderer = m_pEngine->GetRenderer();
	if (IS_VALID_PTR(pRenderer))
	{
		IViewport* pTargetVp = pRenderer->GetTargetViewport();
		if (IS_VALID_PTR(pTargetVp))
		{
			SCamera* pCamera = pTargetVp->GetCamera();
			if (IS_VALID_PTR(pCamera))
				m_DebugInfo.Update(pCamera, 1.0 / m_FrameDebugInfo.lastFrameDuration, m_FrameDebugInfo);
		}
	}
}



SP_NMSPACE_END