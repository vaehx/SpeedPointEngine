//////////////////////////////////////////////////////////////////////////////////////////////////
//
//	This file is part of the SpeedPoint Game Engine
//
//	(c) 2011-2014 Pascal R. aka iSmokiieZz
//	All rights reserved.
//
//////////////////////////////////////////////////////////////////////////////////////////////////

#pragma once

#include <SPrerequisites.h>
#include <Observer\SObservedObject.h>
#include <Pipelines\SDynamicsPipeline.h>
#include <chrono> // Notice: Requires at least C++11 !

SP_NMSPACE_BEG

//////////////////////////////////////////////////////////////////////////////////////////////////

struct S_API IRenderPipeline;
class S_API SpeedPointEngine;

// SpeedPoint Frame Pipeline Engine
// This is the main Pipeline of the Game Engine and handles all other pipeline.
// It also uses an Event System, so that you have access to all pipeline states in your CPP code or your Script
class S_API FramePipeline : public SObservedObject
{
private:
	S_FRAMEPIPELINE_STAGE			m_Stage;			// current state of the frame pipeline
	unsigned int				m_iCurrentSkipStages;		// the stages to be skipped for current frame

	SDynamicsPipeline			m_DynamicsPipeline;		// instance of the dynamics Pipeline
	IRenderPipeline*			m_pRenderPipeline;		// pointer to an implemented instance of IRenderPipeline (abstr.)
							
	SpeedPointEngine*			m_pEngine;

	bool					m_bStartedFPSTimer;		
	std::chrono::high_resolution_clock	m_HighResClock;		
	std::chrono::time_point<std::chrono::high_resolution_clock>* m_pFPSBeginTimestamp; // is a ptr due to STL. DO NEVER make this variable public!
	std::chrono::time_point<std::chrono::high_resolution_clock>* m_pFrameBeginTimestamp; // is a ptr due to STL. DO NEVER make this variable public!
	std::chrono::time_point<std::chrono::high_resolution_clock>* m_pFrameEndTimestamp; // is a ptr due to STL. DO NEVER make this variable public!
	std::chrono::duration<double>*		m_pdLastFrameDuration; // is a ptr due to STL. DO NEVER make this variable public!
	unsigned int				m_nFPSTimerFrameCount;
	double					m_dLastFPS;
	float					m_fLastFPS;			// extra variable due to possible information loss with float

	void*					m_pCustomEventParamData;

public:
	// Default constructor
	FramePipeline()
		: m_pRenderPipeline(0),
		m_pEngine(0),
		m_bStartedFPSTimer(false),
		m_dLastFPS(0),
		m_fLastFPS(0),
		m_iCurrentSkipStages(0)
	{
	}

	// Default destructor
	~FramePipeline()			
	{
		Clear(); // important due to STL timestamps (chrono)

		SObservedObject::~SObservedObject();
	}

	// Initialize the frame pipeline with given SpeedPointEngine ptr
	SResult Initialize(SpeedPointEngine* pEngine);

	// Clear the frame pipeline
	SResult Clear();

	// Get the current state of the frame pipeline, or the partial pipeline respectively
	S_FRAMEPIPELINE_STAGE GetCurrentStage();

	// Takes beginning timestamp
	SResult BeginFrame(unsigned int iSkipStages = 0);

	// Executes the Dynamics pipeline
	SResult DoDynamics();

	// Executes the Rendering pipeline
	SResult DoRender();

	// Executes the End Frame pipeline
	SResult EndFrame();

	// Set the ptr to the custom event parameter data
	void SetCustomEventParameterData(void* pData)
	{
		m_pCustomEventParamData = pData;
	}

	// Get ptr to the custom event parameter data
	void* GetCustomEventParameterData()
	{
		return m_pCustomEventParamData;
	}

	// Overwrite CallEvent because we want to add some params that should be passed everwhere.
	// By putting this into this function, we can save a lot of code.
	SResult CallEvent(unsigned int iIndex, SEventParameters* pParams);
};	

typedef class FramePipeline FrameEngine;


SP_NMSPACE_END