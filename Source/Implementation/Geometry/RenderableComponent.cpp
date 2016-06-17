#include <Implementation\Geometry\RenderableComponent.h>
#include <Abstract\I3DEngine.h>
#include <Abstract\IRenderer.h>
#include <Abstract\IMaterial.h>
#include <Abstract\IVertexBuffer.h>
#include <Abstract\IIndexBuffer.h>

SP_NMSPACE_BEG

S_API CRenderableComponent::CRenderableComponent(IEntity* pEntity, I3DEngine* pRenderer, IMaterialManager* pMatMgr)
	: m_pRenderer(pRenderer),
	m_pMatMgr(pMatMgr),
	m_pEntity(pEntity),
	m_bVisible(true)
{
}

S_API CRenderableComponent::~CRenderableComponent()
{
	Clear();
}

S_API IEntity* CRenderableComponent::GetEntity() const
{
	return m_pEntity;
}

S_API void CRenderableComponent::SetEntity(IEntity* entity)
{
	m_pEntity = entity;
}

S_API void CRenderableComponent::Init()
{
	assert(IS_VALID_PTR(m_pRenderer));
	assert(IS_VALID_PTR(m_pEntity));

	I3DEngine* pRenderer = m_pRenderer;
	Clear();
	m_pRenderer = pRenderer;

#ifdef _DEBUG
	_name = m_pEntity->GetName();
#endif

	SRenderDesc* rd = GetRenderDesc();
	rd->renderPipeline = eRENDER_FORWARD;
	rd->bCustomViewProjMtx = false;

	// copy over subsets
	rd->nSubsets = GetSubsetCount();
	rd->pSubsets = new SRenderSubset[rd->nSubsets];
	for (unsigned int i = 0; i < rd->nSubsets; ++i)
	{
		SRenderSubset& renderSubset = rd->pSubsets[i];
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
			if (IS_VALID_PTR(m_pMatMgr))
			{
				IMaterial* pDefMat = m_pMatMgr->GetDefaultMaterial();
				renderSubset.shaderResources = pDefMat->GetLayer(0)->resources;
			}
		}
	}
}

S_API void CRenderableComponent::Clear()
{
	if (m_pEntity)
		m_pEntity->ReleaseComponent(this);

	m_pRenderer = 0;
	m_pMatMgr = 0;
	m_pEntity = 0;
}

S_API void CRenderableComponent::SetVisible(bool visible)
{
	m_bVisible = visible;

	// Update subsets
	SRenderDesc* rd = GetRenderDesc();
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

S_API IMaterial* CRenderableComponent::GetSubsetMaterial(unsigned int subset /*= 0*/)
{
	SGeomSubset* pSubset = m_Geometry.GetSubset(subset);
	if (IS_VALID_PTR(pSubset))
		return pSubset->pMaterial;
	else
		return 0;
}

S_API void CRenderableComponent::SetViewProjMatrix(const SMatrix& mtx)
{
	SRenderDesc* rd = GetRenderDesc();
	rd->bCustomViewProjMtx = true;
	rd->viewProjMtx = mtx;
}

S_API void CRenderableComponent::UnsetViewProjMatrix()
{
	SRenderDesc* rd = GetRenderDesc();
	rd->bCustomViewProjMtx = false;
}




S_API const AABB& CRenderableComponent::GetAABB() const
{
	if (IS_VALID_PTR(m_pEntity))
		m_pEntity->GetBoundBox();
	else
		return AABB();
}

S_API SRenderDesc* CRenderableComponent::GetRenderDesc()
{
	return &m_RenderDesc;
}

S_API void CRenderableComponent::Update()
{
	if (IS_VALID_PTR(m_pEntity))
	{
		m_RenderDesc.transform.translation = SMatrix::MakeTranslationMatrix(m_pEntity->GetPosition());
		m_RenderDesc.transform.rotation = m_pEntity->GetRotation().ToRotationMatrix();
		m_RenderDesc.transform.scale = SMatrix::MakeScaleMatrix(m_pEntity->GetSize());
	}
}



SP_NMSPACE_END
