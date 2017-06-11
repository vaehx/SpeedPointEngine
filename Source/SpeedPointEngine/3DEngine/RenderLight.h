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

//////////////////////////////////////////////////////////////////////////////////

enum S_API ELightType
{
	eLIGHT_TYPE_OMNIDIRECTIONAL = 0, // Point light
};

inline const char* GetLightTypeName(ELightType type)
{
	switch (type)
	{
	case eLIGHT_TYPE_OMNIDIRECTIONAL: return "eLIGHT_TYPE_OMNIDIRECTIONAL";
	default: return "??";
	}
}

//////////////////////////////////////////////////////////////////////////////////

struct S_API SLightParams
{
	ELightType type;
	Vec3f position;
	float radius;

	SColor intensity; // spectral intensity
	float decay;
};

//////////////////////////////////////////////////////////////////////////////////

class S_API CRenderLight : public IRenderObject
{
protected:
	SLightParams m_Params;

private:
	// We don't use custom render descs for lights
	virtual SRenderDesc* GetRenderDesc() { return 0; }

public:
	CRenderLight();

	virtual AABB GetAABB();
	virtual void SetCustomViewProjMatrix(const Mat44& viewMtx, const Mat44& projMtx);
	SLightParams& GetParams() { m_Params; }
};

SP_NMSPACE_END