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
#include "Vector3.h"
#include "IObject.h"

SP_NMSPACE_BEG


typedef Vec3d SVelocity;

enum ECollisionShapeType
{
	eCOLLSHAPE_CAPSULE,
	eCOLLSHAPE_SPHERE,
	eCOLLSHAPE_BOX
};

// Summary:
//	Used to describe the form of the collision shape around an object that
//	is used for collision detection.
struct SCollisionShape
{
	double radius;
	double length;
	Vec3d center;
	ECollisionShapeType type;
};

enum EPhysicalBeheaviour
{
	ePHYSBEHAVE_SOLID,
	ePHYSBEHAVE_FOLIAGE,
	ePHYSBEHAVE_LIQUID
};


struct IPhysicalObjectComponent
{
	virtual SCollisionShape& GetCollisionShape() = 0;
	virtual SVelocity& GetVelocity() = 0;
	virtual float GetMass() const = 0;
	virtual void SetMass(float mass) = 0;
	virtual EPhysicalBeheaviour GetBeheaviour() const = 0;
};



struct IPhysical : public IObject
{
	virtual bool IsRenderable() const { return true; }
	virtual bool IsPhysical() const { return true; }

	virtual IRenderableComponent* GetRenderable() const = 0;
	virtual IPhysicalComponent* GetPhysical() const = 0;

	virtual SVelocity& GetVelocity() = 0;
	virtual float GetMass() const = 0;
	virtual void SetMass(float mass) = 0;
	// ...
};



struct S_API IWaterVolume : public IPhysical
{
};

struct S_API IOcean
{
};


SP_NMSPACE_END