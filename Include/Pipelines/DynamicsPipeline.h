// ******************************************************************************************

// This file is part of the SpeedPoint Engine

// ******************************************************************************************

#pragma once

#include <SPrerequisites.h>
#include <Observer\SObservedObject.h>
#include <Abstract\IFramePipeline.h>

SP_NMSPACE_BEG

// SpeedPoint Dynamics Pipeline
class S_API DynamicsPipeline : public IFramePipelineSection
{
private:
	SpeedPointEngine*	m_pEngine;
	IFramePipeline*		m_pFramePipeline;	// pointer to the handling frame pipeline

public:	
	DynamicsPipeline()
		: m_pEngine(0)
	{
	}

	DynamicsPipeline(SpeedPointEngine* pEngine)
		: m_pEngine(pEngine)
	{
	}

	~DynamicsPipeline()
	{
		//SObservedObject::~SObservedObject();
	}


	virtual SResult Run();

	virtual void SetFramePipeline(IFramePipeline* pPipe)
	{
		m_pFramePipeline = pPipe;
	}
	
	virtual EFramePipelineSectionType GetType() const
	{
		return eFPSEC_PHYSICS;
	}
};


SP_NMSPACE_END