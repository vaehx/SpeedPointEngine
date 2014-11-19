//////////////////////////////////////////////////////////////////////////////////
//
// This file is part of the SpeedPointEngine
// Copyright (c) 2011-2014, iSmokiieZz
// ------------------------------------------------------------------------------
// Filename:	ILight.h
// Created:	11/18/2014 by iSmokiieZz
// Description:
// -------------------------------------------------------------------------------
// History:
//
//////////////////////////////////////////////////////////////////////////////////

#pragma once

#include <SPrerequisites.h>
#include <Util\SColor.h>

SP_NMSPACE_BEG

enum ELightType
{
	ELIGHT_OMNIDIRECTIONAL,
	ELIGHT_POINT = ELIGHT_OMNIDIRECTIONAL	
};

struct S_API SLight
{
	SColor color;
	float radius;
	float innerRadius;

	SLight()
		: radius(1.0f),
		innerRadius(0.5f)
	{
	}
};

SP_NMSPACE_END