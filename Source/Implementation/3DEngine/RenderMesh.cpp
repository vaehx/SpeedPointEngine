#include <Abstract\RenderMesh.h>
#include <Implementation\3DEngine\Geometry.h>
#include <Abstract\IVertexBuffer.h>
#include <Abstract\IMaterial.h>

SP_NMSPACE_BEG

S_API CRenderMesh::CRenderMesh()
	: m_pGeometry(0),
	m_bBoundBoxInvalid(true)
{
}

S_API void CRenderMesh::Clear()
{
	if (IS_VALID_PTR(m_pGeometry))
	{
		m_pGeometry->Clear();
		delete m_pGeometry;
	}

	m_pGeometry = 0;

	m_RenderDesc.Clear();
}

// --------------------------------------------------------------------------------------------------------------

S_API SResult CRenderMesh::Init(const SRenderMeshParams& params)
{
	IRenderer* pRenderAPI = SpeedPointEnv::GetEngine()->GetRenderer();
	SP_ASSERTR(IS_VALID_PTR(pRenderAPI), S_ERROR, "RenderAPI not initialized");

	Clear();

#ifdef _DEBUG
	_name = params._name;
#endif

	m_pGeometry = new CGeometry();
	if (Failure(m_pGeometry->Init(pRenderAPI, params.pGeomDesc)))
	{
		return CLog::Log(S_ERROR, "Failed init RenderMesh geometry");
	}

	m_bBoundBoxInvalid = true;

	SRenderDesc* rd = GetRenderDesc();
	rd->renderPipeline = eRENDER_FORWARD;
	rd->bCustomViewProjMtx = false;

	// copy over subsets
	rd->nSubsets = m_pGeometry->GetSubsetCount();
	rd->pSubsets = new SRenderSubset[rd->nSubsets];
	for (unsigned int i = 0; i < rd->nSubsets; ++i)
	{
		SRenderSubset& renderSubset = rd->pSubsets[i];
		SGeomSubset* subset = m_pGeometry->GetSubset(i);
		if (!IS_VALID_PTR(subset))
		{
			renderSubset.render = false;
			continue;
		}

		renderSubset.drawCallDesc.primitiveType = m_pGeometry->GetPrimitiveType();

		renderSubset.drawCallDesc.pVertexBuffer = GetVertexBuffer();
		renderSubset.drawCallDesc.pIndexBuffer = subset->pIndexBuffer;

		if (renderSubset.drawCallDesc.primitiveType == PRIMITIVE_TYPE_LINES)
		{
			renderSubset.render = true;
		}
		else if (IS_VALID_PTR(renderSubset.drawCallDesc.pIndexBuffer))
		{
			renderSubset.render = true;
			renderSubset.drawCallDesc.iStartIBIndex = 0;
			renderSubset.drawCallDesc.iEndIBIndex = renderSubset.drawCallDesc.pIndexBuffer->GetIndexCount() - 1;
		}
		else
		{
			renderSubset.render = false;
		}

		if (!IS_VALID_PTR(renderSubset.drawCallDesc.pVertexBuffer))
		{
			renderSubset.render = false;
		}
		else
		{
			renderSubset.drawCallDesc.iStartVBIndex = 0;
			renderSubset.drawCallDesc.iEndVBIndex = renderSubset.drawCallDesc.pVertexBuffer->GetVertexCount() - 1;
		}

		// Material is copied over. Warning: Avoid modifying the material during rendering.
		//TODO: To update a material, implement a method to flag a subset material to be updated,
		//TODO:		then do it below when updating the transform.
		if (subset->pMaterial)
		{
			renderSubset.shaderResources = subset->pMaterial->GetLayer(0)->resources;
		}
		else
		{
			IMaterialManager* pMatMgr = SpeedPointEnv::GetEngine()->GetMaterialManager();
			if (IS_VALID_PTR(pMatMgr))
			{
				IMaterial* pDefMat = pMatMgr->GetDefaultMaterial();
				renderSubset.shaderResources = pDefMat->GetLayer(0)->resources;
			}
		}
	}

	return S_SUCCESS;
}

// --------------------------------------------------------------------------------------------------------------

S_API SRenderDesc* CRenderMesh::GetRenderDesc()
{
	return &m_RenderDesc;
}

S_API IGeometry* CRenderMesh::GetGeometry()
{
	return m_pGeometry;
}

S_API AABB CRenderMesh::GetAABB()
{
	if (m_bBoundBoxInvalid)
	{
		if (IS_VALID_PTR(m_pGeometry))
		{
			m_pGeometry->CalculateBoundBox(m_AABB, SMatrix());
			m_bBoundBoxInvalid = false;
		}
		else
		{
			m_AABB = AABB();
		}
	}

	return m_AABB;
}

S_API void CRenderMesh::SetCustomViewProjMatrix(const SMatrix* viewProj)
{
	if (viewProj)
	{
		m_RenderDesc.bCustomViewProjMtx = true;
		m_RenderDesc.viewProjMtx = *viewProj;
	}
	else
	{
		m_RenderDesc.bCustomViewProjMtx = false;
	}
}

S_API IVertexBuffer* CRenderMesh::GetVertexBuffer()
{
	if (m_pGeometry)
		return m_pGeometry->GetVertexBuffer();
	else
		return 0;
}

S_API IIndexBuffer* CRenderMesh::GetIndexBuffer(unsigned int subset)
{
	if (m_pGeometry)
	{
		SGeomSubset* pSubset = m_pGeometry->GetSubset(subset);
		if (pSubset)
			return pSubset->pIndexBuffer;
	}

	return 0;
}

SP_NMSPACE_END
