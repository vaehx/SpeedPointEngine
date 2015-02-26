//////////////////////////////////////////////////////////////////////////////////////////////////////
//
//	This file is part of the SpeedPoint Engine.
//	Copyright (c) 2011-2015 Pascal Rosenkranz
//
//////////////////////////////////////////////////////////////////////////////////////////////////////

#pragma once

#include <SPrerequisites.h>
#include <Util\SColor.h>


SP_NMSPACE_BEG

struct S_API IRenderer;

struct SPixelPosition
{
	unsigned int x, y;
};

struct SFontConstantsBuffer
{
	unsigned int screenResolution[2];
	float struct_padding[2];
};

struct IFont
{	
};

enum EFontSize
{
	eFONTSIZE_NORMAL,
	eFONTSIZE_MEDIUM,
	eFONTSIZE_LARGE
};

struct IFontRenderer
{
	virtual ~IFontRenderer()
	{
	}

	virtual SResult Init(IRenderer* pRenderer) = 0;

	virtual void BeginRender() = 0;
	virtual void RenderText(const char* text, const SColor& color, const SPixelPosition& pixelPos,
		EFontSize fontSize = eFONTSIZE_NORMAL, bool alignRight = false) = 0;

	virtual void EndRender() = 0;

	virtual void Clear() = 0;
};


SP_NMSPACE_END