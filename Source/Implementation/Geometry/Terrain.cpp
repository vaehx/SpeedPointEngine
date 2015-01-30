// SpeedPoint Basic Terrain

#include <Implementation\Geometry\Terrain.h>
#include <Abstract\IGameEngine.h>
#include <Abstract\IRenderer.h>
#include <Abstract\IVertexBuffer.h>
#include <Abstract\IIndexBuffer.h>
#include <Abstract\IResourcePool.h>


SP_NMSPACE_BEG

// -------------------------------------------------------------------------------------------------------------------
S_API Terrain::~Terrain()
{
	Clear();
}

// -------------------------------------------------------------------------------------------------------------------
S_API SResult Terrain::Initialize(IGameEngine* pEngine)
{	
	SP_ASSERTR(m_pEngine = pEngine, S_INVALIDPARAM);	

	return S_SUCCESS;
}

// -------------------------------------------------------------------------------------------------------------------
S_API SResult Terrain::CreatePlanar(const STerrainDescription& tdsc)
{





	// ........  TODO





	// Fill vertex buffer

	if (Failure(m_pEngine->GetResources()->AddVertexBuffer(&m_pVertexBuffer)))
		return EngLog(S_ERROR, m_pEngine, "Failed add vertex buffer for terrain!");

	if (Failure(m_pVertexBuffer->Initialize(m_pEngine, m_pEngine->GetRenderer(), tdsc.bDynamic ? eVBUSAGE_DYNAMIC_RARE : eVBUSAGE_STATIC, 0, 0)))
		return EngLog(S_ERROR, m_pEngine, "Failed init Vertex Buffer for Terrain!");

	//if (Failure(m_pVertexBuffer->Fill(pVertices, nVertices)))
	//	return EngLog(S_ERROR, m_pEngine, "Failed fill vertex buffer for terrain!");



	// Fill index buffers

	for (unsigned int iIndexBuffer = 0; iIndexBuffer < 2; ++iIndexBuffer)
	{
		if (Failure(m_pEngine->GetResources()->AddIndexBuffer(&m_pIndexBuffer[iIndexBuffer])))
			return EngLog(S_ERROR, m_pEngine, "Failed add index buffer #%d for terrain!", iIndexBuffer);

		/*
		if (Failure(m_pIndexBuffer[iIndexBuffer]->Initialize(m_pEngine, m_pEngine->GetRenderer(), eIBUSAGE_STATIC, nIndices[iIndexBuffer], 0)))
			return EngLog(S_ERROR, m_pEngine, "Failed init index buffer #%d for terrain!", iIndexBuffer);
		*/

		/*
		if (Failure(m_pIndexBuffer[iIndexBuffer]->Fill(pIndices[iIndexBuffer], nIndices[iIndexBuffer], false)))
			return EngLog(S_ERROR, m_pEngine, "Failed fill index buffer #%d for terrain!", iIndexBuffer);
		*/
	}


	m_bRequireCBUpdate = true;

	return RecalculateNormals();
}

// -------------------------------------------------------------------------------------------------------------------
S_API SResult Terrain::RecalculateNormals()
{	
	if (!IS_VALID_PTR(m_pVertexBuffer))
		return S_NOTINIT;

	// Calculate normals	
	for (unsigned long iVtx = 0; iVtx < m_pVertexBuffer->GetVertexCount(); ++iVtx)
	{
		SVertex* vtx = m_pVertexBuffer->GetVertex(iVtx);
		unsigned int iX = iVtx % (m_TerrainDsc.nX + 1),
			iZ = (unsigned int)((iVtx - iX) / (m_TerrainDsc.nX + 1));

		SVector3 dirAcc(0, 0, 0);
		unsigned int nAffectedVertices = 1;

		// 1-2
		if (iX > 0 && iZ > 0)
		{
			const SVertex *vtx1 = m_pVertexBuffer->GetVertex(iVtx - 1),
				*vtx2 = m_pVertexBuffer->GetVertex(iVtx - (m_TerrainDsc.nX + 1));

			dirAcc += SVector3Normalize(SVector3Cross(
				SVector3(vtx1->x - vtx->x, vtx1->y - vtx->y, vtx1->z - vtx->z),
				SVector3(vtx2->x - vtx->x, vtx2->y - vtx->y, vtx2->z - vtx->z)));
		}
		else
			dirAcc += SVector3(0, 1.0f, 0);

		// 2-3
		if (iX < m_TerrainDsc.nX && iZ > 0)
		{
			const SVertex *vtx2 = m_pVertexBuffer->GetVertex(iVtx - (m_TerrainDsc.nX + 1)),
				*vtx3 = m_pVertexBuffer->GetVertex(iVtx + 1);

			dirAcc += SVector3Normalize(SVector3Cross(
				SVector3(vtx2->x - vtx->x, vtx2->y - vtx->y, vtx2->z - vtx->z),
				SVector3(vtx3->x - vtx->x, vtx3->y - vtx->y, vtx3->z - vtx->z)));
		}
		else
			dirAcc += SVector3(0, 1.0f, 0);

		// 3 - 4
		if (iX < m_TerrainDsc.nX && iZ < m_TerrainDsc.nZ)
		{
			const SVertex *vtx3 = m_pVertexBuffer->GetVertex(iVtx + 1),
				*vtx4 = m_pVertexBuffer->GetVertex(iVtx + (m_TerrainDsc.nX + 1));

			dirAcc += SVector3Normalize(SVector3Cross(
				SVector3(vtx3->x - vtx->x, vtx3->y - vtx->y, vtx3->z - vtx->z),
				SVector3(vtx4->x - vtx->x, vtx4->y - vtx->y, vtx4->z - vtx->z)));
		}
		else
			dirAcc += SVector3(0, 1.0f, 0);

		// 4 - 1
		if (iX > 0 && iZ < m_TerrainDsc.nZ)
		{
			const SVertex *vtx4 = m_pVertexBuffer->GetVertex(iVtx + (m_TerrainDsc.nX + 1)),
				*vtx1 = m_pVertexBuffer->GetVertex(iVtx - 1);

			dirAcc += SVector3Normalize(SVector3Cross(
				SVector3(vtx4->x - vtx->x, vtx4->y - vtx->y, vtx4->z - vtx->z),
				SVector3(vtx1->x - vtx->x, vtx1->y - vtx->y, vtx1->z - vtx->z)));
		}
		else
			dirAcc += SVector3(0, 1.0f, 0);

		SVector3 dirAccN = SVector3Normalize(dirAcc);

		if (dirAccN.y < 0)
			dirAccN = -dirAccN;

		vtx->nx = dirAccN.x;
		vtx->ny = dirAccN.y;
		vtx->nz = dirAccN.z;
	}

	return S_SUCCESS;
}

