//////////////////////////////////////////////////////////////////////////////////
//
// This file is part of the SpeedPointEngine
// Copyright (c) 2011-2014, iSmokiieZz
// ------------------------------------------------------------------------------
// Filename:	SpeedPointApplication.h
// Created:	8/18/2014 by iSmokiieZz
// Description:
// -------------------------------------------------------------------------------
// History:
//
//////////////////////////////////////////////////////////////////////////////////

#ifndef __speedpointapplication_h__
#define __speedpointapplication_h__

#if _MSC_VER > 1000
#pragma once
#endif

#include <SPrerequisites.h>
#include <Abstract\IApplication.h>
#include <Abstract\IGameEngine.h> // for SSettingsDesc
SP_NMSPACE_BEG


// Summary:
//	Default application. You can use it and override some funtions to avoid implementing the whole IApplication interface.
class S_API SpeedPointApplication : public IApplication
{
protected:
	IFramePipeline* m_pFramePipeline;
	IGameEngine* m_pEngine;

	virtual bool IsInited();
	virtual void OnInitGeometry();

public:
	SpeedPointApplication();
	virtual ~SpeedPointApplication()
	{
	}

	virtual void OnInit(IFramePipeline* pFramePipeline, IGameEngine* pGameEngine);
	virtual void OnLogReport(SResult res, const SString& msg);
	virtual void OnSettingChange(const SSettingsDesc& dsc);
	virtual void Update(float fLastFrameTime);
	virtual void Render();
};



SP_NMSPACE_END
#endif