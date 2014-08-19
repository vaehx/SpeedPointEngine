//////////////////////////////////////////////////////////////////////////////////
//
// This file is part of the SpeedPointEngine
// Copyright (c) 2011-2014, iSmokiieZz
// ------------------------------------------------------------------------------
// Filename:	IVisibleObject.h
// Created:	8/12/2014 by iSmokiieZz
// Description:
// -------------------------------------------------------------------------------
// History:
//
//////////////////////////////////////////////////////////////////////////////////

#ifndef __ivisibleobject_h__
#define __ivisibleobject_h__

#if _MSC_VER > 1000
#pragma once
#endif

#include <SPrerequisites.h>
#include "IGeometrical.h"
#include <Util\STransformable.h>

SP_NMSPACE_BEG

struct S_API ITexture;
struct S_API IIndexBuffer;
struct S_API IVertexBuffer;

// Summary:
//	An object that can be rendered
struct S_API IVisibleObject : public IGeometrical, public STransformable
{
	virtual ~IVisibleObject()
	{
	}
};



SP_NMSPACE_END

#endif