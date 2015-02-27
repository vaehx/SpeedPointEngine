// SpeedPoint Basic Terrain

#include <Implementation\Geometry\Terrain.h>
#include <Abstract\IGameEngine.h>
#include <Abstract\IRenderer.h>
#include <Abstract\IVertexBuffer.h>
#include <Abstract\IIndexBuffer.h>
#include <Abstract\IResourcePool.h>


SP_NMSPACE_BEG

// -------------------------------------------------------------------------------------------------------------------
S_API void STerrainChunk::AddQuad(unsigned int lodLevel, unsigned int w, unsigned int d,
	unsigned long iStartVtx, unsigned long iStartIndex, unsigned long vtxRowShift, float baseHeight, float xoffs, float zoffs, float uoffs, float voffs)
{
	// generate vertices
	if (!pVertices[lodLevel] || !pIndices[lodLevel])
		return; // unexpected critical

	float quadWidth = (fWidth / (float)nSegsX) * (float)w;
	float quadDepth = (fDepth / (float)nSegsZ) * (float)d;	

	float tangent[3] = { -1.0f, 0, 0 };	

	if (iStartVtx + vtxRowShift + 1 > pLodCounts[lodLevel].vertexCount)
	{
		printf("Warn: Vertex index %u exceeded vertex count of %u in STerrainChunk::AddQuad()!\n",
			iStartVtx + vtxRowShift + 1, pLodCounts[lodLevel].vertexCount);

		return;
	}


	pVertices[lodLevel][iStartVtx] = SVertex(xoffs, baseHeight, zoffs, 0, 1.0f, 0, tangent[0], tangent[1], tangent[2], uoffs, voffs);
	pVertices[lodLevel][iStartVtx + 1] = SVertex(xoffs + quadWidth, baseHeight, zoffs, 0, 1.0f, 0, tangent[0], tangent[1], tangent[2], uoffs + fSegUSz * w, voffs);
	pVertices[lodLevel][iStartVtx + vtxRowShift] = SVertex(xoffs, baseHeight, zoffs + quadDepth, 0, 1.0f, 0, tangent[0], tangent[1], tangent[2], uoffs, voffs + fSegVSz * d);
	pVertices[lodLevel][iStartVtx + vtxRowShift + 1] = SVertex(xoffs + quadWidth, baseHeight, zoffs + quadDepth, 0, 1.0f, 0, tangent[0], tangent[1], tangent[2], uoffs + fSegUSz * w, voffs + fSegVSz * d);

	// Warning: Do not modify the order of index creation! Otherwise (at least) Normal Recreation gets invalid!
	pIndices[lodLevel][iStartIndex]	= (SIndex)(iStartVtx);
	pIndices[lodLevel][iStartIndex + 1] = (SIndex)(iStartVtx + vtxRowShift + 1);
	pIndices[lodLevel][iStartIndex + 2] = (SIndex)(iStartVtx + 1);
	pIndices[lodLevel][iStartIndex + 3] = (SIndex)(iStartVtx);
	pIndices[lodLevel][iStartIndex + 4] = (SIndex)(iStartVtx + vtxRowShift);
	pIndices[lodLevel][iStartIndex + 5] = (SIndex)(iStartVtx + vtxRowShift + 1);
}