// -------------------------------------------------------------------------------------------------------------------
S_API SResult Terrain::RenderTerrain(const SVector3& lodCenterPos)
{
	SP_ASSERTR(IS_VALID_PTR(m_pEngine), S_NOTINIT);
	
	if (!IS_VALID_PTR(m_pVertexBuffer) || !IS_VALID_PTR(m_pIndexBuffer[0]) || m_pVertexBuffer->GetVertexCount() == 0 || m_pIndexBuffer[0]->GetIndexCount() == 0)
		return m_pEngine->LogE("Failed RenderTerrain: Terrain geometry not initialized!");

	IRenderer* pRenderer = m_pEngine->GetRenderer();

	STerrainRenderDesc dsc;
	dsc.drawCallDesc.pVertexBuffer = m_pVertexBuffer;	
	dsc.drawCallDesc.iStartVBIndex = 0;
	dsc.drawCallDesc.iEndVBIndex = dsc.drawCallDesc.pVertexBuffer->GetVertexCount() - 1;
		
	dsc.drawCallDesc.iStartIBIndex = 0;	

	//dsc.pGeometry = &m_Geometry;
	dsc.pGeometry = 0;
	dsc.pColorMap = m_pColorMap;
	dsc.pDetailMap = m_pDetailMap;
	dsc.bRender = true;
	SMatrixIdentity(&dsc.drawCallDesc.transform.scale);	
	
	dsc.bUpdateCB = false;

	if (m_bRequireCBUpdate)
	{
		dsc.bUpdateCB = true;
		dsc.constants.dmTexRatioU = m_fDMTexScaleU;
		dsc.constants.dmTexRatioV = m_fDMTexScaleV;
		dsc.constants.fTerrainDMFadeRadius = m_pEngine->GetSettings()->Get().render.fTerrainDMFadeRange;
		m_bRequireCBUpdate = false;
	}

	// Render each chunk	
	unsigned long chunkStartDiff = (m_nChunkBigQuads + m_nChunkSmallQuads) * 6;
	for (unsigned long iChnkStart = 0; iChnkStart < m_TerrainDsc.nX * m_TerrainDsc.nZ; iChnkStart += chunkStartDiff)
	{

	}

	/*
	for (unsigned int iIndexBuffer = 0; iIndexBuffer < 2; ++iIndexBuffer)
	{
		dsc.drawCallDesc.pIndexBuffer = m_pIndexBuffer[iIndexBuffer];
		dsc.drawCallDesc.iEndIBIndex = dsc.drawCallDesc.pIndexBuffer->GetIndexCount() - 1;

		if (Failure(pRenderer->RenderTerrain(dsc)))
			return S_ERROR;
	}
	*/

	return S_SUCCESS;
}

// -------------------------------------------------------------------------------------------------------------------
S_API SResult Terrain::SetColorMap(ITexture* pColorMap)
{
	if (!IS_VALID_PTR(pColorMap))
		EngLog(S_INVALIDPARAM, m_pEngine, "Invalid ptr passed to Terrain::SetColorMap!");

	m_pColorMap = pColorMap;
	return S_SUCCESS;
}

// -------------------------------------------------------------------------------------------------------------------
S_API SResult Terrain::SetDetailMap(ITexture* pDetailMap)
{
	if (!IS_VALID_PTR(pDetailMap))
		EngLog(S_INVALIDPARAM, m_pEngine, "Invalid trp passed to Terrain::SetDetailMap!");

	m_pDetailMap = pDetailMap;
	return S_SUCCESS;
}

// -------------------------------------------------------------------------------------------------------------------
S_API IVertexBuffer* Terrain::GetVertexBuffer()
{
	return m_pVertexBuffer;	
}

// -------------------------------------------------------------------------------------------------------------------
S_API IIndexBuffer* Terrain::GetIndexBuffer(unsigned int lodLevel)
{
	return m_pIndexBuffer[lodLevel];
}

// -------------------------------------------------------------------------------------------------------------------
S_API SResult Terrain::Clear(void)
{	
	if (IS_VALID_PTR(m_pEngine))
	{
		IResourcePool* pResourcePool = m_pEngine->GetResources();
		if (IS_VALID_PTR(pResourcePool))
		{
			pResourcePool->RemoveIndexBuffer(&m_pIndexBuffer[0]);
			pResourcePool->RemoveIndexBuffer(&m_pIndexBuffer[1]);
			pResourcePool->RemoveVertexBuffer(&m_pVertexBuffer);
		}

		m_pEngine = 0;
	}

	return S_SUCCESS;
}


SP_NMSPACE_END