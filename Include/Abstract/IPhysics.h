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

#include <SPrerequisites.h>
#include "ComponentPool.h"
#include "BoundBox.h"

SP_NMSPACE_BEG

struct S_API IPhysObject
{
	virtual void OnRelease() = 0;
	virtual AABB GetAABB() = 0;
/*
	// Enable the physics calculation
	virtual void EnablePhysics(void) = 0;

	// Disable the physics calculation
	virtual void DisablePhysics(void) = 0;

	// Get a pointer to the current damage state
	virtual float* Damage(void) = 0;

	// Apply damage to this physical and return resulting damage
	virtual float* Damage(const float& dmg) = 0;

	// Get pointer to the current heat
	virtual float* Heat(void) = 0;

	// Apply heat to the physical and return resulting heat
	virtual float* Heat(const float& heat) = 0;

	// Tick the physics calculation. Will not to anything when physics disabled
	virtual SResult TickPhysics(float fTime) = 0;
	*/
};



struct S_API IPhysics
{
protected:
	virtual void SetPhysObjectPool(IComponentPool<IPhysObject>* pPool) = 0;

public:
	virtual ~IPhysics() {}

	template<class PhysObjImpl>
	ILINE void CreateRenderObjectPool()
	{
		SetPhysObjectPool(new ComponentPool<IPhysObject, PhysObjImpl>());
	}

	// Summary:
	//	Finds and returns an empty PhysObject
	ILINE virtual IPhysObject* GetPhysObjects() = 0;

	// Summary:
	//	Releases the passed object in the pool and sets the pointer to 0
	ILINE virtual void ReleasePhysObject(IPhysObject** pObject) = 0;

	ILINE virtual void ClearPhysObjects() = 0;


	// Ideas:
	

	//	- virtual SResult OnIntersection(IPhysicsObject* objA, IPhysicsObject* objB, const SIntersectionInfo& info) = 0;

};


SP_NMSPACE_END

#endif