// -------------------------------------------------------------------------------------------------------------------
void STerrainChunk::GenerateLodLevel(unsigned int lodLvl, unsigned int startSz /* start quad size */, float baseHeight)
{
	if (startSz % 2 > 0 && startSz > 1)
		return;

	unsigned int sz[2] = { startSz, startSz };
	unsigned int w = nSegsX, d = nSegsZ;

	STerrainChunkLodCounts& lodcounts = pLodCounts[lodLvl];
	unsigned int vtxRowShift = lodcounts.quadsX + 1;

	unsigned long iCurVtx = 0;
	unsigned long iCurIdx = 0;

	float segmentWidth = (fWidth / (float)w);
	float segmentDepth = (fDepth / (float)d);

	for (unsigned int z = 0; z < nSegsZ;)
	{
		// find next quad depth
		while ((int)d - (int)sz[1] < 0)
			sz[1] = (sz[1] > 1) ? (unsigned int)((double)sz[1] * 0.5) : 0;

		if (sz[1] == 0)
			break; 
		
		w = nSegsX;
		sz[0] = startSz;
		for (unsigned int x = 0; x < nSegsX;)
		{
			// find next quad width
			while ((int)w - (int)sz[0] < 0)
				sz[0] = (sz[0] > 1) ? (unsigned int)((double)sz[0] * 0.5) : 0;

			if (sz[0] == 0)
				break;
#ifdef _DEBUG
			//printf("seg: (%u, %u) nsegs: (%u, %u) - AddQuad(%u, %u, %u, %u, %u, %u, %f, ...)\n", x, z, nSegsX, nSegsZ, lodLvl, sz[0], sz[1], iCurVtx, iCurIdx, vtxRowShift, baseHeight);
#endif

			if (x == nSegsX - 1)
				int bp = 0;

			AddQuad(lodLvl, sz[0], sz[1], iCurVtx, iCurIdx, vtxRowShift, baseHeight,
				fXOffs + segmentWidth * x, fZOffs + segmentDepth * z,
				fUOffs + fSegUSz * x, fVOffs + fSegVSz * z);

			iCurVtx += (x == nSegsX - sz[0]) ? 2 : 1;
			iCurIdx += 6;

			w -= sz[0];
			x += sz[0];
		}

		d -= sz[1];
		z += sz[1];
	}
}







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


