//////////////////////////////////////////////////////////////////////////////////
//
// This file is part of the SpeedPointEngine
// Copyright (c) 2011-2014, iSmokiieZz
// ------------------------------------------------------------------------------
// Filename:	DirectX11RenderPipeline.h
// Created:	8/18/2014 by iSmokiieZz
// Description:
// -------------------------------------------------------------------------------
// History:
//
//////////////////////////////////////////////////////////////////////////////////

#include <Implementation\DirectX11\DirectX11RenderPipeline.h>

SP_NMSPACE_BEG

// --------------------------------------------------------------------------------------------
S_API DirectX11RenderPipeline::DirectX11RenderPipeline()
: m_pEngine(nullptr),
m_pFramePipeline(nullptr)
{
}

// --------------------------------------------------------------------------------------------
S_API DirectX11RenderPipeline::~DirectX11RenderPipeline()
{
	Clear();
}

// --------------------------------------------------------------------------------------------
S_API void DirectX11RenderPipeline::Clear()
{
	m_pEngine = nullptr;
	m_pFramePipeline = nullptr;
}


SP_NMSPACE_END