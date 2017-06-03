#pragma once

#include "IRenderObject.h"
#include "IGeometry.h"
#include <Renderer\IRenderer.h>
#include <Renderer\IIndexBuffer.h>
#include <Common\SVertex.h>

SP_NMSPACE_BEG

struct S_API I3DEngine;

class S_API CRenderMesh : public IRenderObject
{
protected:
	SRenderDesc m_RenderDesc;
	IGeometry* m_pGeometry;
	
	AABB m_AABB;
	bool m_bBoundBoxInvalid;

	virtual void Clear();

public:
	CRenderMesh();
	virtual ~CRenderMesh() {}

	SResult Init(const SInitialGeometryDesc* pGeomDesc);
	SResult Init(IGeometry* pGeometry);
	SResult SetGeometry(IGeometry* pGeometry);
	IGeometry* GetGeometry();
	virtual SRenderDesc* GetRenderDesc();
	IVertexBuffer* GetVertexBuffer();
	IIndexBuffer* GetIndexBuffer(unsigned int subset = 0);

	// IRenderObject
public:
	virtual AABB GetAABB();
	virtual void SetCustomViewProjMatrix(const Mat44& viewMtx, const Mat44& projMtx);

	// Events:
public:
	virtual void OnInit(const IGeometry* pGeometry) {}
};

SP_NMSPACE_END
