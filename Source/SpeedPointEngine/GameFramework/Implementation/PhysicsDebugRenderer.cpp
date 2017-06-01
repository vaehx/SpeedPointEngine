#include "PhysicsDebugRenderer.h"
#include <3DEngine\Implementation\C3DEngine.h>
#include <3DEngine\CHelper.h>
#include <Common\geo.h>

SP_NMSPACE_BEG

using namespace geo;

///////////////////////////////////////////////////////////////////////////////////////////////////////////////////////

S_API CPhysDebugHelper::CPhysDebugHelper()
	: m_pHelper(0),
	m_ShapeType(eSHAPE_UNKNOWN)
{
}

void MakeCapsule(SInitialGeometryDesc& geom, const Vec3f& bottom, const Vec3f& top, float radius, unsigned int segments, unsigned int rings)
{
	geom.primitiveType = PRIMITIVE_TYPE_TRIANGLELIST;

	auto subset = geom.GetZeroSubset();

	unsigned int numMantleVerts = (segments + 1) * 2;
	unsigned int numCapVerts = (segments + 1) * (rings + 1);
	geom.nVertices = numMantleVerts + numCapVerts * 2;
	geom.pVertices = new SVertex[geom.nVertices];

	unsigned int numMantleIndices = segments * 6;
	unsigned int numCapIndices = segments * rings * 6;
	subset->nIndices = numMantleIndices + numCapIndices * 2;
	subset->pIndices = new SIndex[subset->nIndices];

	Vec3f axis = top - bottom;
	Vec3f naxis = axis.Normalized();
	Vec3f perp = ((Vec3f(-naxis.y, -naxis.z, naxis.x) ^ naxis) ^ naxis).Normalized() * radius;

	float alpha, dAlpha = (2.0f * SP_PI) / (float)segments;
	float beta, dBeta = (SP_PI * 0.5f) / (float)rings;
	Vec3f pbottom, ptop;
	Vec3f cbottom, ctop;
	for (unsigned int seg = 0; seg <= segments; ++seg)
	{
		alpha = dAlpha * seg;
		pbottom = bottom + Quat::FromAxisAngle(naxis, alpha) * perp;
		ptop = pbottom + axis;

		Vec3f n = (pbottom - bottom).Normalized();
		Vec3f t = (n ^ naxis).Normalized();

		unsigned int mantleVtx = seg * 2;
		geom.pVertices[mantleVtx] = SVertex(pbottom.x, pbottom.y, pbottom.z, n.x, n.y, n.z, t.x, t.y, t.z);
		geom.pVertices[mantleVtx + 1] = SVertex(ptop.x, ptop.y, ptop.z, n.x, n.y, n.z, t.x, t.y, t.z);

		if (seg < segments)
		{
			unsigned int i = seg * 6;
			subset->pIndices[i + 0] = mantleVtx;
			subset->pIndices[i + 1] = mantleVtx + 1;
			subset->pIndices[i + 2] = mantleVtx + 1 + 2;
			subset->pIndices[i + 3] = mantleVtx;
			subset->pIndices[i + 4] = mantleVtx + 1 + 2;
			subset->pIndices[i + 5] = mantleVtx + 2;
		}

		for (unsigned int ring = 0; ring <= rings; ++ring)
		{
			beta = dBeta * ring;
			cbottom = bottom + Quat::FromAxisAngle(t, -beta) * (pbottom - bottom);
			ctop = top + Quat::FromAxisAngle(t, beta) * (ptop - top);

			unsigned int capVtxBottom = numMantleVerts + (seg * (rings + 1)) + ring;
			n = Vec3Normalize(cbottom - bottom);
			geom.pVertices[capVtxBottom] = SVertex(cbottom.x, cbottom.y, cbottom.z, n.x, n.y, n.z, t.x, t.y, t.z);

			unsigned int capVtxTop = numMantleVerts + numCapVerts + (seg * (rings + 1)) + ring;
			n = Vec3Normalize(ctop - top);
			geom.pVertices[capVtxTop] = SVertex(ctop.x, ctop.y, ctop.z, n.x, n.y, n.z, t.x, t.y, t.z);

			if (seg < segments && ring < rings)
			{
				unsigned int i = numMantleIndices + ((seg * rings) + ring) * 6;
				subset->pIndices[i + 0] = capVtxBottom;
				subset->pIndices[i + 1] = capVtxBottom + 1;
				subset->pIndices[i + 2] = capVtxBottom + (rings + 1) + 1;
				subset->pIndices[i + 3] = capVtxBottom;
				subset->pIndices[i + 4] = capVtxBottom + (rings + 1) + 1;
				subset->pIndices[i + 5] = capVtxBottom + (rings + 1);

				i = numMantleIndices + numCapIndices + ((seg * rings) + ring) * 6;
				subset->pIndices[i + 0] = capVtxTop;
				subset->pIndices[i + 1] = capVtxTop + 1;
				subset->pIndices[i + 2] = capVtxTop + (rings + 1) + 1;
				subset->pIndices[i + 3] = capVtxTop;
				subset->pIndices[i + 4] = capVtxTop + (rings + 1) + 1;
				subset->pIndices[i + 5] = capVtxTop + (rings + 1);
			}
		}
	}
}