S_API void Terrain::Generate(const STerrainDescription& tdsc)
{
	nSegsX = tdsc.segsX;
	nSegsZ = tdsc.segsZ;
	fWidth = tdsc.w;
	fDepth = tdsc.d;
	nLodLevels = tdsc.lodLevels;

	nChunkSegsX = tdsc.chunkSegsX;
	nChunkSegsZ = tdsc.chunkSegsZ;

	// count chunks    
	nXRestChunkSegs = nSegsX % nChunkSegsX;
	nZRestChunkSegs = nSegsZ % nChunkSegsZ;
	nChunksX = (nSegsX - nXRestChunkSegs) / nChunkSegsX;
	nChunksZ = (nSegsZ - nZRestChunkSegs) / nChunkSegsZ;

	nRestChunksX = 0;
	nRestChunksZ = 0;
	if (nXRestChunkSegs > 0)
	{
		nChunksX++;
		nRestChunksX = 1;
	}
	if (nZRestChunkSegs > 0)
	{
		nChunksZ++;
		nRestChunksZ = 1;
	}

	// regular chunk counts
	unsigned int nRegChunksX = nChunksX - nRestChunksX;
	unsigned int nRegChunksZ = nChunksZ - nRestChunksZ;
	unsigned int nRegChunks = nRegChunksX * nRegChunksZ;

	// calculate segment sizes
	float segWidth = tdsc.w / nSegsX;
	float segDepth = tdsc.d / nSegsZ;
	float segUSz = tdsc.w / (nSegsX + 1) / tdsc.w;
	float segVSz = tdsc.d / (nSegsZ + 1) / tdsc.d;

#ifdef _DEBUG
	EngLog(S_DEBUG, m_pEngine, "Terrain: {nChunkSegsX=%u, nChunkSegsZ=%u, nSegsX=%u, nSegsZ=%u}", nChunkSegsX, nChunkSegsZ, nSegsX, nSegsZ);
#endif

	// generate chunks buffer
	pTerrainChunks = new STerrainChunk[nChunksX * nChunksZ];

	// Precalculate vertex and index count, create the buffers and create chunks
	pLodLevelCounts = new STerrainLodCounts[nLodLevels];
	
	pVertexBuffers = new SVertex*[nLodLevels];
	ZeroMemory(pVertexBuffers, nLodLevels * sizeof(SVertex*));
	
	pIndexBuffers = new SIndex*[nLodLevels];
	ZeroMemory(pVertexBuffers, nLodLevels * sizeof(SIndex*));

	for (unsigned int lodLvl = 0; lodLvl < nLodLevels; ++lodLvl)
	{
		STerrainChunkCounts regChunkCounts = STerrainChunk::PrecalcCounts(lodLvl, nChunkSegsX, nChunkSegsZ);
		STerrainChunkCounts restXChunkCnt = STerrainChunk::PrecalcCounts(lodLvl, nXRestChunkSegs, nChunkSegsZ);
		STerrainChunkCounts restZChunkCnt = STerrainChunk::PrecalcCounts(lodLvl, nChunkSegsX, nZRestChunkSegs);
		STerrainChunkCounts fillChunkCnt = STerrainChunk::PrecalcCounts(lodLvl, nXRestChunkSegs, nZRestChunkSegs);

		STerrainLodCounts &counts = pLodLevelCounts[lodLvl];		

		counts.vertices = regChunkCounts.vertices * nRegChunks;
		counts.indices = regChunkCounts.indices * nRegChunks;		

		if (nRestChunksX > 0 || nRestChunksZ > 0)
		{
			counts.vertices += nRegChunksZ * (nRestChunksX * restXChunkCnt.vertices);
			counts.vertices += nRegChunksX * (nRestChunksZ * restZChunkCnt.vertices);
			counts.vertices += nRestChunksX * nRestChunksZ * fillChunkCnt.vertices; // fill chunk

			counts.indices += nRegChunksZ * (nRestChunksX * restXChunkCnt.indices);
			counts.indices += nRegChunksX * (nRestChunksZ * restZChunkCnt.indices);
			counts.indices += nRestChunksX * nRestChunksZ * fillChunkCnt.vertices; // fill chunk			
		}

		pVertexBuffers[lodLvl] = new SVertex[counts.vertices];
		pIndexBuffers[lodLvl] = new SIndex[counts.indices];				



		// fill chunks
		unsigned long iCurVtxPos = 0, iCurIdxPos = 0;
		for (unsigned int cz = 0; cz < nChunksZ; ++cz)
		{
			bool bIsZRest = (cz == nChunksZ - 1) && (nRestChunksZ > 0);
			for (unsigned int cx = 0; cx < nChunksX; ++cx)
			{
				bool bIsXRest = (cx == nChunksX - 1) && (nRestChunksX > 0);

				STerrainChunk* pChunk = &pTerrainChunks[cz * nChunksX + cx];

				// Get counts for this chunk
				STerrainChunkCounts* pCurChunkCnt = &regChunkCounts;
				if (bIsZRest && bIsXRest)
					pCurChunkCnt = &fillChunkCnt;
				else if (bIsZRest)
					pCurChunkCnt = &restZChunkCnt;
				else if (bIsXRest)
					pCurChunkCnt = &restXChunkCnt;

				// Initialize chunk
				if (lodLvl == 0)
				{
					pChunk->pVertices = new SVertex*[nLodLevels];
					pChunk->pIndices = new SIndex*[nLodLevels];
					pChunk->pLodCounts = new STerrainChunkLodCounts[nLodLevels];
					pChunk->nLodLevels = nLodLevels;

					pChunk->nSegsX = (bIsXRest) ? nXRestChunkSegs : nChunkSegsX;
					pChunk->nSegsZ = (bIsZRest) ? nZRestChunkSegs : nChunkSegsZ;
					pChunk->fWidth = nChunkSegsX * segWidth;
					pChunk->fDepth = nChunkSegsZ * segDepth;
					pChunk->fSegUSz = segUSz;
					pChunk->fSegVSz = segVSz;
					pChunk->fXOffs = cx * (segWidth * nChunkSegsX);
					pChunk->fZOffs = cz * (segDepth * nChunkSegsZ);
					pChunk->fUOffs = cx * (segUSz * nChunkSegsX);
					pChunk->fVOffs = cz * (segVSz * nChunkSegsZ);
				}

				// Set pointer of the chunks vtx/idx to the first vtx/idx in the main buffer, so
				// when generating the lod level, only a portion of the main buffer is set.
				pChunk->pVertices[lodLvl] = &pVertexBuffers[lodLvl][iCurVtxPos];
				pChunk->pIndices[lodLvl] = &pIndexBuffers[lodLvl][iCurIdxPos];
				
				pChunk->pLodCounts[lodLvl].vertexOffset = iCurVtxPos;
				pChunk->pLodCounts[lodLvl].indexOffset = iCurIdxPos;
				pChunk->pLodCounts[lodLvl].vertexCount = pCurChunkCnt->vertices;
				pChunk->pLodCounts[lodLvl].indexCount = pCurChunkCnt->indices;
				pChunk->pLodCounts[lodLvl].quadsX = pCurChunkCnt->quadsX;
				pChunk->pLodCounts[lodLvl].quadsZ = pCurChunkCnt->quadsZ;

#ifdef _DEBUG
				/*
				printf("LOD%u: chnk(%u, %u): vtxOffs=%u, idxOffs=%u, vtxCnt=%u, idxCnt=%u\n", lodLvl, cx, cz,
					pChunk->pLodCounts[lodLvl].vertexOffset, pChunk->pLodCounts[lodLvl].indexOffset,
					pChunk->pLodCounts[lodLvl].vertexCount, pChunk->pLodCounts[lodLvl].indexCount);
				*/
#endif

				// Let the chunk generate its data itself
				pChunk->GenerateLodLevel(lodLvl, STerrainChunk::GetLodLevelQuadStartSz(lodLvl), tdsc.baseHeight);				

				// Update current vtx/idx pointers
				iCurVtxPos += pCurChunkCnt->vertices;
				iCurIdxPos += pCurChunkCnt->indices;
			}
		}
	}
	
	if (Failure(FillVertexAndIndexBuffers()))
		return;

	if (Failure(GenerateFlatVertexHeightmap(tdsc.baseHeight)))
		return;
	
	ClearTemporaryGenerationVertices();
}

