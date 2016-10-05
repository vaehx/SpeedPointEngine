//////////////////////////////////////////////////////////////////////////////////
//
// This file is part of the SpeedPointEngine
// Copyright (c) 2011-2016, Pascal Rosenkranz
// ------------------------------------------------------------------------------
// Filename:	ILight.h
// Created:		11/18/2014 by Pascal Rosenkranz
// Description:
// -------------------------------------------------------------------------------
// History:
//
//////////////////////////////////////////////////////////////////////////////////

#pragma once

#include "IRenderObject.h"
#include "SColor.h"

SP_NMSPACE_BEG

class S_API CRenderLight : public IRenderObject
{
private:
	SLightDesc m_LightDesc;
};

SP_NMSPACE_END