S_API void CPhysDebugHelper::CreateFromShape(const geo::shape* pshape, const SColor& color)
{
	if (!pshape)
		return;

	Clear();

	I3DEngine* p3DEngine = C3DEngine::Get();
	bool releaseAfterRender = false;

	m_ShapeType = pshape->GetType();

	switch (pshape->GetType())
	{
	case eSHAPE_RAY:
		{
			const ray* pray = (const ray*)pshape;
			m_pHelper = p3DEngine->AddHelper<CVectorHelper>(CVectorHelper::Params(pray->p, pray->v, 50.0f), releaseAfterRender);
			break;
		}
	case eSHAPE_PLANE:
		{
			const plane* pplane = (const plane*)pshape;
			CPlaneHelper::Params params;
			params.n = pplane->n;
			params.d = pplane->d;
			params.hsize = 10.0f;
			m_pHelper = p3DEngine->AddHelper<CPlaneHelper>(params, releaseAfterRender);
			break;
		}
	case eSHAPE_SPHERE:
		{
			const sphere* psphere = (const sphere*)pshape;
			m_pHelper = p3DEngine->AddHelper<CSphereHelper>(CSphereHelper::Params(psphere->c, psphere->r), releaseAfterRender);
			break;
		}
	case eSHAPE_CYLINDER:
		{
			const cylinder* pcyl = (const cylinder*)pshape;
			CCylinderHelper::Params params;
			params.p[0] = pcyl->p[0];
			params.p[1] = pcyl->p[1];
			params.r = pcyl->r;
			m_pHelper = p3DEngine->AddHelper<CCylinderHelper>(params, releaseAfterRender);
			break;
		}
	case eSHAPE_CAPSULE:
		{
			const capsule* pcapsule = (const capsule*)pshape;

			SInitialGeometryDesc geomDesc;
			MakeCapsule(geomDesc, pcapsule->c - pcapsule->hh * pcapsule->axis, pcapsule->c + pcapsule->hh * pcapsule->axis, pcapsule->r, 24, 10);

			CDynamicMeshHelper::Params params;
			params.numVertices = geomDesc.nVertices;
			params.pVertices = geomDesc.pVertices;
			params.numIndices = geomDesc.GetZeroSubset()->nIndices;
			params.pIndices = geomDesc.GetZeroSubset()->pIndices;
			params.topology = geomDesc.primitiveType;

			m_pHelper = p3DEngine->AddHelper<CDynamicMeshHelper>(params, releaseAfterRender);

			delete[] geomDesc.pVertices;
			delete[] geomDesc.GetZeroSubset()->pIndices;
			break;
		}
	case eSHAPE_TRIANGLE:
		{
			const triangle* ptri = (const triangle*)pshape;
			CDynamicMeshHelper::Params params;
			params.topology = PRIMITIVE_TYPE_TRIANGLELIST;
			params.numVertices = 3;
			params.pVertices = new SVertex[params.numVertices];
			params.numIndices = 3;
			params.pIndices = new SIndex[params.numIndices];

			for (int i = 0; i < 3; ++i)
			{
				params.pVertices[i] = SVertex(ptri->p[i].x, ptri->p[i].y, ptri->p[i].z);
				params.pIndices[i] = i;
			}

			m_pHelper = p3DEngine->AddHelper<CDynamicMeshHelper>(params, releaseAfterRender);
			delete[] params.pVertices;
			delete[] params.pIndices;
			break;
		}
	case eSHAPE_MESH:
		{
			const mesh* pmesh = dynamic_cast<const mesh*>(pshape);
			if (!pmesh || !pmesh->points || !pmesh->indices || pmesh->num_points == 0 || pmesh->num_indices == 0)
				return;

			// Flatten out triangles so we have sharp edges
			unsigned int ntris = (pmesh->num_indices - (pmesh->num_indices % 3)) / 3;
			CDynamicMeshHelper::Params params;
			params.topology = PRIMITIVE_TYPE_TRIANGLELIST;
			params.pVertices = new SVertex[params.numVertices = ntris * 3];
			params.pIndices = new SIndex[params.numIndices = ntris * 3];

			Vec3f n;
			for (unsigned int itri = 0; itri < ntris * 3; itri += 3)
			{
				const Vec3f &p1 = pmesh->points[pmesh->indices[itri]];
				const Vec3f &p2 = pmesh->points[pmesh->indices[itri + 1]];
				const Vec3f &p3 = pmesh->points[pmesh->indices[itri + 2]];
				n = ((p2 - p1) ^ (p3 - p1)).Normalized();

				for (int i = 0; i < 3; ++i)
				{
					const Vec3f& p = pmesh->points[pmesh->indices[itri + i]];
					params.pVertices[itri + i] = SVertex(p.x, p.y, p.z, n.x, n.y, n.z, 0, 0, 0);
					params.pIndices[itri + i] = itri + i;
				}
			}

			m_pHelper = C3DEngine::Get()->AddHelper<CDynamicMeshHelper>(params);
			m_pHelper->SetColor(color);

			delete[] params.pVertices;
			delete[] params.pIndices;
			break;
		}
	case eSHAPE_BOX:
		{
			const box* pbox = (const box*)pshape;
			CBoxHelper::Params params;
			params.center = pbox->c;
			for (int i = 0; i < 3; ++i)
				params.dimensions[i] = pbox->axis[i] * pbox->dim[i];
			m_pHelper = p3DEngine->AddHelper<CBoxHelper>(params, SHelperRenderParams(color, /*outline*/true, true), releaseAfterRender);
			break;
		}
	}
}

