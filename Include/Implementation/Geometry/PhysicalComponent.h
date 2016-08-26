#pragma once

#include <SPrerequisites.h>
#include <Abstract\IPhysicalComponent.h>
#include <Abstract\MathGeom.h>
#include <Abstract\IPhysics.h>

SP_NMSPACE_BEG

//
//
//
//	 TODO:  Is this the proper place to put the implementation? Or is it better to seperate it in the OnionPhysics project?
//
//
//

class PhysicalComponent : public IPhysicalComponent, public IPhysObject
{
private:
	IEntity* m_pEntity;
	


	// Should be in Object space. Transform this by the object's transformation matrix to get the world-space collision shape.
	// You can also use world-space coordinates for the shape, but whenever the object moves, you have to manually update the shape vectors.
	SGeomShape m_CollisionShape;

	void ClearPhysicalComponent()
	{
		m_pEntity = 0;
	}

	// IComponent:
public:
	ILINE virtual void SetEntity(IEntity* entity)
	{
		m_pEntity = entity;
	}

	ILINE virtual IEntity* GetEntity() const
	{
		return m_pEntity;
	}

	// IPhysicalComponent:
public:
	virtual ~PhysicalComponent()
	{
		OnRelease();
	}

	virtual void SetCollisionShape(const SGeomShape& shape)
	{
		m_CollisionShape = shape;
	}

	virtual SGeomShape* GetCollisionShape()
	{
		return &m_CollisionShape;
	}

	// IPhysObject:
public:
	virtual void OnRelease()
	{
		if (IS_VALID_PTR(m_pEntity))
			m_pEntity->ReleaseComponent(this);

		ClearPhysicalComponent();
	}

	// IPhysicalComponent + IPhysObject:
public:
	//TODO: Not implemented yet -> Calculate AABB from collision shape
	virtual AABB GetAABB()
	{



		//TODO
		return AABB();




	}
};


SP_NMSPACE_END