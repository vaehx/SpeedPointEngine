//////////////////////////////////////////////////////////////////////////////////
//
// This file is part of the SpeedPointEngine
// Copyright (c) 2011-2015, iSmokiieZz
// ------------------------------------------------------------------------------
// Filename:	Entities.cpp
// Created:	6/24/2014 by iSmokiieZz
// Description:
// -------------------------------------------------------------------------------
// History:
//
//////////////////////////////////////////////////////////////////////////////////

#include <Implementation\Geometry\Entity.h>
#include <Abstract\IVertexBuffer.h>

SP_NMSPACE_BEG

S_API CEntity::CEntity(IEntitySystem* pEntitySystem)
	: m_pEntitySystem(pEntitySystem)
{
}

S_API const char* CEntity::GetName() const
{
	return m_Name.c_str();
}

S_API void CEntity::SetName(const char* name)
{
	m_Name = name;
}

S_API IComponent* CEntity::CreateComponent(EComponentType component) const
{
	if (!IS_VALID_PTR(m_pComponents[component]))
	{
		switch (component)
		{
		case eCOMPONENT_RENDERABLE:
			m_pComponents[component] = new 
		}
	}

	return m_pComponents[component];
}

// Returns NULL if the component was not created
S_API IComponent* CEntity::GetComponent(EComponentType component) const
{

}

















//////////////////////////////////////////////////////////////////////////////////////////
//
//	Rigid Body
//
//////////////////////////////////////////////////////////////////////////////////////////
CRigidBodyRenderable::CRigidBodyRenderable()
	: m_bVisible(true),
	m_pTransformable(0)	
{
}

// ------------------------------------------------------------------------------
S_API void CRigidBodyRenderable::Clear()
{
	m_Geometry.Clear();
}

// ------------------------------------------------------------------------------
S_API void CRigidBodyRenderable::SetVisible(bool visible)
{
	m_bVisible = visible;

	if (!IS_VALID_PTR(m_RenderDesc.pSubsets) || m_RenderDesc.nSubsets == 0)
	{
		return;
	}

	// Update subsets
	for (unsigned int iSubset = 0; iSubset < m_RenderDesc.nSubsets; ++iSubset)
	{
		SRenderSubset* pSubset = &m_RenderDesc.pSubsets[iSubset];

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

// ------------------------------------------------------------------------------
S_API void CRigidBodyRenderable::InitRenderDesc(IMaterialManager* pMatMgr, STransformable* pTransformable)
{
	m_pTransformable = pTransformable;

	m_RenderDesc.renderPipeline = eRENDER_FORWARD;
	m_RenderDesc.bCustomViewProjMtx = false;

	// copy over subsets
	m_RenderDesc.nSubsets = GetSubsetCount();
	m_RenderDesc.pSubsets = new SRenderSubset[m_RenderDesc.nSubsets];
	for (unsigned int i = 0; i < m_RenderDesc.nSubsets; ++i)
	{
		SRenderSubset& renderSubset = m_RenderDesc.pSubsets[i];
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

// ------------------------------------------------------------------------------
S_API SRenderDesc* CRigidBodyRenderable::GetUpdatedRenderDesc()
{
	// Update transformation
	if (IS_VALID_PTR(m_pTransformable))
	{		
		m_RenderDesc.transform.translation = SMatrix::MakeTranslationMatrix(m_pTransformable->GetPosition());
		m_RenderDesc.transform.rotation = m_pTransformable->GetRotation().ToRotationMatrix();
		m_RenderDesc.transform.scale = SMatrix::MakeScaleMatrix(m_pTransformable->GetSize());
	}

	return &m_RenderDesc;
}

// ------------------------------------------------------------------------------
S_API IGeometry* CRigidBodyRenderable::GetGeometry()
{
	return &m_Geometry;
}

// ------------------------------------------------------------------------------
S_API IVertexBuffer* CRigidBodyRenderable::GetVertexBuffer()
{
	return m_Geometry.GetVertexBuffer();
}

// ------------------------------------------------------------------------------
S_API SGeomSubset* CRigidBodyRenderable::GetSubset(unsigned int i)
{
	return m_Geometry.GetSubset(i);
}

// ------------------------------------------------------------------------------
S_API unsigned int CRigidBodyRenderable::GetSubsetCount() const
{
	return m_Geometry.GetSubsetCount();
}

// ------------------------------------------------------------------------------
S_API void CRigidBodyRenderable::SetViewProjMatrix(const SMatrix& mtx)
{
	m_RenderDesc.viewProjMtx = mtx;
	m_RenderDesc.bCustomViewProjMtx = true;
}

// ------------------------------------------------------------------------------
S_API void CRigidBodyRenderable::UnsetViewProjMatrix()
{
	m_RenderDesc.bCustomViewProjMtx = false;
}

// ------------------------------------------------------------------------------
S_API IMaterial* CRigidBodyRenderable::GetSubsetMaterial(unsigned int subset /*= 0*/)
{
	if (subset > GetSubsetCount())
		return 0;

	SGeomSubset* pSubset = GetSubset(subset);
	if (!IS_VALID_PTR(pSubset))
		return 0;

	return pSubset->pMaterial;
}







// ------------------------------------------------------------------------------
CRigidBody::CRigidBody()	
{	
	m_Name = "RigidBody";
}

S_API void CRigidBody::RecalcBoundBox()
{
	IGeometry* pGeom = m_Renderable.GetGeometry();

	if (!IS_VALID_PTR(pGeom))
		return;

	pGeom->CalculateBoundBox(m_AABB, GetWorldMatrix());
}

S_API SResult CRigidBody::Init(IGameEngine* pEngine, const char* name /*="RigidBody"*/, SInitialGeometryDesc* pInitialGeom /*= nullptr*/)
{
	SResult res;

	m_Renderable.Clear();

	m_pEngine = pEngine;
	if (!IS_VALID_PTR(m_pEngine))
	{
		return CLog::Log(S_ERROR, "Tried init Static Object, but engine ptr is invalid!");
	}

	if (IS_VALID_PTR(name))
		m_Name = name;

	//m_pEngine->GetMaterialManager()->CollectInitGeomMaterials(pInitialGeom);

	res = m_Renderable.GetGeometry()->Init(m_pEngine, m_pEngine->GetRenderer(), pInitialGeom);
	if (Failure(res))
	{
		return CLog::Log(S_ERROR, "Failed init static object geometry!");
	}

	m_Renderable.InitRenderDesc(pEngine->GetMaterialManager(), (STransformable*)this);	

	return S_SUCCESS;
}

S_API void CRigidBody::SetVisible(bool visible)
{
	m_Renderable.SetVisible(visible);
}

S_API IGeometry* CRigidBody::GetGeometry()
{
	return m_Renderable.GetGeometry();
}

S_API IMaterial* CRigidBody::GetSubsetMaterial(unsigned int subset /*= 0*/)
{
	return m_Renderable.GetSubsetMaterial(subset);
}

S_API unsigned int CRigidBody::GetSubsetCount() const
{
	return m_Renderable.GetSubsetCount();
}

S_API void CRigidBody::Clear()
{
	for (auto itRefObj = m_RefObjects.begin(); itRefObj != m_RefObjects.end(); itRefObj++)
	{
		if (IS_VALID_PTR((*itRefObj)))			
			(*itRefObj)->SetBase(0);
	}

	m_RefObjects.clear();

	m_Renderable.Clear();
	m_AABB.Reset();
}

S_API IReferenceObject* CRigidBody::CreateReferenceObject()
{
	return 0;
}



SP_NMSPACE_END