// ******************************************************************************************

// This file is part of the SpeedPoint Engine

// ******************************************************************************************

#pragma once

#include <SPrerequisites.h>
#include <Observer\SObservedObject.h>
#include <Pipelines\SDynamicsPipeline.h>
#include <chrono> // Notice: Requires at least C++11 !

namespace SpeedPoint
{
	// SpeedPoint Frame Pipeline Engine
	// This is the main Pipeline of the Game Engine and handles all other pipeline.
	// It also uses an Event System, so that you have access to all pipeline states in your CPP code or your Script
	class S_API SFramePipeline : public SObservedObject
	{
	private:
		S_FRAMEPIPELINE_STAGE			m_Stage;			// current state of the frame pipeline
		unsigned int				m_iCurrentSkipStages;		// the stages to be skipped for current frame

		SDynamicsPipeline			m_DynamicsPipeline;		// instance of the dynamics Pipeline
		SRenderPipeline*			m_pRenderPipeline;		// pointer to an implemented instance of SRenderPipeline (abstr.)
							
		SpeedPointEngine*			m_pEngine;

		bool					m_bStartedFPSTimer;
		std::chrono::high_resolution_clock	m_HighResClock;
		std::chrono::high_resolution_clock::time_point	m_FPSBeginTimestamp;
		std::chrono::high_resolution_clock::time_point	m_FrameBeginTimestamp;
		std::chrono::high_resolution_clock::time_point	m_FrameEndTimestamp;
		std::chrono::duration<double>		m_dLastFrameDuration;
		unsigned int				m_nFPSTimerFrameCount;
		double					m_dLastFPS;
		float					m_fLastFPS;			// extra variable due to possible information loss with float

	public:
		// Default constructor
		SFramePipeline()
			: m_pRenderPipeline(0),
			m_pEngine(0),
			m_bStartedFPSTimer(false),
			m_dLastFPS(0),
			m_fLastFPS(0),
			m_iCurrentSkipStages(0)
		{
		}

		// Default destructor
		~SFramePipeline()
		{
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
	};

	typedef class SFramePipeline SFrameEngine;
}