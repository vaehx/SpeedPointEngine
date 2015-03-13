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
#include <Abstract\BoundBox.h>

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
	if (IS_VALID_PTR(m_pRenderer))
	{
		if (m_nSubsets > 0 && IS_VALID_PTR(m_pSubsets))
		{
			for (unsigned short iSubset = 0; iSubset < m_nSubsets; ++iSubset)
				m_pEngine->GetResources()->RemoveIndexBuffer(&m_pSubsets[iSubset].pIndexBuffer);

			delete[] m_pSubsets;
		}
	
		m_pSubsets = 0;
		m_nSubsets = 0;

		m_pRenderer->GetResourcePool()->RemoveVertexBuffer(&m_pVertexBuffer);
		m_pRenderer = nullptr;
	}

	if (IS_VALID_PTR(m_pEngine))
	{
		m_pEngine->UnregisterShutdownHandler(this);
		m_pEngine = nullptr;
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

	EVBUsage vbUsage = eVBUSAGE_STATIC;
	EIBUsage ibUsage = eIBUSAGE_STATIC;

	if (IS_VALID_PTR(pInitialGeom))
	{
		m_PrimitiveType = pInitialGeom->primitiveType;
		switch (pInitialGeom->vertexUsage)
		{
		case eGEOMUSE_MODIFY_RARELY: vbUsage = eVBUSAGE_DYNAMIC_RARE; break;
		case eGEOMUSE_MODIFY_FREQUENTLY: vbUsage = eVBUSAGE_DYNAMIC_FREQUENT; break;
		default:
		case eGEOMUSE_STATIC:
			vbUsage = eVBUSAGE_STATIC; break;
		}

		switch (pInitialGeom->indexUsage)
		{
		case eGEOMUSE_MODIFY_RARELY: ibUsage = eIBUSAGE_DYNAMIC_RARE; break;
		case eGEOMUSE_MODIFY_FREQUENTLY: ibUsage = eIBUSAGE_DYNAMIC_FREQUENT; break;
		default:
		case eGEOMUSE_STATIC:
			ibUsage = eIBUSAGE_STATIC; break;
		}
	}

	RETURN_ON_ERR(pRenderer->GetResourcePool()->AddVertexBuffer(&m_pVertexBuffer));
	RETURN_ON_ERR(m_pVertexBuffer->Initialize(pEngine, pRenderer, vbUsage));

	if (IS_VALID_PTR(pInitialGeom) && IS_VALID_PTR(pInitialGeom->pVertices))
	{
		SVertex* pVertices = pInitialGeom->pVertices;		
		if (pInitialGeom->bRequireNormalRecalc || pInitialGeom->bRequireTangentRecalc)
		{	
			// faces that contributed to each vertex, so that we can afterwards divide by this number
			// in order to properly average the normal.
			unsigned short *pVertexFaces = new unsigned short[pInitialGeom->nVertices];

			// first, reset all normals (and tangents) to (0,0,0)			
			for (usint32 iVtx = 0; iVtx < pInitialGeom->nVertices; ++iVtx)
			{
				if (pInitialGeom->bRequireNormalRecalc)
				{
					pVertices[iVtx].nx = 0.0f;
					pVertices[iVtx].ny = 0.0f;
					pVertices[iVtx].nz = 0.0f;
					pVertexFaces[iVtx] = 0;
				}

				if (pInitialGeom->bRequireTangentRecalc)
				{
					pVertices[iVtx].tx = 0.0f;
					pVertices[iVtx].ty = 0.0f;
					pVertices[iVtx].tz = 0.0f;
				}
			}						

			if (pInitialGeom->bRequireNormalRecalc)
			{
				for (usint32 iIndex = 0; iIndex < pInitialGeom->nIndices; iIndex += 3)
				{
					SVertex& vtx1 = pVertices[pInitialGeom->pIndices[iIndex]];
					SVertex& vtx2 = pVertices[pInitialGeom->pIndices[iIndex + 1]];
					SVertex& vtx3 = pVertices[pInitialGeom->pIndices[iIndex + 2]];

					SVector3 v1(vtx1.x, vtx1.y, vtx1.z), v2(vtx2.x, vtx2.y, vtx2.z), v3(vtx3.x, vtx3.y, vtx3.z);

					SVector3 normal = Vec3Normalize(Vec3Cross(v2 - v1, v3 - v1));

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

			// recalc of tangents has to be done after normal recalc
			if (pInitialGeom->bRequireTangentRecalc)
			{
				for (usint32 iIndex = 0; iIndex < pInitialGeom->nIndices; iIndex += 3)
				{
					SVertex& vtx1 = pVertices[pInitialGeom->pIndices[iIndex]];
					SVertex& vtx2 = pVertices[pInitialGeom->pIndices[iIndex + 1]];
					SVertex& vtx3 = pVertices[pInitialGeom->pIndices[iIndex + 2]];

					vtx1.CalcTangent(vtx2, vtx3);
					vtx2.tx = vtx1.tx; vtx2.ty = vtx1.ty; vtx2.tz = vtx1.tz;
					vtx3.tx = vtx1.tx; vtx3.ty = vtx1.ty; vtx3.tz = vtx1.tz;
				}				
			}
		}

		RETURN_ON_ERR(m_pVertexBuffer->Fill(pInitialGeom->pVertices, pInitialGeom->nVertices));
	}	
	
	bool bLines = (pInitialGeom->primitiveType == PRIMITIVE_TYPE_LINES);

	if (!IS_VALID_PTR(pInitialGeom)
		|| !IS_VALID_PTR(pInitialGeom->pMatIndexAssigns) || pInitialGeom->nMatIndexAssigns == 0
		|| bLines) // Lines can only have one material and only one subset
	{

		// No material assigns given.
		// init standard geometry with default (single) material
		m_pSubsets = new SGeomSubset[1];			
		m_nSubsets = 1;
		SGeomSubset* pDefSubset = &m_pSubsets[0];

		if (!bLines)
		{
			RETURN_ON_ERR(pRenderer->GetResourcePool()->AddIndexBuffer(&pDefSubset->pIndexBuffer));
			RETURN_ON_ERR(pDefSubset->pIndexBuffer->Initialize(pEngine, pRenderer, ibUsage, 0));

			if (IS_VALID_PTR(pInitialGeom) && IS_VALID_PTR(pInitialGeom->pIndices))
				RETURN_ON_ERR(pDefSubset->pIndexBuffer->Fill(pInitialGeom->pIndices, pInitialGeom->nIndices, false));
		}

		// Warning: FindMaterial() may return 0, which results in the renderer using default material
		pDefSubset->pMaterial = m_pEngine->GetMaterialManager()->FindMaterial(pInitialGeom->singleMatName);
	}
	else
	{
		// create and fill one subset per material
		m_nSubsets = pInitialGeom->nMatIndexAssigns;
		m_pSubsets = new SGeomSubset[m_nSubsets];
		unsigned long indexOffset = 0;
		for (unsigned short iSubset = 0; iSubset < m_nSubsets; ++iSubset)
		{			
			SMaterialIndices& matIndices = pInitialGeom->pMatIndexAssigns[iSubset];
			
			if (matIndices.nIdxIndices == 0)
				continue;

			SGeomSubset& subset = m_pSubsets[iSubset];

			RETURN_ON_ERR(pRenderer->GetResourcePool()->AddIndexBuffer(&subset.pIndexBuffer));
			RETURN_ON_ERR(subset.pIndexBuffer->Initialize(pEngine, pRenderer, ibUsage, 0));
			subset.pMaterial = matIndices.pMaterial;
			subset.indexOffset = indexOffset;

			// fill index buffer with according indices
			SIndex* pIndices = new SIndex[matIndices.nIdxIndices];
			indexOffset += matIndices.nIdxIndices;

			for (unsigned int iIdxIndex = 0; iIdxIndex < matIndices.nIdxIndices; ++iIdxIndex)
				pIndices[iIdxIndex] = pInitialGeom->pIndices[matIndices.pIdxIndices[iIdxIndex]];

			if (Failure(subset.pIndexBuffer->Fill(pIndices, matIndices.nIdxIndices, false)))
			{
				delete[] pIndices;
				return S_ERROR;
			}

			delete[] pIndices;
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
	for (unsigned short iSubset = 0; iSubset < m_nSubsets; ++iSubset)
	{
		SGeomSubset* pSubset = &m_pSubsets[iSubset];
		IIndexBuffer* pIndexBuffer = m_pSubsets[iSubset].pIndexBuffer;

		if (index > pSubset->indexOffset && index < pSubset->indexOffset + pIndexBuffer->GetIndexCount())
			return pIndexBuffer->GetIndex(index - pSubset->indexOffset);		
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
	if (!IS_VALID_PTR(m_pSubsets) || m_nSubsets == 0)
		return 0;

	unsigned long nIndices = 0;
	for (unsigned short iSubset = 0; iSubset < m_nSubsets; ++iSubset)
	{
		IIndexBuffer* pIndexBuffer = m_pSubsets[iSubset].pIndexBuffer;
		
		if (!IS_VALID_PTR(pIndexBuffer))
			continue;

		nIndices += pIndexBuffer->GetIndexCount();
	}

	return nIndices;
}



// ----------------------------------------------------------------------------------------
S_API void Geometry::CalculateBoundBox(AABB& aabb)
{
	if (!IS_VALID_PTR(m_pVertexBuffer) || m_pVertexBuffer->GetVertexCount() == 0)
		return;

	aabb.Reset();

	for (unsigned long iVtx = 0; iVtx < m_pVertexBuffer->GetVertexCount(); ++iVtx)
	{
		SVertex* pVertex = m_pVertexBuffer->GetVertex(iVtx);
		aabb.AddPoint(SVector3(pVertex->x, pVertex->y, pVertex->z));
	}
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