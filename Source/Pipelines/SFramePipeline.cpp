// ******************************************************************************************

// This file is part of the SpeedPoint Engine

// ******************************************************************************************

#include <Abstract\Pipelines\IRenderPipeline.h>
#include <Abstract\IRenderer.h>
#include <Pipelines\SFramePipeline.h>
#include <SSpeedPointEngine.h>

namespace SpeedPoint
{	
	// ******************************************************************************************

	S_API SResult SFramePipeline::Initialize(SpeedPointEngine* pEngine)
	{
		SP_ASSERTR(pEngine, S_INVALIDPARAM);
			
		m_pEngine = pEngine;
		m_bStartedFPSTimer = false;

		// Initialize the timestamp pointers		
		m_pFrameBeginTimestamp = new std::chrono::time_point<std::chrono::high_resolution_clock>();
		m_pFrameEndTimestamp = new std::chrono::time_point<std::chrono::high_resolution_clock>();
		m_pFPSBeginTimestamp = new std::chrono::time_point<std::chrono::high_resolution_clock>();
		m_pdLastFrameDuration = new std::chrono::duration<double>();

		// Initialize the Dynamics pipeline
		if (Failure(m_DynamicsPipeline.Initialize(pEngine, this)))
			return m_pEngine->LogE("Failed initialize dynamics pipeline!");

		// Initialize the render pipeline
		m_pRenderPipeline = m_pEngine->GetRenderer()->GetRenderPipeline();
		SP_ASSERTXR(m_pRenderPipeline, S_NOTINIT, m_pEngine, "Render Pipeline is zero!");

		if (Failure(m_pRenderPipeline->SetFramePipeline(this)))
			return m_pEngine->LogE("Failed initialize dynamics pipeline!");

		return S_SUCCESS;
	}

	// ******************************************************************************************

	S_API SResult SFramePipeline::Clear()
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

		// Clear dynamics pipeline
		if (Failure(m_DynamicsPipeline.Clear()))		
			res = S_ERROR;		

		// Clear the render Pipeline
		if (m_pRenderPipeline)
		{
			if (Failure(m_pRenderPipeline->Clear()))	
				res = S_ERROR;

			delete m_pRenderPipeline; // delete the render pipeline instance
			m_pRenderPipeline = 0;
		}

		m_pEngine = 0;
		m_bStartedFPSTimer = false;

