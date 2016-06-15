#pragma once

#include "Geometry.h"
#include <Abstract\IRenderableComponent.h>
#include <Abstract\Transformable.h>

SP_NMSPACE_BEG

struct S_API I3DEngine;
struct S_API IMaterialManager;

class CRenderableComponent : public IRenderableComponent
{
private:
	I3DEngine* m_pRenderer;
	IMaterialManager* m_pMatMgr;
	SRenderObject* m_pRO;
	IEntity* m_pEntity;
	Geometry m_Geometry;
	bool m_bVisible;	

public:
	// Material Manager required to get default materials
	CRenderableComponent(I3DEngine* pRenderer, IMaterialManager* pMatMgr);

	virtual ~CRenderableComponent() {}

	// IComponent:
	virtual void Init(IEntity* pEntity);
	// --

	virtual void Clear();

	virtual void SetVisible(bool visible);

	ILINE virtual SRenderObject* GetRenderObject() const
	{
		return m_pRO;
	}

	virtual IGeometry* GetGeometry();

	virtual IVertexBuffer* GetVertexBuffer();

	virtual SGeomSubset* GetSubset(unsigned int i);
	virtual unsigned int GetSubsetCount() const;

	virtual IMaterial* GetSubsetMaterial(unsigned int subset = 0);

	virtual void SetViewProjMatrix(const SMatrix& mtx);
	virtual void UnsetViewProjMatrix();
};

SP_NMSPACE_END
