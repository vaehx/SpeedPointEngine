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

#include "SPrerequisites.h"
#include "ComponentPool.h"
#include "BoundBox.h"
#include "PhysObject.h"

SP_NMSPACE_BEG



struct S_API IPhysics
{
protected:
	virtual void SetPhysObjectPool(IComponentPool<CPhysObject>* pPool) = 0;

public:
	virtual ~IPhysics() {}

	template<class PhysObjImpl>
	ILINE void CreatePhysObjectPool()
	{
		SetPhysObjectPool(new ComponentPool<CPhysObject, PhysObjImpl>());
	}

	ILINE virtual CPhysObject* CreatePhysObject(const SPhysObjectParams& params = SPhysObjectParams()) = 0;

	// Summary:
	//	Releases the passed object in the pool and sets the pointer to 0
	ILINE virtual void ReleasePhysObject(CPhysObject** pObject) = 0;

	ILINE virtual void ClearPhysObjects() = 0;

	ILINE virtual void Update() = 0;


	// Ideas:
	

	//	- virtual SResult OnIntersection(IPhysicsObject* objA, IPhysicsObject* objB, const SIntersectionInfo& info) = 0;

};


SP_NMSPACE_END

#endif