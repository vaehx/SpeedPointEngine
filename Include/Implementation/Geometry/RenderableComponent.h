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
	IEntity* m_pEntity;
	Geometry m_Geometry;
	SRenderDesc m_RenderDesc;
	bool m_bVisible;

	AABB m_AABB;
	bool m_bBoundBoxInvalid;

	void ClearRenderableComponent();

public:
	// Material Manager required to get default materials
	CRenderableComponent();

	virtual ~CRenderableComponent();

	// IComponent:
public:	
	virtual IEntity* GetEntity() const;
	virtual void SetEntity(IEntity* entity);
	ILINE virtual void OnEntityEvent(const SEntityEvent& e);

	// IRenderableComponent:
public:
	ILINE virtual void Init(const SInitialGeometryDesc* geomDesc = nullptr, IMaterialManager* pMatMgr = nullptr);

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
	virtual void OnRelease();
	virtual void SetRenderer(I3DEngine* p3DEngine);

	virtual SRenderDesc* GetRenderDesc();

	// Called by the Renderer System
	virtual void Update();

	// IRenderableComponent + IRenderObject:
public:
	virtual AABB GetAABB();
};

SP_NMSPACE_END
