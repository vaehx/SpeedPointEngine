//////////////////////////////////////////////////////////////////////////////////
//
// This file is part of the SpeedPointEngine
// Copyright (c) 2011-2015, iSmokiieZz
// ------------------------------------------------------------------------------
// Filename:	StaticObject.cpp
// Created:	8/19/2014 by iSmokiieZz
// Description:
// -------------------------------------------------------------------------------
// History:
//
//////////////////////////////////////////////////////////////////////////////////

#include <Implementation\Geometry\StaticObject.h>
#include <Implementation\Geometry\Material.h>
#include <Abstract\IResourcePool.h>
#include <Abstract\IRenderer.h>
#include <Abstract\IVertexBuffer.h>
#include <Abstract\IIndexBuffer.h>

#include <Implementation\Geometry\RenderableObjects.h>

SP_NMSPACE_BEG


////////////////////////////////////////////////////////////////////////////////////////////////////////
//
// StaticObject Renderable Component
//
////////////////////////////////////////////////////////////////////////////////////////////////////////

S_API CStaticObjectRenderable::CStaticObjectRenderable()
	: m_bRenderDescFilled(false),
	m_bVisible(true)
{
}

// ----------------------------------------------------------------------------------------
S_API IVertexBuffer* CStaticObjectRenderable::GetVertexBuffer()
{
	return m_Geometry.GetVertexBuffer();
}

// ----------------------------------------------------------------------------------------
S_API SGeomSubset* CStaticObjectRenderable::GetSubset(unsigned int i)
{
	return m_Geometry.GetSubset(i);
}

// ----------------------------------------------------------------------------------------
S_API unsigned int CStaticObjectRenderable::GetSubsetCount() const
{
	return m_Geometry.GetSubsetCount();
}

// ----------------------------------------------------------------------------------------
S_API EPrimitiveType CStaticObjectRenderable::GetGeometryPrimitiveType() const
{
	return m_Geometry.GetPrimitiveType();
}

// ----------------------------------------------------------------------------------------
S_API SRenderDesc* CStaticObjectRenderable::GetRenderDesc()
{
	return &m_RenderDesc;
}

// ----------------------------------------------------------------------------------------
S_API void CStaticObjectRenderable::SetVisible(bool visible)
{
	m_bVisible = visible;
}

// ----------------------------------------------------------------------------------------
S_API SRenderDesc* CStaticObjectRenderable::FillRenderDesc(IGameEngine* pEngine)
{
	m_RenderDesc.renderPipeline = eRENDER_FORWARD;

	m_RenderDesc.bCustomViewProjMtx = m_bUseCustomViewProjMtx;
	if (m_bUseCustomViewProjMtx)
	{
		m_RenderDesc.viewProjMtx = m_ViewProjMtx;
	}

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

		renderSubset.drawCallDesc.primitiveType = GetGeometryPrimitiveType();		

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
			IMaterialManager* pMatMgr;
			if (IS_VALID_PTR(pEngine) && IS_VALID_PTR((pMatMgr = pEngine->GetMaterialManager())))
			{
				IMaterial* pDefMat = pMatMgr->GetDefaultMaterial();
				renderSubset.shaderResources = pDefMat->GetLayer(0)->resources;
			}
		}
	}


	m_bRenderDescFilled = true;
	return &m_RenderDesc;
}



S_API SRenderDesc* CStaticObjectRenderable::GetUpdatedRenderDesc()
{
	for (unsigned int i = 0; i < m_RenderDesc.nSubsets; ++i)
	{
		SRenderSubset* pSubset = &m_RenderDesc.pSubsets[i];

		// do not render invalid subsets
		if (IS_VALID_PTR(pSubset->drawCallDesc.pVertexBuffer) && IS_VALID_PTR(pSubset->drawCallDesc.pIndexBuffer))
		{
			pSubset->render = m_bVisible;
		}
		else
		{
			pSubset->render = false;
		}
	}

	return &m_RenderDesc;
}












////////////////////////////////////////////////////////////////////////////////////////////////////////
//
//	Static Object
//
////////////////////////////////////////////////////////////////////////////////////////////////////////


