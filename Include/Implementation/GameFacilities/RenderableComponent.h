#pragma once

#include <Abstract\RenderMesh.h>
#include <Abstract\IEntity.h>
#include <Abstract\I3DEngine.h>
#include <Abstract\Transformable.h>

SP_NMSPACE_BEG

struct S_API I3DEngine;
struct S_API IMaterialManager;

class CRenderableComponent : public IComponent, public CRenderMesh
{
	DEFINE_COMPONENT

private:
	void ClearRenderableComponent();

public:
	CRenderableComponent();
	virtual ~CRenderableComponent();

	// IComponent:
public:
	ILINE virtual const char* GetSerializationType() const { return "Renderable"; };
	ILINE virtual void Serialize(map<string, string>& params) const;
	ILINE virtual void Deserialize(const map<string, string>& params);

	virtual void OnRelease();
	virtual void OnEntityTransformed();


	/*
	// IRenderableComponent:
public:
	ILINE virtual void Init(const SInitialGeometryDesc* geomDesc = nullptr, IMaterialManager* pMatMgr = nullptr);

	virtual void SetVisible(bool visible);

	ILINE virtual IRenderObject* GetRenderObject()
	{
		return (IRenderObject*)this;
	}

	virtual IGeometry* GetGeometry();

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
	virtual void SetTransform(const SMatrix& transform);

	virtual void SetCustomViewProjMatrix(const SMatrix* viewProj);

	// Called by the Renderer System
	virtual void Update();

	// IRenderableComponent + IRenderObject:
public:
	virtual AABB GetAABB();
	virtual IVertexBuffer* GetVertexBuffer();
	virtual IIndexBuffer* GetIndexBuffer(unsigned int subset = 0);
	*/
};

SP_NMSPACE_END
