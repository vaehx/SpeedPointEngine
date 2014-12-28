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
: m_pRenderer(nullptr),
m_pEngine(nullptr)
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
	if (IS_VALID_PTR(m_pEngine))
	{
		m_pEngine->UnregisterShutdownHandler(this);
		m_pEngine = nullptr;
	}

	if (IS_VALID_PTR(m_pRenderer))
	{
		if (m_nIndexBuffers == 1)
		{
			m_pRenderer->GetResourcePool()->RemoveIndexBuffer(&m_pIndexBuffers->pIndexBuffer);
			delete m_pIndexBuffers;
		}
		else if (m_nIndexBuffers > 1)
		{
			for (unsigned short iIndexBuffer = 0; iIndexBuffer < m_nIndexBuffers; ++iIndexBuffer)
				m_pRenderer->GetResourcePool()->RemoveIndexBuffer(&m_pIndexBuffers[iIndexBuffer].pIndexBuffer);		

			delete[] m_pIndexBuffers;
		}
		m_pIndexBuffers = 0;
		m_nIndexBuffers = 0;

		m_pRenderer->GetResourcePool()->RemoveVertexBuffer(&m_pVertexBuffer);
		m_pRenderer = nullptr;
	}
}

// ----------------------------------------------------------------------------------------
S_API SResult Geometry::Init(IGameEngine* pEngine, IRenderer* pRenderer, SInitialGeometryDesc* pInitialGeom /* = nullptr */)
{
	Clear();

	assert(pRenderer);
	m_pRenderer = pRenderer;

	m_pEngine = pEngine;
	m_pEngine->RegisterShutdownHandler(this);	

	if (IS_VALID_PTR(pInitialGeom))
		m_PrimitiveType = pInitialGeom->primitiveType;

	RETURN_ON_ERR(pRenderer->GetResourcePool()->AddVertexBuffer(&m_pVertexBuffer));
	RETURN_ON_ERR(m_pVertexBuffer->Initialize(pEngine, pRenderer, eVBUSAGE_STATIC, 0));

	if (IS_VALID_PTR(pInitialGeom) && IS_VALID_PTR(pInitialGeom->pVertices))
	{
		SVertex* pVertices = pInitialGeom->pVertices;		
		if (pInitialGeom->bRequireNormalRecalc)
		{	
			// faces that contributed to each vertex, so that we can afterwards divide by this number
			// in order to properly average the normal.
			unsigned short *pVertexFaces = new unsigned short[pInitialGeom->nVertices];

			// first, reset all normals to (0,0,0)			
			for (usint32 iVtx = 0; iVtx < pInitialGeom->nVertices; ++iVtx)
			{
				pVertices[iVtx].nx = 0.0f;
				pVertices[iVtx].ny = 0.0f;
				pVertices[iVtx].nz = 0.0f;
				pVertexFaces[iVtx] = 0;
			}						

			for (usint32 iIndex = 0; iIndex < pInitialGeom->nIndices; iIndex += 3)
			{
				SVertex& vtx1 = pVertices[pInitialGeom->pIndices[iIndex]];
				SVertex& vtx2 = pVertices[pInitialGeom->pIndices[iIndex + 1]];
				SVertex& vtx3 = pVertices[pInitialGeom->pIndices[iIndex + 2]];

				SVector3 v1(vtx1.x, vtx1.y, vtx1.z), v2(vtx2.x, vtx2.y, vtx2.z), v3(vtx3.x, vtx3.y, vtx3.z);

				SVector3 normal = SVector3Normalize(SVector3Cross(v2 - v1, v3 - v1));
				
				vtx1.nx += normal.x;
				vtx1.ny += normal.y;
				vtx1.nz += normal.z;
				++pVertexFaces[pInitialGeom->pIndices[iIndex]];

				vtx2.nx += normal.x;
				vtx2.ny += normal.y;
				vtx2.nz += normal.z;
				++pVertexFaces[pInitialGeom->pIndices[iIndex + 1]];

				vtx3.nx += normal.x;
				vtx3.ny += normal.y;
				vtx3.nz += normal.z;
				++pVertexFaces[pInitialGeom->pIndices[iIndex + 2]];
			}

			// now average all normals
			for (usint32 iVtx = 0; iVtx < pInitialGeom->nVertices; ++iVtx)
			{
				float k = 1.0f / (float)pVertexFaces[iVtx];
				pVertices[iVtx].nx *= k;
				pVertices[iVtx].ny *= k;
				pVertices[iVtx].nz *= k;
			}
		}

		RETURN_ON_ERR(m_pVertexBuffer->Fill(pInitialGeom->pVertices, pInitialGeom->nVertices, false));
	}	

	if (m_PrimitiveType != PRIMITIVE_TYPE_LINES)
	{
		if (!IS_VALID_PTR(pInitialGeom) || !IS_VALID_PTR(pInitialGeom->pMatIndexAssigns) || pInitialGeom->nMatIndexAssigns == 0)
		{
			m_pIndexBuffers = new SGeometryIndexBuffer();
			m_nIndexBuffers = 1;
			RETURN_ON_ERR(pRenderer->GetResourcePool()->AddIndexBuffer(&m_pIndexBuffers->pIndexBuffer));
			RETURN_ON_ERR(m_pIndexBuffers->pIndexBuffer->Initialize(pEngine, pRenderer, eIBUSAGE_STATIC, 0));

			if (IS_VALID_PTR(pInitialGeom) && IS_VALID_PTR(pInitialGeom->pIndices))
				RETURN_ON_ERR(m_pIndexBuffers->pIndexBuffer->Fill(pInitialGeom->pIndices, pInitialGeom->nIndices, false));


			m_pIndexBuffers->pMaterial = (IS_VALID_PTR(pInitialGeom) ? pInitialGeom->pSingleMaterial : m_pEngine->GetDefaultMaterial());
		}
		else
		{
			// Create and fill one Index buffer per material
			m_nIndexBuffers = pInitialGeom->nMatIndexAssigns;
			m_pIndexBuffers = new SGeometryIndexBuffer[m_nIndexBuffers];
			for (unsigned short iIndexBuffer = 0; iIndexBuffer < m_nIndexBuffers; ++iIndexBuffer)
			{
				SMaterialIndices& matIndices = pInitialGeom->pMatIndexAssigns[iIndexBuffer];
				SGeometryIndexBuffer& geomIndexBuffer = m_pIndexBuffers[iIndexBuffer];
				RETURN_ON_ERR(pRenderer->GetResourcePool()->AddIndexBuffer(&geomIndexBuffer.pIndexBuffer));
				RETURN_ON_ERR(geomIndexBuffer.pIndexBuffer->Initialize(pEngine, pRenderer, eIBUSAGE_STATIC, 0));
				geomIndexBuffer.pMaterial = matIndices.pMaterial;

				// fill index buffer with according indices
				SIndex* pIndices = new SIndex[matIndices.nIdxIndices];
				for (unsigned int iIdxIndex = 0; iIdxIndex < matIndices.nIdxIndices; ++iIdxIndex)
					pIndices[iIdxIndex] = pInitialGeom->pIndices[matIndices.pIdxIndices[iIdxIndex]];

				if (Failure(geomIndexBuffer.pIndexBuffer->Fill(pIndices, matIndices.nIdxIndices, false)))
				{
					delete[] pIndices;
					return S_ERROR;
				}

				delete[] pIndices;
			}
		}
	}

	return S_SUCCESS;
}


