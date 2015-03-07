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
S_API void CStaticObjectRenderable::FillRenderSlot(IGameEngine* pEngine, SRenderSlot* pRenderSlot)
{
	SRenderDesc* pRenderDesc = &pRenderSlot->renderDesc;

	pRenderSlot->keep = true;
	//m_Renderable.SetRenderSlot(pRenderSlot);

	pRenderDesc->renderPipeline = eRENDER_FORWARD;

	// copy over subsets
	pRenderDesc->nSubsets = GetSubsetCount();
	pRenderDesc->pSubsets = new SRenderSubset[pRenderDesc->nSubsets];
	for (unsigned int i = 0; i < pRenderDesc->nSubsets; ++i)
	{
		SRenderSubset& renderSubset = pRenderDesc->pSubsets[i];
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
			renderSubset.render = true;
		}
		else if (IS_VALID_PTR(renderSubset.drawCallDesc.pIndexBuffer))
		{
			renderSubset.render = true;
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
	m_Renderable.Clear();	

	m_pEngine = pEngine;
	if (!IS_VALID_PTR(m_pEngine))
		return CLog::Log(S_ERROR, "Tried init Static Object, but engine ptr is invalid!");


	// Find matching material pointers based on given material names	
	if (pInitialGeom->nMatIndexAssigns > 0)
	{
		IMaterialManager* pMatMgr = m_pEngine->GetMaterialManager();
		if (!IS_VALID_PTR(pMatMgr))
			return S_ERROR;

		for (unsigned int iMatIndexAssign = 0; iMatIndexAssign < pInitialGeom->nMatIndexAssigns; ++iMatIndexAssign)
		{
			SMaterialIndices& matIndexAssign = pInitialGeom->pMatIndexAssigns[iMatIndexAssign];
			matIndexAssign.pMaterial = pMatMgr->FindMaterial(matIndexAssign.materialName);
		}
	}

	return m_Renderable.GetGeometry()->Init(m_pEngine, m_pEngine->GetRenderer(), pInitialGeom);
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

S_API SResult StaticObject::Render()
{
	SRenderSlot* pRenderSlot = m_Renderable.GetRenderSlot();
	SRenderDesc* pRenderDesc = 0;

	IRenderer* pRenderer = 0;
	if (!IS_VALID_PTR(m_pEngine) || !IS_VALID_PTR((pRenderer = m_pEngine->GetRenderer())))
		return EngLog(S_NOTINIT, m_pEngine, "Cannot Render Static Object: Engine or Renderer not set!");

	// Add RenderScheduleSlot if not there already
	if (m_Renderable.GetRenderSlot() == 0)
	{
		pRenderSlot = pRenderer->GetRenderSlot();
		if (!IS_VALID_PTR(pRenderSlot))
			return S_ERROR;

		pRenderDesc = &pRenderSlot->renderDesc;

		m_Renderable.FillRenderSlot(m_pEngine, pRenderSlot);
		m_Renderable.SetRenderSlot(pRenderSlot);		
	}
	else
	{
		pRenderDesc = &pRenderSlot->renderDesc;
	}

	// set / update transformation
	STransformationDesc& transformDesc = pRenderDesc->transform;
	transformDesc.translation = SMatrix::MakeTranslationMatrix(vPosition);
	transformDesc.rotation = SMatrix::MakeRotationMatrix(vRotation);
	transformDesc.scale = SMatrix::MakeScaleMatrix(vSize);	





	/*
	IRenderer* pRenderer = m_Geometry.GetRenderer();
	if (!IS_VALID_PTR(pRenderer))
		return S_NOTINIT;

	IVertexBuffer* pVB = m_Geometry.GetVertexBuffer();
	if (!IS_VALID_PTR(pVB))
		return S_ERROR;	

	if (!pVB->IsInited())
		return EngLog(S_ERROR, m_pEngine, "Cannot render Static Object: VB never initialized!");

	SRenderDesc dsc;
	dsc.drawCallDesc.pVertexBuffer = pVB;
	dsc.drawCallDesc.iStartVBIndex = 0;
	dsc.drawCallDesc.iEndVBIndex = pVB->GetVertexCount() - 1;
	
	dsc.pGeometry = &m_Geometry;
	dsc.technique = eRENDER_FORWARD;

	dsc.drawCallDesc.transform.translation = SMatrix::MakeTranslationMatrix(vPosition);
	dsc.drawCallDesc.transform.rotation = SMatrix::MakeRotationMatrix(vRotation);
	dsc.drawCallDesc.transform.scale = SMatrix::MakeScaleMatrix(vSize);

	dsc.drawCallDesc.primitiveType = m_Geometry.GetPrimitiveType();

	if (dsc.drawCallDesc.primitiveType == PRIMITIVE_TYPE_LINES)
	{
		RETURN_ON_ERR(pRenderer->RenderGeometry(dsc));
	}
	else
	{
		if (m_Geometry.GetIndexBufferCount() == 0)
			return EngLog(S_ERROR, m_pEngine, "Cannot render Static Object: There is no IB for a triangle-object!");	

		SGeometryIndexBuffer* pGeomIndexBuffers = m_Geometry.GetIndexBuffers();
		if (!IS_VALID_PTR(pGeomIndexBuffers))
			return S_ERROR;

		for (unsigned short iIndexBuffer = 0; iIndexBuffer < m_Geometry.GetIndexBufferCount(); ++iIndexBuffer)
		{
			dsc.drawCallDesc.pIndexBuffer = pGeomIndexBuffers[iIndexBuffer].pIndexBuffer;
			if (!dsc.drawCallDesc.pIndexBuffer->IsInited())
			{
				EngLog(S_ERROR, m_pEngine, "Could not render Static object: IB never initialized!");
				continue;
			}

			dsc.drawCallDesc.iStartIBIndex = 0;
			dsc.drawCallDesc.iEndIBIndex = dsc.drawCallDesc.pIndexBuffer->GetIndexCount() - 1;

			// here, index buffer index has to match with material index
			if (IS_VALID_PTR(pGeomIndexBuffers[iIndexBuffer].pMaterial))
				dsc.material = *pGeomIndexBuffers[iIndexBuffer].pMaterial;
			else
				dsc.material = SMaterial();

			RETURN_ON_ERR(pRenderer->RenderGeometry(dsc));
		}
	}
	*/

	return S_SUCCESS;
}






SP_NMSPACE_END