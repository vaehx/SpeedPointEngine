#pragma once

#include "IEntity.h"
#include "IGeometry.h"

SP_NMSPACE_BEG

struct S_API IRenderObject;
struct S_API IMaterialManager;

struct S_API IRenderableComponent : public IComponent
{
	virtual ~IRenderableComponent()
	{
	}

	// IComponent:
	ILINE virtual EComponentType GetType() const
	{
		return eCOMPONENT_RENDERABLE;
	}	
	// ----

	ILINE virtual void Init(const SInitialGeometryDesc* geomDesc = nullptr, IMaterialManager* pMatMgr = nullptr) = 0;

	ILINE virtual void SetVisible(bool visible) = 0;

	ILINE virtual IRenderObject* GetRenderObject() = 0;

	virtual IGeometry* GetGeometry() = 0;

	virtual IVertexBuffer* GetVertexBuffer() = 0;

	virtual SGeomSubset* GetSubset(unsigned int i) = 0;
	virtual unsigned int GetSubsetCount() const = 0;

	// Returns 0 if the subset was not found!
	virtual IMaterial* GetSubsetMaterial(unsigned int subset = 0) = 0;

	virtual void SetViewProjMatrix(const SMatrix& mtx) = 0;
	virtual void UnsetViewProjMatrix() = 0;
};

SP_NMSPACE_END
