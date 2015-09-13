#pragma once

#include <Abstract\IObject.h>
#include <Abstract\IRenderer.h>
#include "Geometry.h"

SP_NMSPACE_BEG



///////////////////////////////////////////////////////////////////////////////////////
//
//			Rigid Body
//
///////////////////////////////////////////////////////////////////////////////////////

class CRigidBodyRenderable : public IRenderableComponent
{
private:
	SRenderDesc m_RenderDesc;
	STransformable* m_pTransformable;
	Geometry m_Geometry;
	bool m_bVisible;

public:
	CRigidBodyRenderable();

	virtual ~CRigidBodyRenderable() {}

	virtual void Clear();

	virtual void SetVisible(bool visible);	

	ILINE virtual void GetUpdatedRenderDesc(SRenderDesc* pDest);

	virtual IGeometry* GetGeometry();

	virtual IVertexBuffer* GetVertexBuffer();

	virtual SGeomSubset* GetSubset(unsigned int i);
	virtual unsigned int GetSubsetCount() const;

	virtual IMaterial* GetSubsetMaterial(unsigned int subset = 0);

	virtual void SetViewProjMatrix(const SMatrix& mtx);
	virtual void UnsetViewProjMatrix();

public:
	void InitRenderDesc(IMaterialManager* pMatMgr, STransformable* pTransformable);
};


class CRigidBody : public IEntity
{
private:
	IGameEngine* m_pEngine;
	CRigidBodyRenderable m_Renderable;
	IPhysicalComponent* m_pPhysical;

	vector<IReferenceObject*> m_RefObjects;

	// IObject:
public:
	virtual ~CRigidBody() { Clear(); }

	ILINE virtual EntityType GetType() const { return SP_ENTITY_RENDERABLE | SP_ENTITY_PHYSICAL; }

	ILINE virtual void RecalcBoundBox();

	ILINE virtual IRenderableComponent* GetRenderable() const { return (IRenderableComponent*)&m_Renderable; }
	ILINE virtual IPhysicalComponent* GetPhysical() const { return m_pPhysical; }

public:
	CRigidBody();

	SResult Init(IGameEngine* pEngine, const char* name = "RigidBody", SInitialGeometryDesc* pInitialGeom = nullptr);

	void SetVisible(bool visible);

	IGeometry* GetGeometry();

	IMaterial* GetSubsetMaterial(unsigned int subset = 0);
	unsigned int GetSubsetCount() const;

	void Clear();

	IReferenceObject* CreateReferenceObject();
};




SP_NMSPACE_END