// -------------------------------------------------------------------------------------------------------------------
S_API void Terrain::ClearTemporaryGenerationVertices()
{
	// Remove initialization vertex data buffers, as they are stored in the
	// hardware implementations shadow buffers. (Use pHWVertexBuffer->GetVertex())
	for (unsigned int lodLevel = 0; lodLevel < nLodLevels; ++lodLevel)
	{
		if (IS_VALID_PTR(pVertexBuffers[lodLevel]))
			delete[] pVertexBuffers[lodLevel];	

		pVertexBuffers[lodLevel] = 0;
	}
	delete[] pVertexBuffers;	
	pVertexBuffers = 0;	

	// Also reset the pointers to portions to the main buffer in all the chunks.
	for (unsigned int chunk = 0; chunk < nChunksX * nChunksZ; ++chunk)
	{
		STerrainChunk* pChunk = &pTerrainChunks[chunk];

		if (IS_VALID_PTR(pChunk->pVertices))
			delete[] pChunk->pVertices;

		pChunk->pVertices = 0;		
	}
}

// -------------------------------------------------------------------------------------------------------------------
S_API SResult Terrain::FillVertexAndIndexBuffers()
{
	if (!IS_VALID_PTR(m_pEngine) || nLodLevels == 0)
		return S_NOTINIT;

	IResourcePool* pResources = m_pEngine->GetResources();
	if (!IS_VALID_PTR(pResources))
		return S_NOTINIT;

	m_pHWVertexBuffers = new IVertexBuffer*[nLodLevels];
	m_pHWIndexBuffers = new IIndexBuffer*[nLodLevels];

	for (unsigned int lodLevel = 0; lodLevel < nLodLevels; ++lodLevel)
	{
		// Create and fill vertex buffer
		if (Failure(pResources->AddVertexBuffer(&m_pHWVertexBuffers[lodLevel])))
			return EngLog(S_ERROR, m_pEngine, "Failed add hardware vertex buffer #%u for terrain!", lodLevel);

		EVBUsage vbUsage = bDynamic ? eVBUSAGE_DYNAMIC_RARE : eVBUSAGE_STATIC;
		if (Failure(m_pHWVertexBuffers[lodLevel]->Initialize(m_pEngine, m_pEngine->GetRenderer(), vbUsage, 0, 0)))
			return EngLog(S_ERROR, m_pEngine, "Failed initialize hardware vertex buffer #%u for terrain!", lodLevel);

		if (Failure(m_pHWVertexBuffers[lodLevel]->Fill(pVertexBuffers[lodLevel], pLodLevelCounts[lodLevel].vertices)))
			return EngLog(S_ERROR, m_pEngine, "Failed fill hardware vertex buffer #%u for terrain!", lodLevel);


		//---

		// Create and fill index buffer
		if (Failure(pResources->AddIndexBuffer(&m_pHWIndexBuffers[lodLevel])))
			return EngLog(S_ERROR, m_pEngine, "Failed add hardware index buffer #%u for terrain!", lodLevel);
		
		if (Failure(m_pHWIndexBuffers[lodLevel]->Initialize(m_pEngine, m_pEngine->GetRenderer(), eIBUSAGE_STATIC, pLodLevelCounts[lodLevel].indices, 0)))
			return EngLog(S_ERROR, m_pEngine, "Failed initialize hardware index buffer #%u for terrain!", lodLevel);

		if (Failure(m_pHWIndexBuffers[lodLevel]->Fill(pIndexBuffers[lodLevel], pLodLevelCounts[lodLevel].indices, false)))
			return EngLog(S_ERROR, m_pEngine, "Failed fill hardware index buffer #%u for terrain!", lodLevel);
	}

	m_bRequireCBUpdate = true;

	return RecalculateNormals();	
}

