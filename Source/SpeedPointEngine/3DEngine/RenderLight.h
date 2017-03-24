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
#include <Renderer\IRenderer.h>
#include <Common\SColor.h>

SP_NMSPACE_BEG

class S_API CRenderLight : public IRenderObject
{
private:
	SLightDesc m_LightDesc;

public:
	virtual AABB GetAABB();
	virtual SRenderDesc* GetRenderDesc();
	virtual void SetCustomViewProjMatrix(const SMatrix* viewProj);
};

SP_NMSPACE_END