//////////////////////////////////////////////////////////////////////////////////////////////////
//
//	SpeedPoint Game Engine
//	Copyright (c) 2011-2017 Pascal Rosenkranz, All rights reserved.
//
//////////////////////////////////////////////////////////////////////////////////////////////////

#pragma once

#include <Common\SPrerequisites.h>

SP_NMSPACE_BEG

struct S_API IGameEngine;

struct S_API IApplication
{
	virtual ~IApplication() {}

	virtual void OnInit(IGameEngine* pGameEngine) = 0;

	// Summary:
	//	Called whenever the engine is going to execute dynamics stuff
	virtual void Update(float fLastFrameTime) = 0;

	// Summary:
	//	Do all Render() calls inside this function.
	virtual void Render() = 0;
};

SP_NMSPACE_END
