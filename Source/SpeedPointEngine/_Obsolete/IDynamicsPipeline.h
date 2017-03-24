//////////////////////////////////////////////////////////////////////////////////
//
// This file is part of the SpeedPointEngine
// Copyright (c) 2011-2014, iSmokiieZz
// ------------------------------------------------------------------------------
// Filename:	IDynamics.h
// Created:	8/18/2014 by iSmokiieZz
// Description:
// -------------------------------------------------------------------------------
// History:
//
//////////////////////////////////////////////////////////////////////////////////

#ifndef __idynamics_h__
#define __idynamics_h__

#if _MSC_VER > 1000
#pragma once
#endif

#include "SPrerequisites.h"
#include "IFramePipeline.h"

SP_NMSPACE_BEG

struct S_API IPhysics;
struct S_API IAI;
struct S_API IAnimationSystem;
struct S_API IScriptSystem;



struct S_API IPhysicsFramePipeSection : public IFramePipelineSection
{
	virtual ~IPhysicsFramePipeSection() {}

	virtual SResult Init(IFramePipeline* pFramePipe, IPhysics* pPhysics) = 0;

	virtual EFramePipelineSectionType GetType() const
	{
		return eFPSEC_PHYSICS;
	}
};




struct S_API IAIFramePipeSection : public IFramePipelineSection
{
	virtual ~IAIFramePipeSection() {}

	virtual SResult Init(IFramePipeline* pFramePipe, IAI* pAI) = 0;

	virtual EFramePipelineSectionType GetType() const
	{
		return eFPSEC_AI;
	}
};




struct S_API IAnimationFramePipeSection : public IFramePipelineSection
{
	virtual ~IAnimationFramePipeSection() {}

	virtual SResult Init(IFramePipeline* pFramePipe, IAnimationSystem* pAnimation) = 0;

	virtual EFramePipelineSectionType GetType() const
	{
		return eFPSEC_ANIMATION;
	}
};




struct S_API IScriptFramePipeSection : public IFramePipelineSection
{
	virtual ~IScriptFramePipeSection() {}

	virtual SResult Init(IFramePipeline* pFramePipe, IScriptSystem* pScript) = 0;

	virtual EFramePipelineSectionType GetType() const
	{
		return eFPSEC_SCRIPT;
	}
};



SP_NMSPACE_END



#endif