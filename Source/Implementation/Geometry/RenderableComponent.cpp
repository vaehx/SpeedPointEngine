#include <Implementation\Geometry\RenderableComponent.h>
#include <Abstract\I3DEngine.h>
#include <Abstract\IRenderer.h>
#include <Abstract\IMaterial.h>
#include <Abstract\IVertexBuffer.h>
#include <Abstract\IIndexBuffer.h>

SP_NMSPACE_BEG

S_API CRenderableComponent::CRenderableComponent(I3DEngine* pRenderer, IMaterialManager* pMatMgr)
	: m_pRenderer(pRenderer),
	m_pRO(0),
	m_pEntity(0),
	m_bVisible(true)
{
}

S_API CRenderableComponent::~CRenderableComponent()
{
	Clear();
}

S_API void CRenderableComponent::Init(IEntity* pEntity)
{
	assert(IS_VALID_PTR(m_pRenderer));
	assert(IS_VALID_PTR(pEntity));

	I3DEngine* pRenderer = m_pRenderer;
	Clear();
	m_pRenderer = pRenderer;
	m_pEntity = pEntity;

	m_pRO = m_pRenderer->GetRenderObject();
	assert(IS_VALID_PTR(m_pRO));

	SRenderDesc& rd = m_pRO->renderDesc;
	rd.renderPipeline = eRENDER_FORWARD;
	rd.bCustomViewProjMtx = false;

	// copy over subsets
	rd.nSubsets = GetSubsetCount();
	rd.pSubsets = new SRenderSubset[rd.nSubsets];
	for (unsigned int i = 0; i < rd.nSubsets; ++i)
	{
		SRenderSubset& renderSubset = rd.pSubsets[i];
		SGeomSubset* subset = GetSubset(i);
		if (!IS_VALID_PTR(subset))
		{
			renderSubset.render = false;
			continue;
		}

		renderSubset.drawCallDesc.primitiveType = m_Geometry.GetPrimitiveType();

		renderSubset.drawCallDesc.pVertexBuffer = GetVertexBuffer();
		renderSubset.drawCallDesc.pIndexBuffer = subset->pIndexBuffer;

		renderSubset.render = false;
		if (renderSubset.drawCallDesc.primitiveType == PRIMITIVE_TYPE_LINES)
		{
			renderSubset.render = m_bVisible;
		}
		else if (IS_VALID_PTR(renderSubset.drawCallDesc.pIndexBuffer))
		{
			renderSubset.render = m_bVisible;
			renderSubset.drawCallDesc.iStartIBIndex = 0;
			renderSubset.drawCallDesc.iEndIBIndex = renderSubset.drawCallDesc.pIndexBuffer->GetIndexCount() - 1;
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
		// TODO: To update a material, implement a method to flag a subset material to be updated,
		// 	 then do it below when updating the transform.
		if (subset->pMaterial)
		{
			renderSubset.shaderResources = subset->pMaterial->GetLayer(0)->resources;
		}
		else
		{
			if (IS_VALID_PTR(pMatMgr))
			{
				IMaterial* pDefMat = pMatMgr->GetDefaultMaterial();
				renderSubset.shaderResources = pDefMat->GetLayer(0)->resources;
			}
		}
	}
}

S_API void CRenderableComponent::Clear()
{
	if (IS_VALID_PTR(m_pRO) && IS_VALID_PTR(m_pRenderer))
	{
		m_pRenderer->ReleaseRenderObject(&m_pRO);
	}

	m_pRenderer = 0;
	m_pEntity = 0;
}

S_API void CRenderableComponent::SetVisible(bool visible)
{
	m_bVisible = visible;

	assert(m_pRO);

	// Update subsets
	SRenderDesc* rd = &m_pRO->renderDesc;
	for (unsigned int iSubset = 0; iSubset < rd->nSubsets; ++iSubset)
	{
		SRenderSubset* pSubset = &rd->pSubsets[iSubset];

		// do not show invalid subsets
		if (IS_VALID_PTR(pSubset->drawCallDesc.pVertexBuffer) && IS_VALID_PTR(pSubset->drawCallDesc.pIndexBuffer))
		{
			pSubset->render = m_bVisible;
		}
		else
		{
			pSubset->render = false;
		}
	}
}

S_API IGeometry* CRenderableComponent::GetGeometry()
{
	return &m_Geometry;
}

S_API IVertexBuffer* CRenderableComponent::GetVertexBuffer()
{
	return m_Geometry.GetVertexBuffer();
}

S_API SGeomSubset* CRenderableComponent::GetSubset(unsigned int i)
{
	return m_Geometry.GetSubset(i);
}

S_API unsigned int CRenderableComponent::GetSubsetCount() const
{
	return m_Geometry.GetSubsetCount();
}

S_API IMaterial* CRenderableComponent::GetSubsetMaterial(unsigned int subset = 0)
{
	SGeomSubset* pSubset = m_Geometry.GetSubset(subset);
	if (IS_VALID_PTR(pSubset))
		return pSubset->pMaterial;
	else
		return 0;
}

S_API void CRenderableComponent::SetViewProjMatrix(const SMatrix& mtx)
{
	if (IS_VALID_PTR(m_pRO))
	{
		SRenderDesc* rd = &m_pRO->renderDesc;
		rd->bCustomViewProjMtx = true;
		rd->viewProjMtx = mtx;
	}
}

S_API void CRenderableComponent::UnsetViewProjMatrix()
{
	if (IS_VALID_PTR(m_pRO))
	{
		SRenderDesc* rd = &m_pRO->renderDesc;
		rd->bCustomViewProjMtx = false;
	}
}


SP_NMSPACE_END
