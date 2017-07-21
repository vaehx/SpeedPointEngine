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

namespace geo
{
	struct S_API shape;
}

struct S_API SPhysTerrainParams
{
	Vec3f offset; // translation of vtx (0,0)
	float heightScale; // a value multiplied to each height sample
	unsigned int segments[2]; // number of rows/colums to divide the world dimensions into. The heightmap will be sampled bilinearly.
	float size[2]; // (x,z) world-dimensions of the terrain
	unsigned int maxProxyTreeDepth;

	SPhysTerrainParams()
		: maxProxyTreeDepth(16)
	{
	}
};

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

	// heightmap - one coherent row is w pixels and there are h rows.
	// heightmapSz - (w,h) resolution of the heightmap data
	ILINE virtual void CreateTerrainProxy(const float* heightmap, unsigned int heightmapSz[2], const SPhysTerrainParams& params) = 0;
	ILINE virtual void UpdateTerrainProxy(const float* heightmap, unsigned int heightmapSz[2], const AABB& bounds = AABB()) = 0;
	ILINE virtual void ClearTerrainProxy() = 0;

	ILINE virtual void Update(float fTime) = 0;

	ILINE virtual void Pause(bool pause = true) = 0;
	ILINE virtual bool IsPaused() const = 0;
	ILINE virtual void ShowHelpers(bool show = true) = 0;
	ILINE virtual bool HelpersShown() const = 0;
};

struct S_API IPhysDebugHelper
{
	virtual ~IPhysDebugHelper() {}
	virtual void CreateFromShape(const geo::shape* pshape, const SColor& color = SColor::White()) = 0;
	virtual void UpdateFromShape(const geo::shape* pshape, const AABB& bounds = AABB()) = 0;
	virtual void Show(bool show = true) = 0;
	virtual bool IsShown() const = 0;
	virtual void Clear() = 0;
};

struct S_API IPhysicsDebugRenderer
{
	virtual void VisualizePoint(const Vec3f& p, const SColor& color = SColor::Turqouise(), bool releaseAfterRender = false) = 0;
	virtual void VisualizeVector(const Vec3f& p, const Vec3f& v, const SColor& color = SColor::Turqouise(), bool releaseAfterRender = false) = 0;
	virtual void VisualizePlane(const Vec3f& p, const Vec3f& n, const SColor& color = SColor::Turqouise(), bool releaseAfterRender = false) = 0;
	virtual void VisualizeAABB(const AABB& aabb, const SColor& color = SColor::Turqouise(), bool releaseAfterRender = false) = 0;
	virtual void VisualizeBox(const OBB& obb, const SColor& color = SColor::Turqouise(), bool releaseAfterRender = false) = 0;
	
	virtual IPhysDebugHelper* CreateHelper() const = 0;
};

SP_NMSPACE_END

#endif
