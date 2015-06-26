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

#include <Implementation\Geometry\Entities.h>
#include <Abstract\IVertexBuffer.h>

SP_NMSPACE_BEG


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
	
	if (IS_VALID_PTR(m_RenderDesc.pSubsets) && m_RenderDesc.nSubsets > 0)
	{
		delete[] m_RenderDesc.pSubsets;
	}
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
S_API void CRigidBodyRenderable::GetUpdatedRenderDesc(SRenderDesc* pDest)
{
	if (IS_VALID_PTR(m_pTransformable))
	{		
		m_RenderDesc.transform.translation = SMatrix::MakeTranslationMatrix(m_pTransformable->GetPosition());
		m_RenderDesc.transform.rotation = SMatrix::MakeRotationMatrix(m_pTransformable->GetRotation());
		m_RenderDesc.transform.scale = SMatrix::MakeScaleMatrix(m_pTransformable->GetSize());
	}

	memcpy(pDest, &m_RenderDesc, sizeof(SRenderDesc));
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
S_API void CRigidBody::RecalcBoundBox()
{
	IGeometry* pGeom = m_Renderable.GetGeometry();

	if (!IS_VALID_PTR(pGeom))
		return;

	pGeom->CalculateBoundBox(m_AABB);
}

S_API SResult CRigidBody::Init(IGameEngine* pEngine, SInitialGeometryDesc* pInitialGeom = nullptr)
{
	SResult res;

	m_Renderable.Clear();

	m_pEngine = pEngine;
	if (!IS_VALID_PTR(m_pEngine))
	{
		return CLog::Log(S_ERROR, "Tried init Static Object, but engine ptr is invalid!");
	}


	m_pEngine->GetMaterialManager()->CollectInitGeomMaterials(pInitialGeom);
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

S_API IMaterial* CRigidBody::GetSubsetMaterial(unsigned int subset = 0)
{
	if (subset >= m_Renderable.GetSubsetCount())
		return 0;

	SGeomSubset* pSubset = m_Renderable.GetSubset(subset);
	return (IS_VALID_PTR(pSubset)) ? pSubset->pMaterial : 0;
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