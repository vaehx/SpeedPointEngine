// ********************************************************************************************

//	This file is part of the SpeedPoint Game Engine

//	(c) 2011-2014 Pascal R. aka iSmokiieZz
//	All rights reserved.

// ********************************************************************************************

#pragma once

#include <SPrerequisites.h>
#include <Util\SPool.h>
#include "IFramePipeline.h"


SP_NMSPACE_BEG

struct S_API IGameEngine;
struct S_API IRenderer;
struct S_API IViewport;
struct S_API SCommandDescription;
struct S_API ISolid;


// SpeedPoint Render Pipeline (abstract)
struct S_API IRenderPipeline : public IFramePipelineSection
{
public:
	virtual ~IRenderPipeline()
	{
	}

	virtual void Init(IGameEngine* pEngine) = 0;
};


SP_NMSPACE_END