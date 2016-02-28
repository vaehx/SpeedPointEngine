#pragma once

#include <SPrerequisites.h>
#include <Abstract\Physical.h>
#include <Abstract\MathGeom.h>

SP_NMSPACE_BEG

//
//
//
//	 TODO:  Is this the proper place to put the implementation? Or is it better to seperate it in the OnionPhysics project?
//
//
//

class PhysicalComponent : public IPhysicalComponent
{
private:
	// Should be in Object space. Transform this by the object's transformation matrix to get the world-space collision shape.
	// You can also use world-space coordinates for the shape, but whenever the object moves, you have to manually update the shape vectors.
	SGeomShape m_CollisionShape;

public:
	virtual ~PhysicalComponent()
	{
	}

	virtual void SetCollisionShape(const SGeomShape& shape)
	{
		m_CollisionShape = shape;
	}

	virtual SGeomShape* GetCollisionShape()
	{
		return &m_CollisionShape;
	}
};


SP_NMSPACE_END