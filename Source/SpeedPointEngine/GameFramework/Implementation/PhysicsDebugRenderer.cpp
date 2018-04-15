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

S_API void CPhysDebugHelper::CreateFromShape(const shape* pshape, const SColor& color)
{
	if (!pshape)
		return;

	m_pHelper = CreateHelperForShape(pshape, color);
	m_ShapeType = pshape->GetType();
}

S_API void CPhysDebugHelper::UpdateFromShape(const shape* pshape, const AABB& bounds)
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

			unsigned int minvtx = UINT_MAX, maxvtx = 0;
			unsigned int ntris = (pmesh->num_indices - (pmesh->num_indices % 3)) / 3;
			Vec3f n;
			bool insideBounds;
			for (unsigned int itri = 0; itri < ntris * 3; itri += 3)
			{
				insideBounds = false;
				for (int i = 0; i < 3; ++i)
					insideBounds |= bounds.ContainsPoint(Vec3f(pVerts[itri + i].x, pVerts[itri + i].y, pVerts[itri + i].z));

				if (!insideBounds) continue;

				const Vec3f &p1 = pmesh->points[pmesh->indices[itri]];
				const Vec3f &p2 = pmesh->points[pmesh->indices[itri + 1]];
				const Vec3f &p3 = pmesh->points[pmesh->indices[itri + 2]];
				n = ((p2 - p1) ^ (p3 - p1)).Normalized();

				for (unsigned int ivtx = itri; ivtx < itri + 3; ++ivtx)
				{
					const Vec3f& p = pmesh->points[pmesh->indices[ivtx]];
					pVerts[ivtx] = SVertex(p.x, p.y, p.z, n.x, n.y, n.z, 0, 0, 0);
				}

				minvtx = min(itri, minvtx);
				maxvtx = max(itri + 2, maxvtx);
			}

			pVB->UploadVertexData(minvtx, maxvtx - minvtx);
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
	case eSHAPE_TERRAIN_MESH:
		{
			const terrain_mesh* pmesh = dynamic_cast<const terrain_mesh*>(pshape);
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

			unsigned int minOffs[2], maxOffs[2];
			minOffs[0] = (unsigned int)floorf((bounds.vMin.x - pmesh->aabb.vMin.x) / pmesh->segmentSz);
			minOffs[1] = (unsigned int)floorf((bounds.vMin.z - pmesh->aabb.vMin.z) / pmesh->segmentSz);
			maxOffs[0] = (unsigned int)ceilf((bounds.vMax.x - pmesh->aabb.vMin.x) / pmesh->segmentSz);
			maxOffs[1] = (unsigned int)ceilf((bounds.vMax.z - pmesh->aabb.vMin.z) / pmesh->segmentSz);

			unsigned int minvtx = UINT_MAX, maxvtx = 0;
			for (unsigned int y = minOffs[1]; y < maxOffs[1]; ++y)
				for (unsigned int x = minOffs[0]; x < maxOffs[0]; ++x)
				{
					// tris: [ 0->1->2, 0->2->3 ]
					Vec3f p[] =
					{
						pmesh->points[y * (pmesh->segmentsPerSide + 1) + x],
						pmesh->points[(y + 1) * (pmesh->segmentsPerSide + 1) + x],
						pmesh->points[(y + 1) * (pmesh->segmentsPerSide + 1) + (x + 1)],
						pmesh->points[y * (pmesh->segmentsPerSide + 1) + (x + 1)]
					};

					Vec3f n = Vec3Normalize((p[1] - p[0]) ^ (p[3] - p[0]));
					unsigned int ivert = (y * pmesh->segmentsPerSide + x) * 4;
					minvtx = min(minvtx, ivert);
					maxvtx = max(maxvtx, ivert + 3);
					for (int i = 0; i < 4; ++i)
						pVerts[ivert + i] = SVertex(p[i].x, p[i].y, p[i].z, n.x, n.y, n.z, 0, 0, 0);
				}

			if (maxvtx >= minvtx)
				pVB->UploadVertexData(minvtx, max(1, maxvtx - minvtx));
			break;
		}
	}
}

S_API void CPhysDebugHelper::SetMeshTransform(const Mat44& transform)
{
	CDynamicMeshHelper* pMeshHelper = dynamic_cast<CDynamicMeshHelper*>(m_pHelper);
	if (pMeshHelper)
		pMeshHelper->SetTransform(transform);
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

S_API void PhysicsDebugRenderer::VisualizeLine(const Vec3f& p1, const Vec3f& p2, const SColor& color, bool releaseAfterRender)
{
	CHelper* helper = C3DEngine::Get()->AddHelper<CLineHelper>(CLineHelper::Params(p1, p2), releaseAfterRender);
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
