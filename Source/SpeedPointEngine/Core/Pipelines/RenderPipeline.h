////////////////////////////////////////////////////////////////////////////////////////
//
// This file is part of the SpeedPointEngine
// Copyright (c) 2011-2014, iSmokiieZz
// ------------------------------------------------------------------------------
// Filename:	RenderPipeline.h
// Created:	8/18/2014 by iSmokiieZz
// Description:
// -------------------------------------------------------------------------------
// History:
//
//////////////////////////////////////////////////////////////////////////////////

#pragma once
#include "..\SCommandBuffering.h"
#include <Abstract\IRenderPipeline.h>
#include <Abstract\SPrerequisites.h>

SP_NMSPACE_BEG

struct S_API IFramePipeline;

class S_API RenderPipeline : public IRenderPipeline
{
private:
	IGameEngine* m_pEngine;
	IFramePipeline* m_pFramePipeline;

public:
	RenderPipeline();
	~RenderPipeline();

	virtual void Init(IGameEngine* pEngine);
	virtual void Clear();

	virtual void SetFramePipeline(IFramePipeline* pPipe);

	virtual SResult Run();

	virtual EFramePipelineSectionType GetType() const
	{
		return eFPSEC_RENDERING;
	}
};


SP_NMSPACE_END