S_API void CPhysDebugHelper::UpdateFromShape(const shape* pshape)
{
	if (!m_pHelper || !pshape)
		return;

	if (pshape->GetType() != m_ShapeType)
		return;

	switch (pshape->GetType())
	{
	case eSHAPE_RAY:
		{
			const ray* pray = (const ray*)pshape;
			CVectorHelper* pHelper = (CVectorHelper*)m_pHelper;
			CVectorHelper::Params params = pHelper->GetParams();
			params.p = pray->p;
			params.v = pray->v;
			pHelper->SetParams(params);
			break;
		}
	case eSHAPE_PLANE:
		{
			const plane* pplane = (const plane*)pshape;
			CPlaneHelper* pHelper = (CPlaneHelper*)m_pHelper;
			CPlaneHelper::Params params = pHelper->GetParams();
			params.n = pplane->n;
			params.d = pplane->d;
			pHelper->SetParams(params);
			break;
		}
	case eSHAPE_SPHERE:
		{
			const sphere* psphere = (const sphere*)pshape;
			CSphereHelper* pHelper = (CSphereHelper*)m_pHelper;
			CSphereHelper::Params params = pHelper->GetParams();
			params.center = psphere->c;
			params.radius = psphere->r;
			pHelper->SetParams(params);
			break;
		}
	case eSHAPE_CYLINDER:
		{
			const cylinder* pcyl = (const cylinder*)pshape;
			CCylinderHelper* pHelper = (CCylinderHelper*)m_pHelper;
			CCylinderHelper::Params params = pHelper->GetParams();
			params.p[0] = pcyl->p[0];
			params.p[1] = pcyl->p[1];
			params.r = pcyl->r;
			pHelper->SetParams(params);
			break;
		}
	case eSHAPE_CAPSULE:
		{
			const capsule* pcapsule = (const capsule*)pshape;
			CDynamicMeshHelper* pHelper = (CDynamicMeshHelper*)m_pHelper;

			SInitialGeometryDesc geomDesc;
			MakeCapsule(geomDesc, pcapsule->c - pcapsule->hh * pcapsule->axis, pcapsule->c + pcapsule->hh * pcapsule->axis, pcapsule->r, 24, 10);

			// Vertex count does not change, we can simply update vertex positions
			IVertexBuffer* pVB = pHelper->GetVertexBuffer();
			SVertex* pVerts = pVB->GetShadowBuffer();
			unsigned int nverts = pVB->GetVertexCount();
			for (unsigned int ivtx = 0; ivtx < nverts; ++ivtx)
				pVerts[ivtx] = geomDesc.pVertices[ivtx];

			pVB->UploadVertexData();

			delete[] geomDesc.pVertices;
			delete[] geomDesc.GetZeroSubset()->pIndices;
			break;
		}
	case eSHAPE_TRIANGLE:
		{
			const triangle* ptri = (const triangle*)pshape;
			CDynamicMeshHelper* pHelper = (CDynamicMeshHelper*)m_pHelper;

			// Vertex count does not change, we can simply update vertex positions
			IVertexBuffer* pVB = pHelper->GetVertexBuffer();
			SVertex* pVerts = pVB->GetShadowBuffer();
			for (int i = 0; i < 3; ++i)
			{
				pVerts[i] = SVertex(ptri->p[i].x, ptri->p[i].y, ptri->p[i].z);
			}

			pVB->UploadVertexData();
			break;
		}
	case eSHAPE_MESH:
		{
			const mesh* pmesh = dynamic_cast<const mesh*>(pshape);
			if (!pmesh)
				return;

			CDynamicMeshHelper* pMeshHelper = dynamic_cast<CDynamicMeshHelper*>(m_pHelper);
			if (!pMeshHelper)
				return;

			// Helper must be recreated if vertex/index count changes.
			// We just assume it's the same here.
			IVertexBuffer* pVB = pMeshHelper->GetVertexBuffer();
			SVertex* pVerts = pVB->GetShadowBuffer();
			if (!pVerts)
				return;

			unsigned int ntris = (pmesh->num_indices - (pmesh->num_indices % 3)) / 3;
			Vec3f n;
			for (unsigned int itri = 0; itri < ntris * 3; itri += 3)
			{
				const Vec3f &p1 = pmesh->points[pmesh->indices[itri]];
				const Vec3f &p2 = pmesh->points[pmesh->indices[itri + 1]];
				const Vec3f &p3 = pmesh->points[pmesh->indices[itri + 2]];
				n = ((p2 - p1) ^ (p3 - p1)).Normalized();

				for (int i = 0; i < 3; ++i)
				{
					const Vec3f& p = pmesh->points[pmesh->indices[itri + i]];
					pVerts[itri + i] = SVertex(p.x, p.y, p.z, n.x, n.y, n.z, 0, 0, 0);
				}
			}

			pVB->UploadVertexData();
			break;
		}
	case eSHAPE_BOX:
		{
			const box* pbox = (const box*)pshape;
			CBoxHelper* pHelper = (CBoxHelper*)m_pHelper;
			CBoxHelper::Params params = pHelper->GetParams();
			params.center = pbox->c;
			for (int i = 0; i < 3; ++i)
				params.dimensions[i] = pbox->axis[i] * pbox->dim[i];
			pHelper->SetParams(params);
			break;
		}
	}
}

