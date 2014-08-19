//////////////////////////////////////////////////////////////////////////////////
//
// This file is part of the SpeedPointEngine
// Copyright (c) 2011-2014, iSmokiieZz
// ------------------------------------------------------------------------------
// Filename:	VisibleObject.cpp
// Created:	8/19/2014 by iSmokiieZz
// Description:
// -------------------------------------------------------------------------------
// History:
//
//////////////////////////////////////////////////////////////////////////////////

#include <Implementation\Geometry\VisibleObject.h>
#include <Implementation\Geometry\Material.h>
#include <Abstract\IResourcePool.h>
#include <Abstract\IRenderer.h>
#include <Abstract\IVertexBuffer.h>
#include <Abstract\IIndexBuffer.h>

SP_NMSPACE_BEG

// ----------------------------------------------------------------------------------------
S_API VisibleObject::VisibleObject()
: m_pRenderer(nullptr)
{
	m_pMaterial = new Material();
}

// ----------------------------------------------------------------------------------------
S_API VisibleObject::~VisibleObject()
{
	Clear();
}

// ----------------------------------------------------------------------------------------
S_API void VisibleObject::Clear()
{	
	if (IS_VALID_PTR(m_pRenderer))
	{
		m_pRenderer->GetResourcePool()->RemoveIndexBuffer(m_iIndexBuffer);
		m_pRenderer->GetResourcePool()->RemoveVertexBuffer(m_iVertexBuffer);
		m_pRenderer = nullptr;
	}	

	m_iIndexBuffer = SP_ID();
	m_iVertexBuffer = SP_ID();

	if (IS_VALID_PTR(m_pMaterial))
		delete m_pMaterial;

	m_pMaterial = nullptr;
}

// ----------------------------------------------------------------------------------------
S_API SResult VisibleObject::Init(IGameEngine* pEngine, IRenderer* pRenderer, SInitialGeometryDesc* pInitialGeom /* = nullptr */)
{
	Clear();

	assert(pRenderer);
	m_pRenderer = pRenderer;

	IVertexBuffer* pVB;
	IIndexBuffer* pIB;
	if (Failure(pRenderer->GetResourcePool()->AddVertexBuffer(&pVB, &m_iVertexBuffer)))
		return S_ERROR;

	if (Failure(pRenderer->GetResourcePool()->AddIndexBuffer(&pIB, &m_iIndexBuffer)))
		return S_ERROR;

	// Init and fill pVB with initial geometry if given
	if (Failure(pVB->Initialize(pEngine, pRenderer, eVBUSAGE_STATIC, 0)))
		return S_ERROR;

	if (Failure(pIB->Initialize(pEngine, pRenderer, eIBUSAGE_STATIC, 0)))
		return S_ERROR;


	if (IS_VALID_PTR(pInitialGeom))
	{
		if (Failure(pVB->Fill(pInitialGeom->pVertices, pInitialGeom->nVertices, false)))
			return S_ERROR;

		if (Failure(pIB->Fill(pInitialGeom->pIndices, pInitialGeom->nIndices, false)))
			return S_ERROR;
	}


	return S_SUCCESS;
}

// ----------------------------------------------------------------------------------------
S_API IIndexBuffer* VisibleObject::GetIndexBuffer() const
{
	if (!IS_VALID_PTR(m_pRenderer))
		return nullptr;

	return m_pRenderer->GetResourcePool()->GetIndexBuffer(m_iIndexBuffer);
}

// ----------------------------------------------------------------------------------------
S_API IVertexBuffer* VisibleObject::GetVertexBuffer() const
{
	if (!IS_VALID_PTR(m_pRenderer))
		return nullptr;

	return m_pRenderer->GetResourcePool()->GetVertexBuffer(m_iVertexBuffer);
}

// ----------------------------------------------------------------------------------------
S_API IMaterial* VisibleObject::GetMaterial() const
{
	return nullptr;
}













// ----------------------------------------------------------------------------------------
S_API SResult VisibleObject::Render()
{
	if (!IS_VALID_PTR(m_pRenderer))
		return S_NOTINIT;

	IVertexBuffer* pVB = GetVertexBuffer();
	if (!IS_VALID_PTR(pVB))
		return S_ERROR;

	IIndexBuffer* pIB = GetIndexBuffer();
	if (!IS_VALID_PTR(pIB))
		return S_ERROR;

	SRenderDesc dsc;
	dsc.drawCallDesc.pVertexBuffer = pVB;
	dsc.drawCallDesc.iStartVBIndex = 0;
	dsc.drawCallDesc.iEndVBIndex = pVB->GetVertexCount() - 1;
	dsc.drawCallDesc.pIndexBuffer = pIB;
	dsc.drawCallDesc.iStartIBIndex = 0;
	dsc.drawCallDesc.iEndIBIndex = pIB->GetIndexCount() - 1;

	dsc.pGeometrical = this;
	dsc.technique = eRENDER_FORWARD;
	dsc.drawCallDesc.transform.translation = SMatrix::MakeTranslationMatrix(vPosition);
	dsc.drawCallDesc.transform.rotation = SMatrix::MakeRotationMatrix(vRotation);
	dsc.drawCallDesc.transform.scale = SMatrix::MakeScaleMatrix(vSize);

	return m_pRenderer->RenderGeometry(dsc);
}








SP_NMSPACE_END