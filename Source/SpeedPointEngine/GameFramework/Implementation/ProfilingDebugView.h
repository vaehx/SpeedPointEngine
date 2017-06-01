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
	bool m_bShow;

public:
	ProfilingDebugView();

	void Update(IRenderer* renderer);
	void Show(bool show = true);
	bool IsShown() const { return m_bShow; }
	void Hide() { Show(false); }
};

SP_NMSPACE_END
