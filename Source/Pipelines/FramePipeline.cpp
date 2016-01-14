// ******************************************************************************************

// This file is part of the SpeedPoint Engine

// ******************************************************************************************

#include <Abstract\IRenderPipeline.h>
#include <Abstract\IRenderer.h>
#include <Abstract\IApplication.h>
#include <Pipelines\FramePipeline.h>
#include <SpeedPointEngine.h>

SP_NMSPACE_BEG



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
		pFRS->text = new char[200];
		pFRS->fontSize = fontSize;
	}
}

// -----------------------------------------------------------------------------
S_API void CDebugInfo::Update(SpeedPoint::SCamera* pCamera, double fps, const SFrameDebugInfo& fdi)
{
	// CamStat
	InitFontRenderSlot(&m_pCamStats, true, true, SpeedPoint::SColor(1.0f, 1.0f, 1.0f), 0, 0);
	Vec3f camForward = pCamera->GetForward();
	Quat& turnQuat = pCamera->d_turnQuat;
	SpeedPoint::SPSPrintf(m_pCamStats->text, 200, "Cam(%.2f %.2f %.f | %.2f %.2f %.2f | %.2f %.2f %.2f)",
		pCamera->position.x, pCamera->position.y, pCamera->position.z,
		camForward.x, camForward.y, camForward.z,
		pCamera->d_turn.x, pCamera->d_turn.y, pCamera->d_turn.z);

	// Cam view matrix
	SMatrix& viewMtx = pCamera->viewMatrix;
	for (unsigned int i = 0; i < 4; ++i)
	{
		InitFontRenderSlot(&m_pViewMtxRows[i], true, true, SColor(1.0f, 1.0f, 1.0f), 0, 100 + i*18);
		SPSPrintf(m_pViewMtxRows[i]->text, 200, "( %.2f %.2f %.2f %.2f )", viewMtx.m[i][0], viewMtx.m[i][1], viewMtx.m[i][2], viewMtx.m[i][3]);
	}

	// Frame Timers
	InitFontRenderSlot(&m_pFrameTimes, true, true, SpeedPoint::SColor(1.0f, 1.0f, 1.0f), 0, 18);
	SpeedPoint::SPSPrintf(m_pFrameTimes->text, 200, "Render: %.2f Tick: %.2f Frame: %.2f",
		fdi.renderTimer.GetDuration() * 1000.0, fdi.tickTimer.GetDuration() * 1000.0, fdi.frameTimer.GetDuration() * 1000.0);

	// FPS
	InitFontRenderSlot(&m_pFPS, true, true, SpeedPoint::SColor(1.0f, 1.0f, 0.3f), 0, 37, SpeedPoint::eFONTSIZE_MEDIUM);
	SpeedPoint::SPSPrintf(m_pFPS->text, 200, "FPS %u (%u..%u)",
		fdi.lastFrameCounter, (unsigned int)(1.0 / fdi.lastMaxFrameTime), (unsigned int)(1.0 / fdi.lastMinFrameTime));	

	// Terrain info
	InitFontRenderSlot(&m_pTerrain, true, true, SpeedPoint::SColor(1.f, 1.f, 1.f), 0, 65);
	SpeedPoint::STerrainRenderDesc* pTerrainRenderDesc = m_pEngine->GetRenderer()->GetTerrainRenderDesc();
	SpeedPoint::SPSPrintf(m_pTerrain->text, 200, "Terrain: %u DCs", pTerrainRenderDesc->nDrawCallDescs);
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
	m_pEngine->GetApplication()->Update((float)m_pdLastFrameDuration->count());

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

	m_FrameDebugInfo.frameTimer.Stop();


	///////////////////////////// Frame End //////////////////////////////	


	return S_SUCCESS;
}

// -------------------------------------------------------------------------------------
S_API void FramePipeline::RenderDebugInfo()
{
	// Update DebugInfo Font Render Slots
	double lastFrameTime = m_FrameDebugInfo.frameTimer.GetDuration();
	m_FrameDebugInfo.frameTimeAcc += lastFrameTime;
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

	if (lastFrameTime < m_FrameDebugInfo.minFrameTime)
		m_FrameDebugInfo.minFrameTime = lastFrameTime;

	if (lastFrameTime > m_FrameDebugInfo.maxFrameTime)
		m_FrameDebugInfo.maxFrameTime = lastFrameTime;

	IRenderer* pRenderer = m_pEngine->GetRenderer();
	if (IS_VALID_PTR(pRenderer))
	{
		IViewport* pTargetVp = pRenderer->GetTargetViewport();
		if (IS_VALID_PTR(pTargetVp))
		{
			SCamera* pCamera = pTargetVp->GetCamera();
			if (IS_VALID_PTR(pCamera))
				m_DebugInfo.Update(pCamera, 1.0 / m_FrameDebugInfo.frameTimer.GetDuration(), m_FrameDebugInfo);
		}
	}
}



SP_NMSPACE_END