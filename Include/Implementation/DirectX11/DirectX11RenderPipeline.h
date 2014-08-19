//////////////////////////////////////////////////////////////////////////////////////////////////
//
//	This file is part of the SpeedPoint Game Engine
//
//	written by Pascal R. aka iSmokiieZz
//	(c) 2011-2014, All rights reserved.
//
//////////////////////////////////////////////////////////////////////////////////////////////////

#pragma once
#include <SPrerequisites.h>
#include <Abstract\IRenderPipeline.h>
#include <Pipelines\SCommandBuffering.h>

SP_NMSPACE_BEG

struct S_API IFramePipeline;
class S_API SpeedPointEngine;

// DirectX9 Implementation of the SpeedPoint Render Pipeline
class S_API DirectX11RenderPipeline : public IRenderPipeline
{
private:
	SpeedPointEngine*		m_pEngine;
	IFramePipeline*			m_pFramePipeline;

public:	
	DirectX11RenderPipeline();	
	~DirectX11RenderPipeline();

	virtual void Init(SpeedPointEngine* pEngine);
	virtual void Clear();

	virtual void SetFramePipeline(IFramePipeline* pPipe)
	{
		m_pFramePipeline = pPipe;
	}

	virtual SResult Run();

	virtual EFramePipelineSectionType GetType() const
	{
		return eFPSEC_RENDERING;
	}	
};


SP_NMSPACE_END