//////////////////////////////////////////////////////////////////////////////////
//
// This file is part of the SpeedPointEngine
// Copyright (c) 2011-2014, iSmokiieZz
// ------------------------------------------------------------------------------
// Filename:	IApplication.h
// Created:	8/12/2014 by iSmokiieZz
// Description:
// -------------------------------------------------------------------------------
// History:
//
//////////////////////////////////////////////////////////////////////////////////

#ifndef __iapplication_h__
#define __iapplication_h__

#if _MSC_VER > 1000
#pragma once
#endif


#include <SPrerequisites.h>
#include <Abstract\IGameEngine.h>


SP_NMSPACE_BEG


struct S_API IFramePipeline;

struct S_API IApplication
{
	virtual ~IApplication() {}

	virtual void OnInit(IFramePipeline* pFramePipeline, IGameEngine* pGameEngine) = 0;
	virtual void OnSettingChange(const SSettingsDesc& dsc) = 0;

	virtual void OnLogReport(SResult res, const SString& msg) = 0;

	// Summary:
	//	Called whenever the engine is going to execute dynamics stuff
	virtual void Update(float fLastFrameTime) = 0;



	
	// Summary:
	//	Do all Render() calls inside this function.
	virtual void Render() = 0;
};



SP_NMSPACE_END
#endif