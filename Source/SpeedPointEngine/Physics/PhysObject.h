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

struct S_API IPhysDebugHelper;
class S_API PhysTerrain;

enum S_API EPhysObjectType
{
	ePHYSOBJ_TYPE_NORMAL,
	ePHYSOBJ_TYPE_TERRAIN
};

enum S_API EPhysObjectBehavior
{
	ePHYSOBJ_BEHAVIOR_RIGID_BODY = 0,
	ePHYSOBJ_BEHAVIOR_STATIC,
	ePHYSOBJ_BEHAVIOR_LIVING
};

struct S_API SPhysObjectState
{
	float damping;

	Vec3f centerOfMass; // center point of mass in object space
	Vec3f pos; // center point of mass in world-space
	Vec3f v; // linear velocity
	Vec3f P; // linear momentum
	Vec3f F; // force "generated by object itself" via friction
	bool livingMoves;
	bool livingOnGround;

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

struct S_API SProxyPart
{
	AABB aabb;
	AABB aabbworld;
	geo::shape* pshape;
	geo::shape* pshapeworld; // shape transformed into world space
	IPhysDebugHelper* phelper;

	SProxyPart()
		: pshape(0),
		pshapeworld(0),
		phelper(0)
	{
	}
};

// Rigid Body
class S_API PhysObject
{
private:
	bool m_bTrash;

	void RecalculateInertia();

protected:
	// Proxy describing the collision geometry of this object
	// TODO: Convert this to a hierarchical accelerated structure (BVH)
	SProxyPart m_Proxy;

	SPhysObjectState m_State;
	Vec3f m_Scale;
	bool m_bHelperShown;
	EPhysObjectBehavior m_Behavior;

	void Clear();

public:
	PhysObject();
	virtual ~PhysObject();

	virtual void Release();
	virtual bool IsTrash() const;

	virtual EPhysObjectType GetType() const { return ePHYSOBJ_TYPE_NORMAL; }

	void SetBehavior(EPhysObjectBehavior behavior);
	EPhysObjectBehavior GetBehavior() const { return m_Behavior; }

	void Update(float fTime);
	void ResolveLivingContact(const PhysObject* pother, const geo::SIntersection* pinters, float fTime);

	const AABB& GetAABB() const { return m_Proxy.aabbworld; }
	
	// Don't use this for meshes
	template<typename T> void SetProxy(const T& shape = T())
	{
		SetProxyPtr(new T(shape));
	}

	// pshape can be 0 to clear proxy.
	// !! If pshape is a mesh, its tree must be initialized already
	void SetProxyPtr(geo::shape* pshape);

	void SetMeshProxy(const Vec3f* ppoints, u32 npoints, const u32* pindices, u32 nindices, bool octree = true, u16 maxTrisPerLeaf = 8);
	const SProxyPart& GetProxy() const { return m_Proxy; }

	SPhysObjectState* GetState() { return &m_State; }
	void SetMass(float m) { m_State.M = m; m_State.Minv = 1.0f / m; }

	void ShowHelper(bool show = true);

	// These are implemented by the component and synchronize m_Pos, m_Rotation and m_Scale with the one of the entity
	virtual void OnSimulationPrepare() {};
	virtual void OnSimulationFinished() {};

	virtual void OnIntersection(const geo::SIntersection& contact, const PhysObject* other) {};
};

SP_NMSPACE_END
