// ********************************************************************************************

//	SpeedPoint Physical Object

//	This is a virtual class. You cannot instantiate it.
//	Please use Specific Implementation to instantiate.

//	NOTE: The implementation of this class can be found in the Onion Physics engine

// ********************************************************************************************

#pragma once

#include <Common\SPrerequisites.h>
#include <Common\Vector3.h>
#include <Common\Mat44.h>
#include <Common\MathGeom.h>

SP_NMSPACE_BEG


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
	SGeomShape m_CollisionShape;
	float m_Mass;

public:
	CPhysObject() {};
	virtual ~CPhysObject() {}

	void Init(const SPhysObjectParams& params);

	virtual void Release() = 0;
	virtual bool IsTrash() const = 0;

	void SetCollisionShape(const SGeomShape& shape);
	SGeomShape* GetCollisionShape();

	AABB GetAABB();

	float GetMass() { return m_Mass; }
};

SP_NMSPACE_END
