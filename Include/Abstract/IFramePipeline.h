//////////////////////////////////////////////////////////////////////////////////
//
// This file is part of the SpeedPointEngine
// Copyright (c) 2011-2014, iSmokiieZz
// ------------------------------------------------------------------------------
// Filename:	IFramePipeline.h
// Created:	8/12/2014 by iSmokiieZz
// Description:
// -------------------------------------------------------------------------------
// History:
//
//////////////////////////////////////////////////////////////////////////////////

#ifndef __iframepipeline_h__
#define __iframepipeline_h__

#if _MSC_VER > 1000
#pragma once
#endif

#include <SPrerequisites.h>

SP_NMSPACE_BEG

struct S_API IFramePipeline;
struct S_API IGameEngine;


//////////////////////////////////////////////////////////////////////////////////

enum EFramePipelineSectionType
{
	eFPSEC_PHYSICS		= BITN(1),		// not yet supported
	eFPSEC_AI		= BITN(2),		// not yet supported
	eFPSEC_ANIMATION	= BITN(3),		// not yet supported
	eFPSEC_SCRIPT		= BITN(4),		// not yet supported
	eFPSEC_RENDERING	= BITN(5)
};
#define DEFAULT_SKIPPED_SECTIONS eFPSEC_PHYSICS | eFPSEC_AI | eFPSEC_ANIMATION | eFPSEC_SCRIPT

struct S_API IFramePipelineSection
{
	virtual ~IFramePipelineSection()
	{
	}

	virtual SResult Run() = 0;
	virtual EFramePipelineSectionType GetType() const = 0;
	virtual void SetFramePipeline(IFramePipeline* pipe) = 0;
	virtual void Clear() = 0;
};



//////////////////////////////////////////////////////////////////////////////////

// Summary:
//	Interface for the frame pipeline handling the execution of all engine features
struct S_API IFramePipeline
{
	virtual ~IFramePipeline()
	{
	}

	virtual void Initialize(IGameEngine* pGameEngine) = 0;
	virtual void Clear() = 0;
	virtual IFramePipelineSection* GetCurrentSection() const = 0;
	virtual SResult RegisterSection(IFramePipelineSection* pSection) = 0;
	virtual SResult ExecuteSections(usint32 iSkippedSections = DEFAULT_SKIPPED_SECTIONS) = 0;
};



SP_NMSPACE_END

#endif