// -------------------------------------------------------------------------------------------------------------------
S_API SResult Terrain::GenerateFlatVertexHeightmap(float baseHeight)
{
	if (!IS_VALID_PTR(m_pEngine))
		return S_NOTINIT;

	IResourcePool* pRes = m_pEngine->GetResources();	

	// Remove old texture, if there was one
	if (IS_VALID_PTR(m_pVtxHeightMap))
		pRes->RemoveTexture(&m_pVtxHeightMap);

	// Create new Texture
	float baseHeightScaled = baseHeight / m_MaxHeight;
	SColor baseColor(baseHeightScaled, baseHeightScaled, baseHeightScaled);	

	SResult res = pRes->AddTexture(nSegsX + 1, nSegsZ + 1, "terrain_vtxheightmap", eTEXTURE_R8G8B8A8_UNORM, baseColor, &m_pVtxHeightMap, true);
	if (Failure(res))
		return res;

	unsigned int rowPitch;
	unsigned long *pPixels;
	unsigned int nPixels;
	if (Success(m_pVtxHeightMap->Lock((void**)&pPixels, &nPixels, &rowPitch)))
	{
		rowPitch = (unsigned int)(rowPitch / 4);
		for (unsigned int x = 0; x < nSegsX + 1; ++x)
		{
			for (unsigned int y = 0; y < nSegsZ + 1; ++y)
			{
				// ff ff ff ff
				float val = sinf((float)x / 10.0f) * sinf((float)y / 10.0f);
				unsigned long byteval = (unsigned char)(((val + 1.0f) * 0.5f) * 255.0f);

				pPixels[x * rowPitch + y] = byteval;
			}
		}

		m_pVtxHeightMap->Unlock();
	}
	else
	{
		EngLog(S_ERROR, m_pEngine, "Failed lock vtx height map for terrain!");
	}	

	return S_SUCCESS;
}







