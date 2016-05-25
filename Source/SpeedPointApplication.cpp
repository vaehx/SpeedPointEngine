//////////////////////////////////////////////////////////////////////////////////
//
// This file is part of the SpeedPointEngine
// Copyright (c) 2011-2014, iSmokiieZz
// ------------------------------------------------------------------------------
// Filename:	SpeedPointApplication.cpp
// Created:	8/18/2014 by iSmokiieZz
// Description:
// -------------------------------------------------------------------------------
// History:
//
//////////////////////////////////////////////////////////////////////////////////

#include <SpeedPointApplication.h>

SP_NMSPACE_BEG

// ---------------------------------------------------------------------------------------------
S_API SpeedPointApplication::SpeedPointApplication()
: m_pFramePipeline(nullptr),
m_pEngine(nullptr)
{
}

// ---------------------------------------------------------------------------------------------
S_API bool SpeedPointApplication::IsInited()
{
	return IS_VALID_PTR(m_pFramePipeline) && IS_VALID_PTR(m_pEngine);
}

// ---------------------------------------------------------------------------------------------
S_API void SpeedPointApplication::OnInit(IFramePipeline* pFramePipeline, IGameEngine* pGameEngine)
{
	assert(IS_VALID_PTR(pFramePipeline));
	assert(IS_VALID_PTR(pGameEngine));

	m_pFramePipeline = pFramePipeline;
	m_pEngine = pGameEngine;

	OnInitGeometry();
}

// ---------------------------------------------------------------------------------------------
S_API void SpeedPointApplication::OnLogReport(SResult res, const SString& msg)
{
#ifdef _DEBUG
	OutputDebugString(msg);
	OutputDebugString("\n");
#endif
}

// ---------------------------------------------------------------------------------------------
S_API void SpeedPointApplication::Update(float fLastFrameTime)
{
	assert(IsInited());
}

// ---------------------------------------------------------------------------------------------
S_API void SpeedPointApplication::Render()
{
	assert(IsInited());
}

// ---------------------------------------------------------------------------------------------
S_API void SpeedPointApplication::OnInitGeometry()
{
}


SP_NMSPACE_END