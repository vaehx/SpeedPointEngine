//////////////////////////////////////////////////////////////////////////////////
//
// This file is part of the SpeedPointEngine
// Copyright (c) 2011-2016, Pascal Rosenkranz
// ------------------------------------------------------------------------------
// Filename:	Geometry.cpp
// Created:		8/18/2014 by Pascal Rosenkranz
// Description:
// -------------------------------------------------------------------------------
// History:
//
//////////////////////////////////////////////////////////////////////////////////

#include "Geometry.h"
#include "Material.h"
#include "..\I3DEngine.h"
#include "C3DEngine.h"
#include <Renderer\IResourcePool.h>
#include <Renderer\IRenderer.h>
#include <Renderer\IVertexBuffer.h>
#include <Renderer\IIndexBuffer.h>
#include <Common\BoundBox.h>
#include <Common\FileUtils.h>
#include <FileSPM.h>

SP_NMSPACE_BEG

// ----------------------------------------------------------------------------------------
EVBUsage ParseVBUsage(EGeomUsage geomUsage)
{
	switch (geomUsage)
	{
	case eGEOMUSE_MODIFY_FREQUENTLY: return eVBUSAGE_DYNAMIC_FREQUENT;
	case eGEOMUSE_MODIFY_RARELY: return eVBUSAGE_DYNAMIC_RARE;
	default:
		return eVBUSAGE_STATIC;
	}
}

// ----------------------------------------------------------------------------------------
EIBUsage ParseIBUsage(EGeomUsage geomUsage)
{
	switch (geomUsage)
	{
	case eGEOMUSE_MODIFY_FREQUENTLY: return eIBUSAGE_DYNAMIC_FREQUENT;
	case eGEOMUSE_MODIFY_RARELY: return eIBUSAGE_DYNAMIC_RARE;
	default:
		return eIBUSAGE_STATIC;
	}
}

// ----------------------------------------------------------------------------------------
S_API void MakeSphere(SInitialGeometryDesc* geom, const Vec3f& pos, float radius, unsigned int nStripes, unsigned int nRings)
{
	if (!geom)
		return;

	geom->nVertices = (nStripes + 1) * (nRings + 1);
	geom->pVertices = new SVertex[geom->nVertices];

	auto* subset = geom->GetZeroSubset();
	subset->nIndices = (nStripes * nRings) * 6;
	subset->pIndices = new SIndex[subset->nIndices];

	geom->primitiveType = PRIMITIVE_TYPE_TRIANGLELIST;

	float dTheta = (float)SP_PI / (float)nRings;
	float dPhi = (float)SP_PI * 2.0f / (float)nStripes;

	float dU = 1.0f / (float)nRings;
	float dV = 1.0f / (float)nStripes;

	unsigned int indexAccum = 0;
	for (unsigned int ring = 0; ring <= nRings; ++ring)
	{
		float theta = (float)ring * dTheta;
		for (unsigned int stripe = 0; stripe <= nStripes; ++stripe)
		{
			float phi = (float)stripe * dPhi;

			Vec3f normal;
			normal.x = sinf(theta) * cosf(phi);
			normal.y = sinf(theta) * sinf(phi);
			normal.z = cosf(theta);

			geom->pVertices[ring * (nStripes + 1) + stripe] = SVertex(
				radius * normal.x, radius * normal.y, radius * normal.z,
				normal.x, normal.y, normal.z, 0, 0, 0, (float)stripe * dU, (float)ring * dV);

			if (ring < nRings && stripe < nStripes)
			{
				subset->pIndices[indexAccum] = ring * (nStripes + 1) + stripe;
				subset->pIndices[indexAccum + 1] = subset->pIndices[indexAccum] + (nStripes + 1) + 1;
				subset->pIndices[indexAccum + 2] = subset->pIndices[indexAccum] + 1;
				subset->pIndices[indexAccum + 3] = subset->pIndices[indexAccum];
				subset->pIndices[indexAccum + 4] = subset->pIndices[indexAccum] + (nStripes + 1);
				subset->pIndices[indexAccum + 5] = subset->pIndices[indexAccum + 1];
				indexAccum += 6;
			}
		}
	}
}



// ----------------------------------------------------------------------------------------
S_API CGeometry::CGeometry()
: m_pRenderer(0),
m_pVertexBuffer(0),
m_pSubsets(0),
m_nSubsets(0),
m_RefCount(0)
{
}