// ----------------------------------------------------------------------------------------
S_API SResult Geometry::CalculateNormalsGeometry(SInitialGeometryDesc& dsc, float fLineLength) const
{
	SVertex* pShadowBuffer = 0;
	if (!IS_VALID_PTR(m_pVertexBuffer) || !IS_VALID_PTR((pShadowBuffer = m_pVertexBuffer->GetShadowBuffer())))
		return S_NOTINIT;

	unsigned long nVertices = m_pVertexBuffer->GetVertexCount();

	dsc.primitiveType = PRIMITIVE_TYPE_LINES;

	dsc.nVertices = nVertices * 2;
	dsc.nIndices = 0; // lines are not drawn using indices	

	dsc.pVertices = new SVertex[dsc.nVertices];
	dsc.pIndices = 0;

	dsc.bRequireNormalRecalc = false;

	for (unsigned long iVertex = 0; iVertex < nVertices; ++iVertex)
	{
		dsc.pVertices[iVertex * 2] = pShadowBuffer[iVertex];
		dsc.pVertices[iVertex * 2 + 1] = pShadowBuffer[iVertex];
		
		SVertex& outerVtx = dsc.pVertices[iVertex * 2 + 1];
		outerVtx.x = outerVtx.x + outerVtx.nx * fLineLength;
		outerVtx.y = outerVtx.y + outerVtx.ny * fLineLength;
		outerVtx.z = outerVtx.z + outerVtx.nz * fLineLength;				
	}

	return S_SUCCESS;
}

// ----------------------------------------------------------------------------------------
S_API SVertex* Geometry::GetVertex(unsigned long index)
{
	if (!IS_VALID_PTR(m_pVertexBuffer))
		return 0;

	if (index >= m_pVertexBuffer->GetVertexCount())
		return 0;

	SVertex* pVertices = m_pVertexBuffer->GetShadowBuffer();
	return &pVertices[index];
}

// ----------------------------------------------------------------------------------------
S_API SIndex* Geometry::GetIndex(unsigned long index)
{
	unsigned long indexCount = 0;
	for (unsigned short iIndexBuffer = 0; iIndexBuffer < m_nIndexBuffers; ++iIndexBuffer)
	{
		IIndexBuffer* pIndexBuffer = m_pIndexBuffers[iIndexBuffer].pIndexBuffer;
		if (index > indexCount && index < indexCount + pIndexBuffer->GetIndexCount())
			return pIndexBuffer->GetIndex(index - indexCount);
		else
			indexCount += pIndexBuffer->GetIndexCount();
	}

	return 0;
}

// ----------------------------------------------------------------------------------------
S_API SVertex* Geometry::GetVertices()
{
	if (!IS_VALID_PTR(m_pVertexBuffer))
		return 0;

	return m_pVertexBuffer->GetShadowBuffer();
}

// ----------------------------------------------------------------------------------------
S_API unsigned long Geometry::GetVertexCount() const
{
	if (!IS_VALID_PTR(m_pVertexBuffer))
		return 0;

	return m_pVertexBuffer->GetVertexCount();
}

// ----------------------------------------------------------------------------------------
S_API unsigned long Geometry::GetIndexCount() const
{
	if (!IS_VALID_PTR(m_pIndexBuffers) || m_nIndexBuffers == 0)
		return 0;

	unsigned long nIndices = 0;
	for (unsigned short iIndexBuffer = 0; iIndexBuffer < m_nIndexBuffers; ++iIndexBuffer)
	{
		if (!IS_VALID_PTR(m_pIndexBuffers[iIndexBuffer].pIndexBuffer))
			continue;

		nIndices += m_pIndexBuffers[iIndexBuffer].pIndexBuffer->GetIndexCount();
	}

	return nIndices;
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