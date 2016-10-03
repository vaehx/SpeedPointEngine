//////////////////////////////////////////////////////////////////////////////////
//
// This file is part of the SpeedPointEngine
// Copyright (c) 2011-2015, Pascal Rosenkranz
// ------------------------------------------------------------------------------
// Filename:	Animateable.h
// Created:	2/8/2015 by Pascal Rosenkranz
// Description:
// -------------------------------------------------------------------------------
// History:
//
//////////////////////////////////////////////////////////////////////////////////

#pragma once

#include <SPrerequisites.h>

SP_NMSPACE_BEG

struct IGeometry;


struct SAnimationKey
{
};

struct IAnimateableObjectComponent
{
	virtual void Create(IGeometry* pGeom = 0) = 0;
	virtual IGeometry* GetGeometry() = 0;
	virtual SAnimationKey* GetAnimationKey() = 0;
	virtual unsigned int GetAnimationKeyCount() = 0;
	virtual float GetAnimationRunTime() const = 0;
};



SP_NMSPACE_END