// -------------------------------------------------------------------------------------------------------------------
S_API SResult Terrain::RecalculateNormals(unsigned int lodLevel /* = 0*/)
{
	if (!IS_VALID_PTR(m_pHWVertexBuffers) || !IS_VALID_PTR(m_pHWIndexBuffers) || !IS_VALID_PTR(pLodLevelCounts))
		return S_NOTINIT;

	IVertexBuffer* pVB = m_pHWVertexBuffers[lodLevel];		

	// calculate
	for (unsigned int cz = 0; cz < nChunksZ; ++cz)
	{
		for (unsigned int cx = 0; cx < nChunksX; ++cx)
		{		
			//      1
			//      |
			// 4----o----2
			//      |
			//      3

			// o-----o-----o
			// |   / |   / |
			// | /   | /   |
			// 4---1,5,10--7,11
			// |   / |   / |
			// | /   | /   |
			//0,3--2,6,9---8

			STerrainChunk& chunk = pTerrainChunks[cz * nChunksX + cx];
			SIndex* pChunkIndices = chunk.pIndices[lodLevel];
			STerrainChunkLodCounts& lodcounts = chunk.pLodCounts[lodLevel];
			for (unsigned int qx = 0; qx < lodcounts.quadsX; ++qx)
			{
				bool isLeftCol = (qx == 0);
				bool isRightCol = (qx == lodcounts.quadsX - 1);
				for (unsigned int qz = 0; qz < lodcounts.quadsZ; ++qz)
				{
					bool isBottomRow = (qz == 0);
					bool isTopRow = (qz == lodcounts.quadsZ - 1);

					unsigned long centerIdx = (unsigned long)(qz * lodcounts.quadsX + qx) * 6UL;
					unsigned long leftIdx = (unsigned long)(qz * lodcounts.quadsX + qx - 1) * 6UL;
					unsigned long downIdx = (unsigned long)((qz - 1) * lodcounts.quadsX + qx) * 6UL;
					unsigned long rightIdx = (unsigned long)(qz * lodcounts.quadsX + qx + 1) * 6UL;
					unsigned long upIdx = (unsigned long)((qz + 1) * lodcounts.quadsX + qx) * 6UL;
					SVertex* pCenterVert = pVB->GetVertex(pChunkIndices[centerIdx]);

					SVector3 dirAccum(0, 0, 0);					

					if (!isLeftCol && !isBottomRow) // left & down										
						dirAccum += SPNormalFromThreeVerts(*pCenterVert, *pVB->GetVertex(pChunkIndices[leftIdx]), *pVB->GetVertex(pChunkIndices[downIdx]));
					else
						dirAccum += SVector3(0, 1.0f, 0);

					if (!isBottomRow && !isRightCol) // down & right
						dirAccum += SPNormalFromThreeVerts(*pCenterVert, *pVB->GetVertex(pChunkIndices[downIdx]), *pVB->GetVertex(pChunkIndices[rightIdx]));
					else
						dirAccum += SVector3(0, 1.0f, 0);

					if (!isRightCol && !isTopRow) // right & top
						dirAccum += SPNormalFromThreeVerts(*pCenterVert, *pVB->GetVertex(pChunkIndices[rightIdx]), *pVB->GetVertex(pChunkIndices[upIdx]));
					else
						dirAccum += SVector3(0, 1.0f, 0);

					if (!isTopRow && !isLeftCol) // top & left
						dirAccum += SPNormalFromThreeVerts(*pCenterVert, *pVB->GetVertex(pChunkIndices[upIdx]), *pVB->GetVertex(pChunkIndices[leftIdx]));
					else
						dirAccum += SVector3(0, 1.0f, 0);

					dirAccum = SVector3Normalize(dirAccum);
					pCenterVert->nx = dirAccum.x;
					pCenterVert->ny = dirAccum.y;
					pCenterVert->nz = dirAccum.z;
				}
			}
		}				
	}

	return S_SUCCESS;
}

