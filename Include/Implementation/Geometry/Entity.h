#pragma once

#include <Abstract\IEntity.h>
#include "Geometry.h"

SP_NMSPACE_BEG

// GameEngine required to call subsystem component factories
struct S_API IGameEngine;


///////////////////////////////////////////////////////////////////////////////////////
//
//			Entity
//
///////////////////////////////////////////////////////////////////////////////////////

class CEntity : public IEntity
{
private:
	IEntitySystem* m_pEntitySystem;
	IComponent* m_pComponents[NUM_COMPONENTS];
	string m_Name;

	Vec3f m_Pos, m_Scale, m_Pivot;
	Quat m_Rot;

	void OnEntityEvent(const SEntityEvent& e);
	void OnEntityTransformEvent();

public:
	CEntity(IEntitySystem* pEntitySystem);
	


	// IEntity:

	ILINE virtual void Clear();

	ILINE virtual const Vec3f& GetPos() const;
	ILINE virtual void SetPos(const Vec3f& pos);

	ILINE virtual const Quat& GetRotation() const;
	ILINE virtual void SetRotation(const Quat& rotation);

	ILINE virtual const Vec3f& GetScale() const;
	ILINE virtual void SetScale(const Vec3f& scale);

	ILINE virtual const Vec3f& GetPivot() const;
	ILINE virtual void SetPivot(const Vec3f& pivot);

	ILINE virtual const char* GetName() const;
	ILINE virtual void SetName(const char* name);

	ILINE virtual IComponent* CreateComponent(EComponentType component);

	ILINE virtual IRenderableComponent* CreateRenderable();
	ILINE virtual IPhysicalComponent* CreatePhysical();

	// Returns NULL if the component was not created
	ILINE virtual IComponent* GetComponent(EComponentType component) const;
	ILINE virtual IRenderableComponent* GetRenderable() const;
	ILINE virtual IPhysicalComponent* GetPhysical() const;

	ILINE virtual void SetComponent(EComponentType type, IComponent* pComponent);
	ILINE virtual void ReleaseComponent(IComponent* pComponent);
};



/*class CRigidBody : public IEntity
{
private:
	IGameEngine* m_pEngine;
	CRigidBodyRenderable m_Renderable;
	PhysicalComponent m_Physical;

	vector<IReferenceObject*> m_RefObjects;

	// IObject:
public:
	virtual ~CRigidBody() { Clear(); }

	ILINE virtual void RecalcBoundBox();

	ILINE virtual IRenderableComponent* GetRenderable() const { return (IRenderableComponent*)&m_Renderable; }
	ILINE virtual IPhysicalComponent* GetPhysical() const { return (IPhysicalComponent*)&m_Physical; }

public:
	CRigidBody();

	SResult Init(IGameEngine* pEngine, const char* name = "RigidBody", SInitialGeometryDesc* pInitialGeom = nullptr);

	void SetVisible(bool visible);

	IGeometry* GetGeometry();

	IMaterial* GetSubsetMaterial(unsigned int subset = 0);
	unsigned int GetSubsetCount() const;

	void Clear();

	IReferenceObject* CreateReferenceObject();
};*/




SP_NMSPACE_END