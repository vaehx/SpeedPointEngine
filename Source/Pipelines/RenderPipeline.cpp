//////////////////////////////////////////////////////////////////////////////////
//
// This file is part of the SpeedPointEngine
// Copyright (c) 2011-2014, iSmokiieZz
// ------------------------------------------------------------------------------
// Filename:	RenderPipeline.cpp
// Created:	8/18/2014 by iSmokiieZz
// Description:
// -------------------------------------------------------------------------------
// History:
//
//////////////////////////////////////////////////////////////////////////////////

#include <Pipelines\RenderPipeline.h>
#include <Abstract\IGameEngine.h>
#include <Abstract\IApplication.h>
#include <Abstract\IRenderer.h>
#include <Abstract\I3DEngine.h>

SP_NMSPACE_BEG


// ----------------------------------------------------------------------------------------
S_API RenderPipeline::RenderPipeline()
: m_pEngine(nullptr),
m_pFramePipeline(nullptr)
{
}

// ----------------------------------------------------------------------------------------
S_API RenderPipeline::~RenderPipeline()
{
	Clear();
}

// ----------------------------------------------------------------------------------------
S_API void RenderPipeline::Clear()
{
	m_pEngine = nullptr;
	m_pFramePipeline = nullptr;
}

// ----------------------------------------------------------------------------------------
S_API void RenderPipeline::Init(IGameEngine* pEngine)
{
	assert(IS_VALID_PTR(pEngine));
	m_pEngine = pEngine;
}

// ----------------------------------------------------------------------------------------
S_API void RenderPipeline::SetFramePipeline(IFramePipeline* pPipe)
{
	assert(pPipe);
	m_pFramePipeline = pPipe;
}

// ----------------------------------------------------------------------------------------
S_API SResult RenderPipeline::Run()
{
	if (!IS_VALID_PTR(m_pEngine))
		return S_NOTINIT;

	m_pEngine->GetRenderer()->BeginScene();	


	const SCamera* pCamera = m_pEngine->GetRenderer()->GetTargetViewport()->GetCamera();
	m_pEngine->Get3DEngine()->CollectVisibleObjects(pCamera);

	m_pEngine->Get3DEngine()->RenderCollected();

	// Let the Frame Pipeline render debug info
	if (IS_VALID_PTR(m_pFramePipeline))
		m_pFramePipeline->RenderDebugInfo();

	m_pEngine->GetRenderer()->EndScene();
	m_pEngine->Get3DEngine()->ClearRenderObjects();

	return S_SUCCESS;
}


SP_NMSPACE_END