S_API void CPhysDebugHelper::Show(bool show)
{
	if (!m_pHelper)
		return;

	m_pHelper->GetRenderParams()->visible = show;
}

S_API bool CPhysDebugHelper::IsShown() const
{
	if (!m_pHelper)
		return false;

	return m_pHelper->GetRenderParams()->visible;
}

S_API void CPhysDebugHelper::Clear()
{
	if (m_pHelper)
		m_pHelper->Release();
	m_pHelper = 0;
}

///////////////////////////////////////////////////////////////////////////////////////////////////////////////////////

S_API void PhysicsDebugRenderer::VisualizePoint(const Vec3f& p, const SColor& color, bool releaseAfterRender)
{
	CHelper* helper = C3DEngine::Get()->AddHelper<CPointHelper>(CPointHelper::Params(p), releaseAfterRender);
	helper->SetColor(color);
}

S_API void PhysicsDebugRenderer::VisualizeVector(const Vec3f& p, const Vec3f& v, const SColor& color, bool releaseAfterRender)
{
	CHelper* helper = C3DEngine::Get()->AddHelper<CVectorHelper>(CVectorHelper::Params(p, v.Normalized(), v.Length()), releaseAfterRender);
	helper->SetColor(color);
}

S_API void PhysicsDebugRenderer::VisualizePlane(const Vec3f& p, const Vec3f& n, const SColor& color, bool releaseAfterRender)
{
	CPlaneHelper::Params params;
	params.n = n;
	params.d = Vec3Dot(p, n);
	params.hsize = 10.0f;
	CHelper* helper = C3DEngine::Get()->AddHelper<CPlaneHelper>(params, releaseAfterRender);
	helper->SetColor(color);
}

S_API void PhysicsDebugRenderer::VisualizeAABB(const AABB& aabb, const SColor& color, bool releaseAfterRender)
{
	CBoxHelper::Params params;
	params.center = (aabb.vMin + aabb.vMax) * 0.5f;
	params.dimensions[0] = Vec3f((aabb.vMax.x - aabb.vMin.x) * 0.5f, 0, 0);
	params.dimensions[1] = Vec3f(0, (aabb.vMax.y - aabb.vMin.y) * 0.5f, 0);
	params.dimensions[2] = Vec3f(0, 0, (aabb.vMax.z - aabb.vMin.z) * 0.5f);
	C3DEngine::Get()->AddHelper<CBoxHelper>(params, SHelperRenderParams(color, true, true), releaseAfterRender);
}

S_API void PhysicsDebugRenderer::VisualizeBox(const OBB& obb, const SColor& color, bool releaseAfterRender)
{
	CBoxHelper::Params params;
	params.center = obb.center;
	for (int i = 0; i < 3; ++i)
		params.dimensions[i] = obb.directions[i] * obb.dimensions[i];

	C3DEngine::Get()->AddHelper<CBoxHelper>(params, SHelperRenderParams(color, true, true), releaseAfterRender);
}

SP_NMSPACE_END
