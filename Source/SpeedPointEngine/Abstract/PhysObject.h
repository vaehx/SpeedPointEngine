// ********************************************************************************************

//	SpeedPoint Physical Object

//	This is a virtual class. You cannot instantiate it.
//	Please use Specific Implementation to instantiate.

//	NOTE: The implementation of this class can be found in the Onion Physics engine

// ********************************************************************************************

#pragma once

#include <SPrerequisites.h>
#include "Vector3.h"
#include "BoundBox.h"
#include "MathGeom.h"

SP_NMSPACE_BEG

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

struct SPhysObjectParams
{
	SGeomShape collisionShape;

	SPhysObjectParams()
	{
	}

	SPhysObjectParams(const SGeomShape& collisionShape_)
		: collisionShape(collisionShape_)
	{
	}
};

class S_API CPhysObject
{
protected:
	SPhysicalState m_State;
	SGeomShape m_CollisionShape;

public:
	CPhysObject() {};
	virtual ~CPhysObject() {}

	void Init(const SPhysObjectParams& params);

	virtual void Release() = 0;
	virtual bool IsTrash() const = 0;

	void SetCollisionShape(const SGeomShape& shape);
	SGeomShape* GetCollisionShape();

	AABB GetAABB();
};

SP_NMSPACE_END
