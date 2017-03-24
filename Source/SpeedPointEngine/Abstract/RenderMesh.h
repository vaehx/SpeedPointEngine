#pragma once

#include "IRenderObject.h"
#include "IRenderer.h"
#include "SVertex.h"
#include "IIndexBuffer.h"
#include "IGeometry.h"

SP_NMSPACE_BEG

struct S_API SRenderMeshParams
{
	SInitialGeometryDesc* pGeomDesc;
#ifdef _DEBUG
	string _name;
#endif

	SRenderMeshParams() : pGeomDesc(0) {}
	SRenderMeshParams(SInitialGeometryDesc* pGeomDesc_) : pGeomDesc(pGeomDesc_) {}
};

class S_API CRenderMesh : public IRenderObject
{
protected:
	SRenderDesc m_RenderDesc;
	IGeometry* m_pGeometry; // composition
	
	AABB m_AABB;
	bool m_bBoundBoxInvalid;

	virtual void Clear();

public:
	CRenderMesh();
	virtual ~CRenderMesh() {}

	SResult Init(const SRenderMeshParams& params);

	virtual SRenderDesc* GetRenderDesc();
	IGeometry* GetGeometry();
	IVertexBuffer* GetVertexBuffer();
	IIndexBuffer* GetIndexBuffer(unsigned int subset = 0);

	// IRenderObject
public:
	virtual AABB GetAABB();
	virtual void SetCustomViewProjMatrix(const SMatrix* viewProj);	

	// Events:
public:
	virtual void OnInit(const SRenderMeshParams& params) {}
};

SP_NMSPACE_END
