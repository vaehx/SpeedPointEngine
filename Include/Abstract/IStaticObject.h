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

SP_NMSPACE_BEG

struct S_API ITexture;
struct S_API IIndexBuffer;
struct S_API IVertexBuffer;
struct S_API IGeometry;
struct S_API STransformation;

// Summary:
//	An object that can be rendered
struct S_API IStaticObject
{
	virtual ~IStaticObject()
	{
	}

	virtual IGeometry* GetGeometry() = 0;
	virtual STransformation& GetTransformation() = 0;
};



SP_NMSPACE_END

#endif