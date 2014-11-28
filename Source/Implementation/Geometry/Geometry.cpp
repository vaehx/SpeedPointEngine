//////////////////////////////////////////////////////////////////////////////////
//
// This file is part of the SpeedPointEngine
// Copyright (c) 2011-2014, iSmokiieZz
// ------------------------------------------------------------------------------
// Filename:	Geometry.cpp
// Created:	8/18/2014 by iSmokiieZz
// Description:
// -------------------------------------------------------------------------------
// History:
//
//////////////////////////////////////////////////////////////////////////////////

#include <Implementation\Geometry\Geometry.h>
#include <Implementation\Geometry\Material.h>
#include <Abstract\IResourcePool.h>
#include <Abstract\IRenderer.h>
#include <Abstract\IVertexBuffer.h>
#include <Abstract\IIndexBuffer.h>

SP_NMSPACE_BEG

// ----------------------------------------------------------------------------------------
S_API Geometry::Geometry()
: m_pRenderer(nullptr)
{
	//m_pMaterial = new Material();
}

// ----------------------------------------------------------------------------------------
S_API Geometry::~Geometry()
{
	Clear();
}

// ----------------------------------------------------------------------------------------
S_API void Geometry::Clear()
{	
	if (IS_VALID_PTR(m_pRenderer))
	{
		m_pRenderer->GetResourcePool()->RemoveIndexBuffer(&m_pIndexBuffer);
		m_pRenderer->GetResourcePool()->RemoveVertexBuffer(&m_pVertexBuffer);
		m_pRenderer = nullptr;
	}	

	/*
	if (IS_VALID_PTR(m_pMaterial))
		delete m_pMaterial;

	m_pMaterial = nullptr;
	*/
}

// ----------------------------------------------------------------------------------------
S_API SResult Geometry::Init(IGameEngine* pEngine, IRenderer* pRenderer, SInitialGeometryDesc* pInitialGeom /* = nullptr */)
{
	Clear();

	assert(pRenderer);
	m_pRenderer = pRenderer;
	
	if (Failure(pRenderer->GetResourcePool()->AddVertexBuffer(&m_pVertexBuffer)) ||
		Failure(pRenderer->GetResourcePool()->AddIndexBuffer(&m_pIndexBuffer)))
	{
		return S_ERROR;
	}

	// Init and fill pVB with initial geometry if given
	if (Failure(m_pVertexBuffer->Initialize(pEngine, pRenderer, eVBUSAGE_STATIC, 0)) ||
		Failure(m_pIndexBuffer->Initialize(pEngine, pRenderer, eIBUSAGE_STATIC, 0)))
	{
		return S_ERROR;
	}	


	if (IS_VALID_PTR(pInitialGeom))
	{
		if (Failure(m_pVertexBuffer->Fill(pInitialGeom->pVertices, pInitialGeom->nVertices, false)))
			return S_ERROR;

		if (Failure(m_pIndexBuffer->Fill(pInitialGeom->pIndices, pInitialGeom->nIndices, false)))
			return S_ERROR;
	}


	return S_SUCCESS;
}

// ----------------------------------------------------------------------------------------
S_API IIndexBuffer* Geometry::GetIndexBuffer()
{
	return m_pIndexBuffer;
}

// ----------------------------------------------------------------------------------------
S_API IVertexBuffer* Geometry::GetVertexBuffer()
{
	return m_pVertexBuffer;
}













// ----------------------------------------------------------------------------------------
/*
S_API SResult Geometry::Render()
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

	return m_pRenderer->RenderGeometry(dsc);
}
*/







SP_NMSPACE_END