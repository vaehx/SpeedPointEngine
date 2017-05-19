//////////////////////////////////////////////////////////////////////////////////
//
// This file is part of the SpeedPointEngine
// Copyright (c) 2011-2014, iSmokiieZz
// ------------------------------------------------------------------------------
// Filename:	IPhysics.h
// Created:	8/12/2014 by iSmokiieZz
// Description:
// -------------------------------------------------------------------------------
// History:
//
//////////////////////////////////////////////////////////////////////////////////

#ifndef __iphysics_h__
#define __iphysics_h__

#if _MSC_VER > 1000
#pragma once
#endif

#include "PhysObject.h"
#include <Common\ComponentPool.h>
#include <Common\BoundBox.h>
#include <Common\SColor.h>
#include <Common\SPrerequisites.h>

SP_NMSPACE_BEG

struct S_API IPhysics
{
protected:
	virtual void SetPhysObjectPool(IComponentPool<PhysObject>* pPool) = 0;

public:
	virtual ~IPhysics() {}

	template<class PhysObjImpl>
	ILINE void CreatePhysObjectPool()
	{
		SetPhysObjectPool(new ComponentPool<PhysObject, PhysObjImpl>());
	}

	ILINE virtual PhysObject* CreatePhysObject() = 0;
	ILINE virtual void ClearPhysObjects() = 0;

	ILINE virtual void Update(float fTime) = 0;
};

struct S_API IPhysicsDebugRenderer
{
	virtual void VisualizePoint(const Vec3f& p, const SColor& color = SColor::Turqouise(), bool releaseAfterRender = false) = 0;
	virtual void VisualizeVector(const Vec3f& p, const Vec3f& v, const SColor& color = SColor::Turqouise(), bool releaseAfterRender = false) = 0;
	virtual void VisualizePlane(const Vec3f& p, const Vec3f& n, const SColor& color = SColor::Turqouise(), bool releaseAfterRender = false) = 0;
};

SP_NMSPACE_END

#endif
