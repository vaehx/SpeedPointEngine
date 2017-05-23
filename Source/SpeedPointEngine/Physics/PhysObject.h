///////////////////////////////////////////////////////////////////////////////////////////////////////////////////////
//
//	SpeedPoint Game Engine
//	Copyright (c) 2011-2017 Pascal Rosenkranz, All rights reserved.
//
///////////////////////////////////////////////////////////////////////////////////////////////////////////////////////

#pragma once

#include <Common\SPrerequisites.h>
#include <Common\Vector3.h>
#include <Common\Quaternion.h>
#include <Common\Mat44.h>
#include <Common\geo.h>

SP_NMSPACE_BEG

struct S_API SPhysObjectState
{
	float damping;

	Vec3f centerOffs; // object position -> center of mass

	Vec3f pos; // center of mass
	Vec3f v; // linear velocity
	Vec3f P; // linear momentum

	Quat rotation; // around axis through center of mass
	Vec3f w; // angular velocity
	Vec3f L; // angular momentum

	float M; // mass
	float Minv; // 1 / mass
	Mat33 Iinv;
	Mat33 Ibodyinv;
	float V; // volume
	bool gravity;
};

class S_API PhysObject
{
private:
	bool m_bTrash;
	AABB m_AABB;
	geo::shape* m_pTransformedShape; // collision shape, in world space

	void RecalculateInertia();

protected:
	geo::shape* m_pShape; // collision shape, in object space
	SPhysObjectState m_State;
	Vec3f m_Scale;

	void Clear();

public:
	PhysObject();
	virtual ~PhysObject();

	virtual void Release();
	virtual bool IsTrash() const;

	template<typename T>
	void SetCollisionShape(const T& shape = T())
	{
		if (m_pShape) delete m_pShape;
		if (m_pTransformedShape) delete m_pTransformedShape;
		m_pShape = new T(shape);
		m_pTransformedShape = new T(shape);
		RecalculateInertia();
	}

	void Update(float fTime);
	const AABB& GetAABB() const { return m_AABB; }
	const geo::shape* GetTransformedCollisionShape() const { return m_pTransformedShape; }
	const geo::shape* GetCollisionShape() const { return m_pShape; }
	SPhysObjectState* GetState() { return &m_State; }
	void SetMass(float m) { m_State.M = m; m_State.Minv = 1.0f / m; }
	void SetUnmoveable();

	// These are implemented by the component and synchronize m_Pos, m_Rotation and m_Scale with the one of the entity
	virtual void OnSimulationPrepare() {};
	virtual void OnSimulationFinished() {};

	virtual void OnIntersection(const geo::SIntersection& contact, const PhysObject* other) {};
};

SP_NMSPACE_END
