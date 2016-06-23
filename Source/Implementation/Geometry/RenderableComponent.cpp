#include <Implementation\Geometry\RenderableComponent.h>
#include <Abstract\I3DEngine.h>
#include <Abstract\IRenderer.h>
#include <Abstract\IMaterial.h>
#include <Abstract\IVertexBuffer.h>
#include <Abstract\IIndexBuffer.h>

SP_NMSPACE_BEG

S_API CRenderableComponent::CRenderableComponent()
	: m_pRenderer(0),
	m_pEntity(0),
	m_bVisible(true),
	m_bBoundBoxInvalid(true)
{
}

S_API CRenderableComponent::~CRenderableComponent()
{
	OnRelease();
}

S_API IEntity* CRenderableComponent::GetEntity() const
{
	return m_pEntity;
}

S_API void CRenderableComponent::SetEntity(IEntity* entity)
{
	m_pEntity = entity;
}

S_API void CRenderableComponent::Init(const SInitialGeometryDesc* geomDesc /*= nullptr*/, IMaterialManager* pMatMgr /*= nullptr*/)
{
	assert(IS_VALID_PTR(m_pRenderer));
	assert(IS_VALID_PTR(m_pEntity));

	I3DEngine* pRenderer = m_pRenderer;
	ClearRenderableComponent();
	m_pRenderer = pRenderer;

#ifdef _DEBUG
	_name = m_pEntity->GetName();
#endif

	if (Failure(m_Geometry.Init(m_pRenderer->GetRenderer(), geomDesc)))
	{
		CLog::Log(S_ERROR, "Failed init renderable geometry of entity '%s'", m_pEntity->GetName());
		return;
	}

	m_bBoundBoxInvalid = true;

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
			if (IS_VALID_PTR(pMatMgr))
			{
				IMaterial* pDefMat = pMatMgr->GetDefaultMaterial();
				renderSubset.shaderResources = pDefMat->GetLayer(0)->resources;
			}
		}
	}
}

S_API void CRenderableComponent::ClearRenderableComponent()
{
	m_Geometry.Clear();
	m_RenderDesc.Clear();

	m_pRenderer = 0;
	m_pEntity = 0;

	m_bBoundBoxInvalid = true;
}

S_API void CRenderableComponent::OnRelease()
{
	if (IS_VALID_PTR(m_pEntity))
		m_pEntity->ReleaseComponent(this);

	// In case entity was null or the entity didn't call ClearComponent()
	ClearRenderableComponent();
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




S_API void CRenderableComponent::SetRenderer(I3DEngine* p3DEngine)
{
	m_pRenderer = p3DEngine;
}

S_API AABB CRenderableComponent::GetAABB()
{
	if (m_bBoundBoxInvalid)
		m_Geometry.CalculateBoundBox(m_AABB, SMatrix());

	return m_AABB;
}

S_API SRenderDesc* CRenderableComponent::GetRenderDesc()
{
	return &m_RenderDesc;
}

S_API void CRenderableComponent::Update()
{	
}

S_API void CRenderableComponent::OnEntityEvent(const SEntityEvent& e)
{
	if (!IS_VALID_PTR(m_pEntity))
		return;

	switch (e.type)
	{
	case eENTITY_EVENT_TRANSFORM:
		//TODO: This is not safe for multi-threading. When an entity transform event is triggered while it is rendered, this causes very bad things
		//TODO:   Solution:
		//TODO:			#1 Do not allow changing entity transform when rendering. (Is this even possible?)
		//TODO:			#2 Use a flag 'm_bTransformChanged'. Then copy the actual transformation in RenderableComponent::Update()
		m_RenderDesc.transform.translation = SMatrix::MakeTranslationMatrix(e.transform.pos);
		m_RenderDesc.transform.rotation = e.transform.rot.ToRotationMatrix();
		m_RenderDesc.transform.scale = SMatrix::MakeScaleMatrix(e.transform.scale);
		m_RenderDesc.transform.preRotation = SMatrix::MakeTranslationMatrix(-e.transform.pivot);
		break;
	}
}



SP_NMSPACE_END