// ----------------------------------------------------------------------------------------
S_API StaticObject::StaticObject()
{
}

// ----------------------------------------------------------------------------------------
S_API StaticObject::~StaticObject()
{
	Clear();
}

// ----------------------------------------------------------------------------------------
S_API void StaticObject::Clear()
{
	for (auto itRefObj = m_RefObjects.begin(); itRefObj != m_RefObjects.end(); itRefObj++)
	{
		if (IS_VALID_PTR((*itRefObj)))	
			(*itRefObj)->SetBase(0);	
	}

	m_RefObjects.clear();

	m_Renderable.Clear();
	m_AABB.Reset();

	/*m_Geometry.Clear();
	if (IS_VALID_PTR(m_pMaterials))
	{
		delete[] m_pMaterials;	
	}
	*/

	/*m_pMaterials = 0;
	m_nMaterials = 0;
	m_pEngine = 0;*/	
}

// ----------------------------------------------------------------------------------------
S_API SResult StaticObject::Init(IGameEngine* pEngine, SInitialGeometryDesc* pInitialGeom /*= nullptr*/)
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

	m_Renderable.FillRenderDesc(pEngine);
	return S_SUCCESS;
}

// ----------------------------------------------------------------------------------------
S_API IMaterial* StaticObject::GetSubsetMaterial(unsigned int subset /*= 0*/)
{
	if (subset >= m_Renderable.GetSubsetCount())
		return 0;

	SGeomSubset* pSubset = m_Renderable.GetSubset(subset);
	return (IS_VALID_PTR(pSubset)) ? pSubset->pMaterial : 0;
}

// ----------------------------------------------------------------------------------------
S_API unsigned int StaticObject::GetSubsetCount() const
{
	return m_Renderable.GetSubsetCount();
}

// ----------------------------------------------------------------------------------------
/*
S_API SResult StaticObject::CreateNormalsGeometry(IRenderableObject** pNormalGeometryObject) const
{
	if (!IS_VALID_PTR(m_pEngine))
		return S_NOTINIT;

	if (!IS_VALID_PTR(pNormalGeometryObject))
		return S_INVALIDPARAM;


	SInitialGeometryDesc normalGeom;
	if (Failure(m_Geometry.CalculateNormalsGeometry(normalGeom)))
		return EngLog(S_ERROR, m_pEngine, "Failed Calculcate Normals Geometry!");


	IStaticObject* pStaticObject = new StaticObject();
	RETURN_ON_ERR(pStaticObject->Init(m_pEngine, m_pEngine->GetRenderer(), nullptr, &normalGeom));

*pNormalGeometryObject = pStaticObject;

	return S_ERROR;
}
*/

// ----------------------------------------------------------------------------------------
S_API void StaticObject::RecalcBoundBox()
{
	IGeometry* pGeom = m_Renderable.GetGeometry();
	
	if (!IS_VALID_PTR(pGeom))
		return;

	pGeom->CalculateBoundBox(m_AABB);	
}



// ----------------------------------------------------------------------------------------
S_API IReferenceObject* StaticObject::CreateReferenceObject()
{
	IReferenceObject* pRefObject = new CReferenceObject(m_pEngine);
	pRefObject->SetBase((IRenderableObject*)this);
	pRefObject->vPosition = vPosition;
	pRefObject->vRotation = vRotation;
	pRefObject->vSize = vSize;

	m_RefObjects.push_back(pRefObject);

	return pRefObject;
}



// ----------------------------------------------------------------------------------------

S_API SRenderDesc* StaticObject::GetUpdatedRenderDesc()
{
	if (!m_Renderable.RenderDescFilled())
	{
		m_Renderable.FillRenderDesc(m_pEngine);
	}

	SRenderDesc* pRenderDesc = m_Renderable.GetUpdatedRenderDesc();
	
	STransformationDesc& transformDesc = pRenderDesc->transform;
	transformDesc.translation = SMatrix::MakeTranslationMatrix(vPosition);
	transformDesc.rotation = SMatrix::MakeRotationMatrix(vRotation);
	transformDesc.scale = SMatrix::MakeScaleMatrix(vSize);

	return pRenderDesc;
}






SP_NMSPACE_END