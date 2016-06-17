#pragma once

#include "Geometry.h"
#include <Abstract\IRenderableComponent.h>
#include <Abstract\I3DEngine.h>
#include <Abstract\Transformable.h>

SP_NMSPACE_BEG

struct S_API I3DEngine;
struct S_API IMaterialManager;

class CRenderableComponent : public IRenderableComponent, public IRenderObject
{
private:
	I3DEngine* m_pRenderer;
	IMaterialManager* m_pMatMgr;
	IEntity* m_pEntity;
	Geometry m_Geometry;
	SRenderDesc m_RenderDesc;
	bool m_bVisible;	

public:
	// Material Manager required to get default materials
	CRenderableComponent();

	virtual ~CRenderableComponent();

	// IComponent:
public:
	virtual void Init();
	virtual IEntity* GetEntity() const;
	virtual void SetEntity(IEntity* entity);

	// IRenderableComponent:
public:
	virtual void Clear();

	virtual void SetVisible(bool visible);

	ILINE virtual IRenderObject* GetRenderObject()
	{
		return (IRenderObject*)this;
	}

	virtual IGeometry* GetGeometry();

	virtual IVertexBuffer* GetVertexBuffer();

	virtual SGeomSubset* GetSubset(unsigned int i);
	virtual unsigned int GetSubsetCount() const;

	virtual IMaterial* GetSubsetMaterial(unsigned int subset = 0);

	virtual void SetViewProjMatrix(const SMatrix& mtx);
	virtual void UnsetViewProjMatrix();

	// IRenderObject:
public:
	virtual const AABB& GetAABB() const;

	virtual SRenderDesc* GetRenderDesc();

	// Called by the Renderer System
	virtual void Update();
};

SP_NMSPACE_END
