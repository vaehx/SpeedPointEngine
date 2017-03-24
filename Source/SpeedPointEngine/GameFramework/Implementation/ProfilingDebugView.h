//////////////////////////////////////////////////////////////////////////////////////////////////
//
//	SpeedPoint Game Engine
//	Copyright (c) 2011-2017 Pascal Rosenkranz, All rights reserved.
//
//////////////////////////////////////////////////////////////////////////////////////////////////

#pragma once

#include <Common\SColor.h>
#include <Common\SPrerequisites.h>

SP_NMSPACE_BEG

struct S_API SFontRenderSlot;
struct S_API IRenderer;
class S_API ProfilingSystem;

// Used to render profiling information to the screen
class ProfilingDebugView
{
private:
	SFontRenderSlot* m_pCamStatus;
	SFontRenderSlot* m_pFPS;
	SFontRenderSlot* m_pTerrain;

public:
	ProfilingDebugView();

	void Update(IRenderer* renderer);
};

SP_NMSPACE_END