// ----------------------------------------------------------------------------------------
S_API CGeometry::~CGeometry()
{
	Clear();
}

// ----------------------------------------------------------------------------------------
S_API void CGeometry::AddRef()
{
	m_RefCount++;
}

// ----------------------------------------------------------------------------------------
S_API void CGeometry::Release()
{
	if (m_RefCount == 0)
		return;
	
	m_RefCount--;

	if (m_RefCount == 0)
		Clear();
}

// ----------------------------------------------------------------------------------------
S_API void CGeometry::Clear()
{
	if (IS_VALID_PTR(m_pRenderer))
	{
		if (m_nSubsets > 0 && IS_VALID_PTR(m_pSubsets))
		{
			for (unsigned short iSubset = 0; iSubset < m_nSubsets; ++iSubset)
				SP_SAFE_RELEASE(m_pSubsets[iSubset].pIndexBuffer);

			delete[] m_pSubsets;
		}
	
		m_pSubsets = 0;
		m_nSubsets = 0;

		SP_SAFE_RELEASE(m_pVertexBuffer);
		m_pRenderer = nullptr;
	}

	ShutdownManager::UnregisterShutdownHandler(this);
}

// ----------------------------------------------------------------------------------------
S_API void CGeometry::CalculateInitialNormalsOrTangents(const SInitialGeometryDesc* pInitialGeom)
{
	SVertex* pVertices = pInitialGeom->pVertices;

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
		// accumulate normals over all faces in all subsets
		for (u32 iSubset = 0; iSubset < pInitialGeom->nSubsets; ++iSubset)
		{
			const SInitialSubsetGeometryDesc& subset = pInitialGeom->pSubsets[iSubset];
			for (u32 iIndex = 0; iIndex < subset.nIndices; iIndex += 3)
			{
				SVertex& vtx1 = pVertices[subset.pIndices[iIndex]];
				SVertex& vtx2 = pVertices[subset.pIndices[iIndex + 1]];
				SVertex& vtx3 = pVertices[subset.pIndices[iIndex + 2]];

				SVector3 v1(vtx1.x, vtx1.y, vtx1.z), v2(vtx2.x, vtx2.y, vtx2.z), v3(vtx3.x, vtx3.y, vtx3.z);

				SVector3 normal = Vec3Normalize(Vec3Cross(v2 - v1, v3 - v1));

				vtx1.nx += normal.x;
				vtx1.ny += normal.y;
				vtx1.nz += normal.z;
				++pVertexFaces[subset.pIndices[iIndex]];

				vtx2.nx += normal.x;
				vtx2.ny += normal.y;
				vtx2.nz += normal.z;
				++pVertexFaces[subset.pIndices[iIndex + 1]];

				vtx3.nx += normal.x;
				vtx3.ny += normal.y;
				vtx3.nz += normal.z;
				++pVertexFaces[subset.pIndices[iIndex + 2]];
			}
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
		for (u32 iSubset = 0; iSubset < pInitialGeom->nSubsets; ++iSubset)
		{
			SInitialSubsetGeometryDesc& subset = pInitialGeom->pSubsets[iSubset];
			for (usint32 iIndex = 0; iIndex < subset.nIndices; iIndex += 3)
			{
				SVertex& vtx1 = pVertices[subset.pIndices[iIndex]];
				SVertex& vtx2 = pVertices[subset.pIndices[iIndex + 1]];
				SVertex& vtx3 = pVertices[subset.pIndices[iIndex + 2]];

				vtx1.CalcTangent(vtx2, vtx3);
				vtx2.tx = vtx1.tx; vtx2.ty = vtx1.ty; vtx2.tz = vtx1.tz;
				vtx3.tx = vtx1.tx; vtx3.ty = vtx1.ty; vtx3.tz = vtx1.tz;
			}
		}
	}
}

