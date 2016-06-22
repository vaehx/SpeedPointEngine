//////////////////////////////////////////////////////////////////////////////////
//
// This file is part of the SpeedPointEngine
// Copyright (c) 2011-2015, iSmokiieZz
// ------------------------------------------------------------------------------
// Filename:	Physical.h
// Created:	2/8/2015 by iSmokiieZz
// Description:
// -------------------------------------------------------------------------------
// History:
//
//////////////////////////////////////////////////////////////////////////////////

#pragma once

#include <SPrerequisites.h>
#include "IEntity.h"
#include "Vector3.h"

SP_NMSPACE_BEG

struct S_API SGeomShape;

typedef SGeomShape SCollisionShape;

enum EPhysicalBeheaviour
{
	ePHYSBEHAVE_SOLID,
	ePHYSBEHAVE_FOLIAGE,
	ePHYSBEHAVE_LIQUID
};

struct SPhysicalState
{
	Vec3f x; // x(t) - Position
	Vec3f v; // v(t) - Linear velocity
	// ... linear momentum, force, rotation, torque, angular velocity, angular momentum, inverse inertia tensor, ...
};

struct IPhysicalComponent : public IComponent
{
	virtual ~IPhysicalComponent()
	{
	}

	virtual EComponentType GetType() const
	{
		return eCOMPONENT_PHYSICAL;
	}

	virtual void SetCollisionShape(const SGeomShape& shape) = 0;
	virtual SGeomShape* GetCollisionShape() = 0;
};



SP_NMSPACE_END