		return res;
	}

	// ******************************************************************************************

	S_API S_FRAMEPIPELINE_STAGE SFramePipeline::GetCurrentStage()
	{
		return m_Stage;
	}
	
	// ******************************************************************************************

	S_API SResult SFramePipeline::BeginFrame(unsigned int iSkipStages)
	{
		SResult res = S_SUCCESS;
		SResult tempRes = S_SUCCESS;
		
		// Capture current timestamps
		*m_pFrameBeginTimestamp = m_HighResClock.now();
		if (!m_bStartedFPSTimer)
		{
			*m_pFPSBeginTimestamp = *m_pFrameBeginTimestamp;
			m_bStartedFPSTimer = true;			
		}
		
		// store skip stages
		m_iCurrentSkipStages = iSkipStages;

		// Call the BeginFrame event, as timestamp has now been captured				
		SEventParameters params;
		params.Add("sender", S_PARAMTYPE_PTR, this);
		CallEvent(S_E_BEGINFRAME, &params);		
		
		// Go on with Dynamics Pipeline
		if ((m_iCurrentSkipStages & S_DYNAMICS) == 0)
		{
			tempRes = DoDynamics(); // will throw specific errors		
			if (res == S_SUCCESS && res != tempRes)
				res = tempRes;
		}

		// Go on with Render Pipeline
		if ((m_iCurrentSkipStages & S_RENDER) == 0)
		{
			tempRes = DoRender(); // will throw specific errors
			if (res == S_SUCCESS && res != tempRes)
				res = tempRes;
		}					
		
		// return gathered result		
		return res;
	}

	// ******************************************************************************************

	S_API SResult SFramePipeline::DoDynamics()
	{
		SResult res = S_SUCCESS, tempRes = S_SUCCESS;

		// Call the Event
		SEventParameters params;
		params.Add("sender", S_PARAMTYPE_PTR, this);
		CallEvent(S_E_DYNAMICS, &params);				

		// Do the animation part of the dynamics pipeline		
		if ((m_iCurrentSkipStages & S_DYNAMICS_ANIM) == 0)
		{			
			tempRes = m_DynamicsPipeline.DoAnimation();
			if (res == S_SUCCESS && res != tempRes)
				res = tempRes;
		}

		// Do the physics part of the dynamics pipeline
		if ((m_iCurrentSkipStages & S_DYNAMICS_PHYSICS) == 0)
		{			
			tempRes = m_DynamicsPipeline.DoPhysics();
			if (res == S_SUCCESS && res != tempRes)
				res = tempRes;
		}

		// Do the input part of the dynamics pipeline
		if ((m_iCurrentSkipStages & S_DYNAMICS_INPUT) == 0)
		{			
			tempRes = m_DynamicsPipeline.DoInteraction();
			if (res == S_SUCCESS && res != tempRes)
				res = tempRes;
		}

		// Do the Script part of the dynamics pipeline
		if ((m_iCurrentSkipStages & S_DYNAMICS_SCRIPT) == 0)
		{			
			tempRes = m_DynamicsPipeline.DoScriptExecution();
			if (res == S_SUCCESS && res != tempRes)
				res = tempRes;
		}

		// Do the RenderScript part of the dynamics pipeline
		if ((m_iCurrentSkipStages & S_DYNAMICS_RENDERSCRIPT) == 0)
		{			
			tempRes = m_DynamicsPipeline.DoRecalcRenderscript();
			if (res == S_SUCCESS && res != tempRes)
				res = tempRes;
		}

		// Return the result of all stages
		params.Clear();
		return res;
	}

	// ******************************************************************************************
	
	S_API SResult SFramePipeline::DoRender()
	{
		SResult res = S_SUCCESS, tempRes = S_SUCCESS;

		// fire event
		SEventParameters params;
		params.Add("sender", S_PARAMTYPE_PTR, this);
		CallEvent(S_E_RENDER, &params);

		// Begin the rendering (Clear backbuffers, ...)
		if ((m_iCurrentSkipStages & S_RENDER_BEGIN) == 0)
		{			
			tempRes = m_pRenderPipeline->DoBeginRendering();
			if (res == S_SUCCESS && res != tempRes)
				res = tempRes;
		}

		// Begin the Draw-Calls for solids GeometrySection
		if ((m_iCurrentSkipStages & S_RENDER_GEOMETRY) == 0)
		{
			tempRes = m_pRenderPipeline->DoGeometrySection();
			if (res == S_SUCCESS && res != tempRes)
				res = tempRes;

			// m_pRenderPipeline->DoGeometrySection(); already fired S_E_RENDER_GEOMETRY_CALLS, so
			// the application should already have its Draw calls done here.
			// that's why we can go on with ExitGeometrySection() here.			
			tempRes = m_pRenderPipeline->ExitGeometrySection();
			if (res == S_SUCCESS && res != tempRes)
				res = tempRes;
		}				

		// Do the lighting section
		if ((m_iCurrentSkipStages & S_RENDER_LIGHTING) == 0)
		{
			tempRes = m_pRenderPipeline->DoLightingSection();
			if (res == S_SUCCESS && res != tempRes)
				res = tempRes;
		}

		// Do the post section
		if ((m_iCurrentSkipStages & S_RENDER_POST) == 0)
		{
			tempRes = m_pRenderPipeline->DoPost();
			if (res == S_SUCCESS && res != tempRes)
				res = tempRes;
		}

		// Do end rendering
		if ((m_iCurrentSkipStages & S_RENDER_PRESENT) == 0)
		{
			tempRes = m_pRenderPipeline->DoEndRendering();
			if (res == S_SUCCESS && res != tempRes)
				res = tempRes;
		}

		return res;
	}

	// ******************************************************************************************
	
	S_API SResult SFramePipeline::EndFrame()
	{
		SP_ASSERTR(m_pEngine
			&& m_pFrameBeginTimestamp
			&& m_pFrameEndTimestamp
			&& m_pdLastFrameDuration
			&& m_pFPSBeginTimestamp,
			S_NOTINIT);

		// capture current timestamp and calculate FPS
		*m_pFrameEndTimestamp = m_HighResClock.now();
		if (m_bStartedFPSTimer)
		{			
			*m_pdLastFrameDuration =
				std::chrono::duration_cast<std::chrono::duration<double>>(*m_pFrameEndTimestamp - *m_pFrameBeginTimestamp);

			std::chrono::duration<double> fpsTimerDuration =
				std::chrono::duration_cast<std::chrono::duration<double>>(*m_pFrameEndTimestamp - *m_pFPSBeginTimestamp);

			m_nFPSTimerFrameCount++;
			if (fpsTimerDuration.count() >= 1.0)
			{			
				m_dLastFPS = (double)m_nFPSTimerFrameCount / fpsTimerDuration.count();
				m_fLastFPS = (float)m_dLastFPS;
				m_nFPSTimerFrameCount = 0;
				m_bStartedFPSTimer = false;
			}
		}

		// Everything went well
		return S_SUCCESS;
	}
}