// ----------------------------------------------------------------------------------------
S_API SResult CGeometry::Init(IRenderer* pRenderer, const SInitialGeometryDesc* pInitialGeom /* = nullptr */)
{
	Clear();

	assert(pRenderer);
	m_pRenderer = pRenderer;

	ShutdownManager::RegisterShutdownHandler(this);

	EVBUsage vbUsage = eVBUSAGE_STATIC;
	EIBUsage ibUsage = eIBUSAGE_STATIC;

	if (IS_VALID_PTR(pInitialGeom))
	{
		m_File = pInitialGeom->geomFile;
		m_PrimitiveType = pInitialGeom->primitiveType;

		vbUsage = ParseVBUsage(pInitialGeom->vertexUsage);
		ibUsage = ParseIBUsage(pInitialGeom->indexUsage);
	}

	// Initialize vertices
	RETURN_ON_ERR(pRenderer->GetResourcePool()->AddVertexBuffer(&m_pVertexBuffer));
	RETURN_ON_ERR(m_pVertexBuffer->Initialize(pRenderer, vbUsage));

	if (IS_VALID_PTR(pInitialGeom))
	{
		if (!IS_VALID_PTR(pInitialGeom->pVertices))
			return CLog::Log(S_ERROR, "Cannot init geometry with initial geometry desc: pVertices is invalid!");

		if (pInitialGeom->bRequireNormalRecalc || pInitialGeom->bRequireTangentRecalc)
		{
			// Do not calculate normals or tangents if there is no subset defined
			if (IS_VALID_PTR(pInitialGeom->pSubsets) && pInitialGeom->nSubsets > 0)
				CalculateInitialNormalsOrTangents(pInitialGeom);
		}

		RETURN_ON_ERR(m_pVertexBuffer->Fill(pInitialGeom->pVertices, pInitialGeom->nVertices));
	}	


	// Initialize indices and materials
	bool bLines = (pInitialGeom->primitiveType == PRIMITIVE_TYPE_LINES);
	if (!IS_VALID_PTR(pInitialGeom)
		|| !IS_VALID_PTR(pInitialGeom->pSubsets) || pInitialGeom->nSubsets == 0
		|| bLines) // Lines can only have one material and only one subset
	{
		// No material assigns given.
		// init standard geometry with single (fallback) subset
		m_pSubsets = new SGeomSubset[1];
		m_nSubsets = 1;
		SGeomSubset* pDefSubset = &m_pSubsets[0];

		pDefSubset->pMaterial = 0;

		if (!bLines)
		{
			RETURN_ON_ERR(pRenderer->GetResourcePool()->AddIndexBuffer(&pDefSubset->pIndexBuffer));
			RETURN_ON_ERR(pDefSubset->pIndexBuffer->Initialize(pRenderer, ibUsage));

			if (IS_VALID_PTR(pInitialGeom) && IS_VALID_PTR(pInitialGeom->pSubsets) && pInitialGeom->nSubsets > 0)
			{
				SInitialSubsetGeometryDesc& subset = pInitialGeom->pSubsets[0];
				if (IS_VALID_PTR(subset.pIndices) && subset.nIndices > 0)
					RETURN_ON_ERR(pDefSubset->pIndexBuffer->Fill(subset.pIndices, subset.nIndices, false));

				pDefSubset->pMaterial = subset.pMaterial;
				pDefSubset->iMaterialDefinition = subset.iMaterialDefinition;
			}
		}
		else
		{
			if (IS_VALID_PTR(pInitialGeom) && IS_VALID_PTR(pInitialGeom->pSubsets) && pInitialGeom->nSubsets > 0)
			{
				pDefSubset->pMaterial = pInitialGeom->pSubsets[0].pMaterial;
				pDefSubset->iMaterialDefinition = pInitialGeom->pSubsets[0].iMaterialDefinition;
			}
		}
	}
	else
	{
		// create and fill one subset per material
		m_nSubsets = pInitialGeom->nSubsets;
		m_pSubsets = new SGeomSubset[m_nSubsets];
		unsigned long indexOffset = 0;
		for (unsigned short iSubset = 0; iSubset < m_nSubsets; ++iSubset)
		{
			SGeomSubset& subset = m_pSubsets[iSubset];
			SInitialSubsetGeometryDesc& subsetGeom = pInitialGeom->pSubsets[iSubset];

			if (!IS_VALID_PTR(subsetGeom.pIndices))
			{
				CLog::Log(S_ERROR, "pIndices is NULL of subset %d. Skipping...", iSubset);
				continue;
			}

			// Create the index buffer for this subset
			RETURN_ON_ERR(pRenderer->GetResourcePool()->AddIndexBuffer(&subset.pIndexBuffer));
			RETURN_ON_ERR(subset.pIndexBuffer->Initialize(pRenderer, ibUsage));

			subset.pMaterial = subsetGeom.pMaterial;
			subset.iMaterialDefinition = subsetGeom.iMaterialDefinition;
			subset.indexOffset = indexOffset;

			// fill index buffer with according indices
			SIndex* pIndices = new SIndex[subsetGeom.nIndices];
			indexOffset += subsetGeom.nIndices;

			for (unsigned int iIndex = 0; iIndex < subsetGeom.nIndices; ++iIndex)
				pIndices[iIndex] = subsetGeom.pIndices[iIndex];

			if (Failure(subset.pIndexBuffer->Fill(pIndices, subsetGeom.nIndices, false)))
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
S_API SResult CGeometry::CalculateNormalsGeometry(SInitialGeometryDesc& dsc, float fLineLength) const
{
	SVertex* pShadowBuffer = 0;
	if (!IS_VALID_PTR(m_pVertexBuffer) || !IS_VALID_PTR((pShadowBuffer = m_pVertexBuffer->GetShadowBuffer())))
		return S_NOTINIT;

	unsigned long nVertices = m_pVertexBuffer->GetVertexCount();

	dsc.primitiveType = PRIMITIVE_TYPE_LINES;

	dsc.nVertices = nVertices * 2;
	dsc.nSubsets = 0;

	dsc.pVertices = new SVertex[dsc.nVertices];
	dsc.pSubsets = 0;

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
S_API SVertex* CGeometry::GetVertex(unsigned long index)
{
	if (!IS_VALID_PTR(m_pVertexBuffer))
		return 0;

	if (index >= m_pVertexBuffer->GetVertexCount())
		return 0;

	SVertex* pVertices = m_pVertexBuffer->GetShadowBuffer();
	return &pVertices[index];
}

// ----------------------------------------------------------------------------------------
S_API SIndex* CGeometry::GetIndex(unsigned long index)
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
S_API SVertex* CGeometry::GetVertices()
{
	if (!IS_VALID_PTR(m_pVertexBuffer))
		return 0;

	return m_pVertexBuffer->GetShadowBuffer();
}

// ----------------------------------------------------------------------------------------
S_API unsigned long CGeometry::GetVertexCount() const
{
	if (!IS_VALID_PTR(m_pVertexBuffer))
		return 0;

	return m_pVertexBuffer->GetVertexCount();
}

// ----------------------------------------------------------------------------------------
S_API unsigned long CGeometry::GetIndexCount() const
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
S_API void CGeometry::CalculateBoundBox(AABB& aabb, const Mat44& transform)
{
	if (!IS_VALID_PTR(m_pVertexBuffer) || m_pVertexBuffer->GetVertexCount() == 0)
		return;

	aabb.Reset();

	for (unsigned long iVtx = 0; iVtx < m_pVertexBuffer->GetVertexCount(); ++iVtx)
	{
		SVertex* pVertex = m_pVertexBuffer->GetVertex(iVtx);
		Vec4f transformed = transform * Vec4f(pVertex->x, pVertex->y, pVertex->z, 1.0f);
		aabb.AddPoint(transformed.xyz());
	}
}








//////////////////////////////////////////////////////////////////////////////////

S_API CGeometryManager::~CGeometryManager()
{
	Clear();
}

// ----------------------------------------------------------------------------------------
S_API IGeometry* CGeometryManager::CreateGeometry(const SInitialGeometryDesc& desc, const string& name)
{
	CGeometry* pGeom = m_Geom.Get();
	pGeom->AddRef();
	if (Failure(pGeom->Init(C3DEngine::Get()->GetRenderer(), &desc)))
	{
		pGeom->Release();
		m_Geom.Release(&pGeom);
		CLog::Log(S_ERROR, "Failed create geometry '%s': CGeometry::Init() failed", name);
		return 0;
	}

	m_NameCache.insert(std::make_pair(name, pGeom));

	return pGeom;
}

// ----------------------------------------------------------------------------------------
S_API IGeometry* CGeometryManager::LoadGeometry(const string& spmResourcePath)
{
	// Check if already loaded
	auto found = m_NameCache.find(spmResourcePath);
	if (found != m_NameCache.end())
		return found->second;

	// Actually load the model
	CSPMLoader spmLoader;
	string modelDir = GetDirectoryPath(spmResourcePath);
	string filepath = C3DEngine::Get()->GetRenderer()->GetResourcePool()->GetResourceSystemPath(spmResourcePath);
	if (!spmLoader.Load(filepath.c_str()))
	{
		CLog::Log(S_ERROR, "Failed to load model file '%s'", filepath.c_str());
		return 0;
	}

	const std::vector<SModelMeta>& models = spmLoader.GetModels();

	SInitialGeometryDesc geomDesc;
	geomDesc.geomFile = spmResourcePath;
	geomDesc.bRequireNormalRecalc = true;
	geomDesc.bRequireTangentRecalc = true;
	geomDesc.indexUsage = eGEOMUSE_STATIC;
	geomDesc.vertexUsage = eGEOMUSE_STATIC;
	geomDesc.primitiveType = PRIMITIVE_TYPE_TRIANGLELIST;

	// Determine subset and vertex count
	for (auto itModel = models.begin(); itModel != models.end(); ++itModel)
	{
		geomDesc.nSubsets += itModel->nLoadedSubsets;
		geomDesc.nVertices += itModel->nVertices;
	}

	geomDesc.pSubsets = new SInitialSubsetGeometryDesc[geomDesc.nSubsets];
	geomDesc.pVertices = new SVertex[geomDesc.nVertices];

	// Flatten-out the subsets in all models into a single array of subsets	
	IMaterialManager* pMatMgr = C3DEngine::Get()->GetMaterialManager();
	u32 vtxOffset = 0;
	unsigned int iSubset = 0;
	for (auto itModel = models.begin(); itModel != models.end(); ++itModel)
	{
		// Copy over vertices
		for (u32 iVtx = 0; iVtx < itModel->nVertices; ++iVtx)
		{
			geomDesc.pVertices[vtxOffset + iVtx] = itModel->pVertices[iVtx];
		}

		// itModel->materialFile may already be absolute
		string absMaterialFile = MakePathAbsolute(itModel->materialFile, modelDir);

		IMaterial* pMaterial = pMatMgr->LoadMaterial(absMaterialFile);
		for (u16 iModelSubset = 0; iModelSubset < itModel->nLoadedSubsets; ++iModelSubset)
		{
			const SSubset& modelSubset = itModel->pSubsets[iModelSubset];
			SInitialSubsetGeometryDesc& subset = geomDesc.pSubsets[iSubset];
			subset.nIndices = modelSubset.nIndices;
			subset.pIndices = new SIndex[subset.nIndices];

			// TODO: Use SIndex in SPMFile, so we don't have to copy the indices here!
			for (u32 iIndex = 0; iIndex < subset.nIndices; ++iIndex)
				subset.pIndices[iIndex] = modelSubset.pIndices[iIndex] + vtxOffset;

			if (pMaterial)
			{
				subset.pMaterial = pMaterial;
				subset.iMaterialDefinition = pMaterial->GetDefinitionIndex(modelSubset.materialDefinition);
			}

			iSubset++;
		}

		vtxOffset += itModel->nVertices;
	}

	for (auto itModel = models.begin(); itModel != models.end(); ++itModel)
	{
		for (u16 iSubset = 0; iSubset < itModel->nSubsets; ++iSubset)
			delete[] itModel->pSubsets[iSubset].pIndices;

		delete[] itModel->pSubsets;
		delete[] itModel->pVertices;
	}

	return CreateGeometry(geomDesc, spmResourcePath);
}

// ----------------------------------------------------------------------------------------
S_API void CGeometryManager::GarbageCollect()
{
	// Release objects with 0 refcount
	unsigned int iGeom;
	for (CGeometry* pGeom = m_Geom.GetFirstUsedObject(iGeom); pGeom; pGeom = m_Geom.GetNextUsedObject(iGeom))
	{
		if (pGeom->GetRefCount() == 0)
			m_Geom.Release(&pGeom);
	}

	// Remove name cache entries with invalid pointer
	for (auto itEntry = m_NameCache.begin(); itEntry != m_NameCache.end();)
	{
		if (!m_Geom.IsValidPtr(itEntry->second))
			itEntry = m_NameCache.erase(itEntry);
		else
			itEntry++;
	}
}

// ----------------------------------------------------------------------------------------
S_API void CGeometryManager::Clear()
{
	unsigned int numUsed = 0;
	unsigned int iGeom;
	for (CGeometry* pGeom = m_Geom.GetFirstUsedObject(iGeom); pGeom; pGeom = m_Geom.GetNextUsedObject(iGeom))
	{
		if (pGeom->GetRefCount() > 0)
			CLog::Log(S_WARN, "CGeometryManager::Clear(): Geometry still has %u references!", pGeom->GetRefCount());
	}

	m_Geom.Clear();
	m_NameCache.clear();
}




SP_NMSPACE_END
