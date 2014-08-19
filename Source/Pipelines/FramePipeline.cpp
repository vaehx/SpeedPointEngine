// ******************************************************************************************

// This file is part of the SpeedPoint Engine

// ******************************************************************************************

#include <Abstract\IRenderPipeline.h>
#include <Abstract\IRenderer.h>
#include <Pipelines\FramePipeline.h>
#include <SpeedPointEngine.h>

SP_NMSPACE_BEG



S_API FramePipeline::FramePipeline()
: m_pEngine(0),
m_bStartedFPSTimer(false),
m_dLastFPS(0),
m_fLastFPS(0),
m_pSections(nullptr),
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

	m_pSections = new std::vector<SFramePipeSectionPtr>();
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

	if (m_pSections)
	{
		m_pSections->clear();
		delete m_pSections;
	}
	m_pSections = 0;	
}



// -------------------------------------------------------------------------------------
S_API SResult FramePipeline::RegisterSection(IFramePipelineSection* pSection)
{
	assert(pSection);
	assert(m_pSections);
	
	pSection->SetFramePipeline(this);
	m_pSections->push_back(SFramePipeSectionPtr());
	m_pSections->back().pSection = pSection;
	m_pSections->back().ref = 0;

	return S_SUCCESS;
}


// -------------------------------------------------------------------------------------
S_API SResult FramePipeline::ExecuteSections(usint32 iSkippedSections)
{
	assert(m_pSections);

	if (m_pSections->empty())
		return S_SUCCESS;

	for (auto itSection = m_pSections->begin(); itSection != m_pSections->end(); itSection++)
	{
		SFramePipeSectionPtr pSection = (*itSection);
		if (iSkippedSections & pSection->GetType())
			continue;

		if (Failure(pSection->Run()))
			return S_ERROR;
	}


	return S_SUCCESS;
}



SP_NMSPACE_END