// -------------------------------------------------------------------------------------------------------------------
S_API SResult Terrain::RenderTerrain(const SVector3& camPos)
{
	SP_ASSERTR(IS_VALID_PTR(m_pEngine), S_NOTINIT);

	if (!IS_VALID_PTR(m_pHWVertexBuffers) || !IS_VALID_PTR(m_pHWIndexBuffers))
		return m_pEngine->LogE("Failed RenderTerrain: Terrain geometry not initialized!");

	if (!m_bRequireRender)
		return S_SUCCESS;

	m_bRequireRender = false;

	IRenderer* pRenderer = m_pEngine->GetRenderer();

	

	STerrainRenderDesc* dsc = pRenderer->GetTerrainRenderDesc();
	dsc->pColorMap = m_pColorMap;
	dsc->pDetailMap = m_pDetailMap;
	dsc->pVtxHeightMap = m_pVtxHeightMap;
	dsc->bRender = true;
	SMatrixIdentity(&dsc->transform.scale);

	dsc->bUpdateCB = false;

	if (m_bRequireCBUpdate)
	{
		dsc->bUpdateCB = true;
		dsc->constants.dmTexRatioU = m_fDMTexScaleU;
		dsc->constants.dmTexRatioV = m_fDMTexScaleV;
		dsc->constants.fTerrainDMFadeRadius = m_pEngine->GetSettings()->Get().render.fTerrainDMFadeRange;
		dsc->constants.fTerrainMaxHeight = m_MaxHeight;
		dsc->constants.vtxHeightMapSz[0] = nSegsX + 1;
		dsc->constants.vtxHeightMapSz[1] = nSegsZ + 1;
		dsc->constants.segmentSize[0] = this->fWidth / nSegsX;
		dsc->constants.segmentSize[1] = this->fDepth / nSegsZ;
		m_bRequireCBUpdate = false;
	}
	


	if (IS_VALID_PTR(dsc->pDrawCallDescs) && dsc->nDrawCallDescs < nChunksZ * nChunksX)
	{
		delete[] dsc->pDrawCallDescs;
		dsc->pDrawCallDescs = 0;
	}

	if (!IS_VALID_PTR(dsc->pDrawCallDescs))
	{
		dsc->pDrawCallDescs = new SDrawCallDesc[nChunksX * nChunksZ];
		dsc->nDrawCallDescs = nChunksX * nChunksZ;
	}

	for (unsigned int cz = 0; cz < nChunksZ; ++cz)
	{
		for (unsigned int cx = 0; cx < nChunksX; ++cx)
		{
			SDrawCallDesc* dcd = &dsc->pDrawCallDescs[cz * nChunksX + cx];
			STerrainChunk* pChunk = &pTerrainChunks[cz * nChunksX + cx];

			// get current lod level using distance to chunk center			
			SVector3 chunkCenter(pChunk->fXOffs + 0.5f * pChunk->fWidth, camPos.y, pChunk->fZOffs + 0.5f * pChunk->fDepth);
			float camChunkDist = (camPos - chunkCenter).Length();

			unsigned int lodLvl = (unsigned int)floorf(camChunkDist / 10.0f);
			if (lodLvl >= nLodLevels)
				lodLvl = nLodLevels - 1;

			//lodLvl = 1;

			// get the vertex and index buffer based on that lodLvl
			dcd->pVertexBuffer = m_pHWVertexBuffers[lodLvl];
			dcd->pIndexBuffer = m_pHWIndexBuffers[lodLvl];

			if (!IS_VALID_PTR(dcd->pVertexBuffer) || !IS_VALID_PTR(dcd->pIndexBuffer))
			{
				EngLog(S_ERROR, m_pEngine, "VB or IB of lod lvl #%u not inited!", lodLvl);
				continue;
			}
			
			const STerrainChunkLodCounts& chunkCnts = pChunk->pLodCounts[lodLvl];
			if (m_bUseVBCulledRendering)
			{
				dcd->iStartVBIndex = chunkCnts.vertexOffset;
				dcd->iEndVBIndex = chunkCnts.vertexOffset + chunkCnts.vertexCount;
			}
			else
			{
				dcd->iStartVBIndex = 0;
				dcd->iEndVBIndex = dcd->pVertexBuffer->GetVertexCount();
			}
			dcd->iStartIBIndex = chunkCnts.indexOffset;
			dcd->iEndIBIndex = chunkCnts.indexOffset + chunkCnts.indexCount - 1;

			dcd->primitiveType = PRIMITIVE_TYPE_TRIANGLELIST;
		}
	}

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
S_API IVertexBuffer* Terrain::GetVertexBuffer(unsigned int lodLevel)
{
	if (!IS_VALID_PTR(m_pHWVertexBuffers))
		return 0;

	return m_pHWVertexBuffers[lodLevel];
}

// -------------------------------------------------------------------------------------------------------------------
S_API IIndexBuffer* Terrain::GetIndexBuffer(unsigned int lodLevel)
{
	if (!IS_VALID_PTR(m_pHWIndexBuffers))
		return 0;

	return m_pHWIndexBuffers[lodLevel];
}

// -------------------------------------------------------------------------------------------------------------------
S_API SResult Terrain::Clear(void)
{	
	if (IS_VALID_PTR(m_pEngine))
	{
		IResourcePool* pResourcePool = m_pEngine->GetResources();
		if (IS_VALID_PTR(pResourcePool))
		{
			for (unsigned int lod = 0; lod < nLodLevels; ++lod)
			{
				if (IS_VALID_PTR(m_pHWVertexBuffers))
					pResourcePool->RemoveVertexBuffer(&m_pHWVertexBuffers[lod]);

				if (IS_VALID_PTR(m_pHWIndexBuffers))
					pResourcePool->RemoveIndexBuffer(&m_pHWIndexBuffers[lod]);
			}			
		}

		if (IS_VALID_PTR(m_pHWVertexBuffers))
			delete[] m_pHWVertexBuffers;
		if (IS_VALID_PTR(m_pHWIndexBuffers))
			delete[] m_pHWIndexBuffers;

		m_pEngine = 0;
	}

	m_pHWVertexBuffers = 0;
	m_pHWIndexBuffers = 0;



	// -------

	
	for (unsigned int lodLevel = 0; lodLevel < nLodLevels; ++lodLevel)
	{		
		if (IS_VALID_PTR(pVertexBuffers) && IS_VALID_PTR(pVertexBuffers[lodLevel]))
			delete[] pVertexBuffers[lodLevel];

		if (IS_VALID_PTR(pIndexBuffers) && IS_VALID_PTR(pIndexBuffers[lodLevel]))
			delete[] pIndexBuffers[lodLevel];
	}

	SP_SAFE_DELETE_ARR(pVertexBuffers, nLodLevels);
	SP_SAFE_DELETE_ARR(pIndexBuffers, nLodLevels);	

	// Clear terrain chunks
	if (IS_VALID_PTR(pTerrainChunks))
	{
		for (unsigned int chunk = 0; chunk < nChunksX * nChunksZ; ++chunk)
		{
			STerrainChunk* pChunk = &pTerrainChunks[chunk];

			SP_SAFE_DELETE_ARR(pChunk->pVertices, pChunk->nLodLevels);
			SP_SAFE_DELETE_ARR(pChunk->pIndices, pChunk->nLodLevels);
		}

		delete[] pTerrainChunks;		
	}
	pTerrainChunks = 0;



	// --------	

	SP_SAFE_DELETE_ARR(pLodLevelCounts, nLodLevels);

	nLodLevels = 0;

	m_pColorMap = 0;
	m_pDetailMap = 0;

	return S_SUCCESS;
}


SP_NMSPACE_END