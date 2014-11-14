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

	IApplication* pApp = m_pEngine->GetApplication();
	assert(pApp);

	pApp->Render();	// Render callback

	m_pEngine->GetRenderer()->EndScene();

	return S_SUCCESS;
}


SP_NMSPACE_END