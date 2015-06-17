// SpeedPoint Basic Terrain

#include <Implementation\Geometry\Terrain.h>
#include <Abstract\IGameEngine.h>
#include <Abstract\IRenderer.h>
#include <Abstract\IVertexBuffer.h>
#include <Abstract\IIndexBuffer.h>
#include <Abstract\IResourcePool.h>

SP_NMSPACE_BEG

static ITerrain::LodLevel* g_pLodLevel = 0;

///////////////////////////////////////////////////////////////////////////////////////////////                                 
S_API void STerrainChunk::CreateCoreBasedOnItsLODLevel(SLargeIndex* pIB, unsigned long startVtxIdx, unsigned long vtxRowShift, unsigned long& idxAccum)
{
	/*
	printf("TerChnk: CreateCoreBasedOnItsLODLevel(pIB=0x%p, startVtxIdx=%u, vtxRowShift=%u, idxAccum=%u\n",
		pIB, startVtxIdx, vtxRowShift, idxAccum);
	*/

	for (unsigned int z = 1; z <= coreQuads; ++z)
	{
		for (unsigned int x = 1; x <= coreQuads; ++x)
		{
			AddCoreQuad(pIB, startVtxIdx + z * (chunkQuads + 1) + x, vtxRowShift, idxAccum);
		}
	}
}

///////////////////////////////////////////////////////////////////////////////////////////////
S_API void STerrainChunk::AddCoreQuad(SLargeIndex* pIB, unsigned long startVtxIdx, unsigned long vtxRowShift, unsigned long& idxAccum)
{
	pIB[idxAccum] = startVtxIdx;
	pIB[idxAccum + 1] = startVtxIdx + vtxRowShift + 1;
	pIB[idxAccum + 2] = startVtxIdx + 1;
	pIB[idxAccum + 3] = startVtxIdx;
	pIB[idxAccum + 4] = startVtxIdx + vtxRowShift;
	pIB[idxAccum + 5] = startVtxIdx + vtxRowShift + 1;
	idxAccum += 6;
}

///////////////////////////////////////////////////////////////////////////////////////////////
// startVtxIdx: The index of the start index of the chunk
S_API void STerrainChunk::CreateBorder(SLargeIndex* pIB, ESide side, unsigned long startVtxIdx, const STerrainChunk* neighbor, unsigned long vtxRowShift, unsigned long& idxAccum)
{	
	//printf("Ter: CreateBorder(side=%u, startVtxIdx=%u, vtxRowShift=%u, idxAccum=%u\n",
	//	side, startVtxIdx, vtxRowShift, idxAccum);

	unsigned int borderQuadVtxShift = 0;
	if (IS_VALID_PTR(neighbor))
		borderQuadVtxShift = neighbor->quadSegs / quadSegs;
	else
		borderQuadVtxShift = 1;

	if (side == eSIDE_RIGHT || side == eSIDE_LEFT)
		borderQuadVtxShift *= vtxRowShift;

	if (!IS_VALID_PTR(neighbor) || neighbor->curLodLevel <= curLodLevel)
	{
		unsigned long borderStartIndex;

		borderQuadVtxShift = 1;
		if (side == eSIDE_RIGHT || side == eSIDE_LEFT)
			borderQuadVtxShift *= vtxRowShift;

		/*
		if (IS_VALID_PTR(neighbor) && neighbor->curLodLevel < curLodLevel)
		{
			borderQuadVtxShift = neighbor->quadSegs / quadSegs;
			if (side == eSIDE_RIGHT || side == eSIDE_LEFT)
				borderQuadVtxShift *= vtxRowShift;
		}
		*/

		switch (side)

		{
		case eSIDE_TOP: borderStartIndex = 1 + (chunkQuads - 1) * vtxRowShift; break;
		case eSIDE_RIGHT: borderStartIndex = (chunkQuads - 1) + vtxRowShift; break;
		case eSIDE_BOTTOM: borderStartIndex = 1; break;
		case eSIDE_LEFT: borderStartIndex = 1 * vtxRowShift; break;
		}

		// Same size -> simply create the quadsize quads		
		for (unsigned int i = 0; i < coreQuads; ++i)
		{
			pIB[idxAccum] = startVtxIdx + borderStartIndex + i * borderQuadVtxShift;
			pIB[idxAccum + 1] = startVtxIdx + borderStartIndex + i * borderQuadVtxShift + vtxRowShift + 1;
			pIB[idxAccum + 2] = startVtxIdx + borderStartIndex + i * borderQuadVtxShift + 1;
			pIB[idxAccum + 3] = startVtxIdx + borderStartIndex + i * borderQuadVtxShift;
			pIB[idxAccum + 4] = startVtxIdx + borderStartIndex + i * borderQuadVtxShift + vtxRowShift;
			pIB[idxAccum + 5] = startVtxIdx + borderStartIndex + i * borderQuadVtxShift + vtxRowShift + 1;
			idxAccum += 6;
		}
	}
	else if (neighbor->curLodLevel > curLodLevel)
	{		
		// bq = big quad, sq = small quad
		unsigned int nQuadsPerBQ = neighbor->quadSegs / quadSegs;
		unsigned int bqHalf = nQuadsPerBQ / 2;

		unsigned int borderStartIndex;		
		switch (side)
		{
		case eSIDE_TOP: borderStartIndex = nQuadsPerBQ + (chunkQuads - 1) * vtxRowShift; break;
		case eSIDE_RIGHT: borderStartIndex = (chunkQuads - 1) + nQuadsPerBQ * vtxRowShift; break;
		case eSIDE_BOTTOM: borderStartIndex = vtxRowShift + nQuadsPerBQ; break;
		case eSIDE_LEFT: borderStartIndex = nQuadsPerBQ * vtxRowShift + 1; break;
		}		
		
		for (unsigned int bq = 0; bq < neighbor->coreQuads; ++bq)
		{			
			unsigned int bqOffs = borderStartIndex + bq * borderQuadVtxShift;
			
			unsigned int bindVtx1; // vtx offset to the next bq vtx1            
			switch (side)
			{
			case eSIDE_TOP: bindVtx1 = bqOffs + vtxRowShift; break;
			case eSIDE_BOTTOM: bindVtx1 = bqOffs - vtxRowShift; break;
			case eSIDE_RIGHT: bindVtx1 = bqOffs + 1; break;
			case eSIDE_LEFT: bindVtx1 = bqOffs - 1; break;
			}

			// vtx idx of the second bind vtx                                                
			unsigned int bindVtx2;
			if (side == eSIDE_TOP || side == eSIDE_BOTTOM)
				bindVtx2 = bindVtx1 + nQuadsPerBQ;
			else
				bindVtx2 = bindVtx1 + nQuadsPerBQ * vtxRowShift;

			// go through all small quads per bq
			for (unsigned int sq = 0; sq < nQuadsPerBQ; ++sq)
			{
				// B1---------B2
				// | \       / |  
				// |   \   /   |
				// o-----o-----o

				// Chose one of the bind vertices which is of the above ones (B1 / B2)                        
				unsigned int bindVtx = (sq >= bqHalf) ? bindVtx2 : bindVtx1;

				unsigned int sqOffs = (side == eSIDE_TOP || side == eSIDE_BOTTOM) ? (bqOffs + sq) : (bqOffs + sq * vtxRowShift);
				pIB[idxAccum] = startVtxIdx + sqOffs;				
				if (side == eSIDE_TOP || side == eSIDE_LEFT)
				{
					pIB[idxAccum + 1] = startVtxIdx + bindVtx;
					pIB[idxAccum + 2] = startVtxIdx + sqOffs + ((side == eSIDE_TOP || side == eSIDE_BOTTOM) ? 1 : vtxRowShift);
				}
				else
				{
					pIB[idxAccum + 1] = startVtxIdx + sqOffs + ((side == eSIDE_TOP || side == eSIDE_BOTTOM) ? 1 : vtxRowShift);
					pIB[idxAccum + 2] = startVtxIdx + bindVtx;					
				}

				idxAccum += 3;
			}

			// add the center triangle
			if (side == eSIDE_TOP || side == eSIDE_LEFT)
			{
				pIB[idxAccum] = startVtxIdx + bqOffs + ((side == eSIDE_TOP || side == eSIDE_BOTTOM) ? bqHalf : (bqHalf * vtxRowShift));
				pIB[idxAccum + 1] = startVtxIdx + bindVtx1;
			}
			else
			{
				pIB[idxAccum] = startVtxIdx + bindVtx1;
				pIB[idxAccum + 1] = startVtxIdx + bqOffs + ((side == eSIDE_TOP || side == eSIDE_BOTTOM) ? bqHalf : (bqHalf * vtxRowShift));
			}			
			pIB[idxAccum + 2] = startVtxIdx + bindVtx2;
			idxAccum += 3;
		}
	}
}

///////////////////////////////////////////////////////////////////////////////////////////////
// Border must have coherent sides (clockwise). I.e. side1 = LEFT ==> side2 = TOP
S_API void STerrainChunk::CreateCorner(const STerrainChunkCorner& corner, SLargeIndex* pIB, unsigned long startVtxIdx, unsigned long vtxRowShift, unsigned long& idxAccum)
{
	unsigned int cornerVtx, neighborVtx1, neighborVtx2;
	unsigned int intVtx; // intersection vertex / corner of chunk
	long quadShift1, quadShift2;
	unsigned int nbqquads1 = (IS_VALID_PTR(corner.neighbor1) ? (max(corner.neighbor1->quadSegs, quadSegs) / quadSegs) : 1);
	unsigned int nbqquads2 = (IS_VALID_PTR(corner.neighbor2) ? (max(corner.neighbor2->quadSegs, quadSegs) / quadSegs) : 1);
	switch (corner.side1)
	{
	case eSIDE_LEFT:
		cornerVtx = 1 + (chunkQuads - 1) * vtxRowShift;
		neighborVtx1 = ((IS_VALID_PTR(corner.neighbor1)) ? ((chunkQuads - nbqquads1) * vtxRowShift) : ((chunkQuads - 1) * vtxRowShift));
		neighborVtx2 = ((IS_VALID_PTR(corner.neighbor2)) ? (nbqquads2 + chunkQuads * vtxRowShift) : (1 + chunkQuads * vtxRowShift));
		intVtx = chunkQuads * vtxRowShift;
		quadShift1 = -(long)(vtxRowShift);
		quadShift2 = (long)1;
		break;
	case eSIDE_TOP:
		cornerVtx = (chunkQuads - 1) + (chunkQuads - 1) * vtxRowShift;
		neighborVtx1 = ((IS_VALID_PTR(corner.neighbor1)) ? ((chunkQuads - nbqquads1) + chunkQuads * vtxRowShift) : ((chunkQuads - 1) + chunkQuads * vtxRowShift));
		neighborVtx2 = ((IS_VALID_PTR(corner.neighbor2)) ? (chunkQuads + (chunkQuads - nbqquads2) * vtxRowShift) : (chunkQuads + (chunkQuads - 1) * vtxRowShift));
		intVtx = chunkQuads * vtxRowShift + chunkQuads;
		quadShift1 = -(long)1;
		quadShift2 = -(long)(vtxRowShift);
		break;
	case eSIDE_RIGHT:
		cornerVtx = (chunkQuads - 1) + vtxRowShift;
		neighborVtx1 = ((IS_VALID_PTR(corner.neighbor1)) ? (chunkQuads + nbqquads1 * vtxRowShift) : (chunkQuads + vtxRowShift));
		neighborVtx2 = ((IS_VALID_PTR(corner.neighbor2)) ? (chunkQuads - nbqquads2) : (chunkQuads - 1));
		intVtx = chunkQuads;
		quadShift1 = (long)(vtxRowShift);
		quadShift2 = -(long)1;
		break;
	case eSIDE_BOTTOM:
		cornerVtx = 1 + vtxRowShift;
		neighborVtx1 = ((IS_VALID_PTR(corner.neighbor1)) ? nbqquads1 : 1);
		neighborVtx2 = ((IS_VALID_PTR(corner.neighbor2)) ? (nbqquads2 * vtxRowShift) : vtxRowShift);
		intVtx = 0;
		quadShift1 = (long)1;
		quadShift2 = (long)(vtxRowShift);
		break;
	}	

	// o-------o
	// | \   / |
	// |   o---o
	// | / | \ |
	// o---o---o

	// create triangles for neighbor1
	if (IS_VALID_PTR(corner.neighbor1))
	{
		unsigned int quads1 = (corner.neighbor1->quadSegs / quadSegs);
		if (quads1 > 0)
			quads1--;		

		for (unsigned int i = 0; i < quads1; ++i)
		{
			pIB[idxAccum] = startVtxIdx + neighborVtx1;
			pIB[idxAccum + 1] = (unsigned long)((long)startVtxIdx + (long)cornerVtx + (long)i * quadShift1);
			pIB[idxAccum + 2] = (unsigned long)((long)pIB[idxAccum + 1] + quadShift1);
			idxAccum += 3;
		}
	}

	pIB[idxAccum] = startVtxIdx + neighborVtx1;
	pIB[idxAccum + 1] = startVtxIdx + intVtx;
	pIB[idxAccum + 2] = startVtxIdx + cornerVtx;
	idxAccum += 3;	

	// create triangles for neighbor2
	if (IS_VALID_PTR(corner.neighbor2))
	{
		unsigned int quads2 = (corner.neighbor2->quadSegs / quadSegs);
		if (quads2 > 0)
			quads2--;

		for (unsigned int i = 0; i < quads2; ++i)
		{
			pIB[idxAccum] = startVtxIdx + (unsigned long)((long)cornerVtx + (long)i * quadShift2);
			pIB[idxAccum + 1] = startVtxIdx + neighborVtx2;
			pIB[idxAccum + 2] = (unsigned long)((long)pIB[idxAccum] + quadShift2);
			idxAccum += 3;
		}
	}

	pIB[idxAccum] = startVtxIdx + neighborVtx2;
	pIB[idxAccum + 1] = startVtxIdx + cornerVtx;
	pIB[idxAccum + 2] = startVtxIdx + intVtx;
	idxAccum += 3;
}







///////////////////////////////////////////////////////////////////////////////////////////////
S_API Terrain::~Terrain()
{
	Clear();
}

///////////////////////////////////////////////////////////////////////////////////////////////
S_API SResult Terrain::Init(IGameEngine* pEngine, unsigned int segments, unsigned int chunkSegments, float size, float baseHeight, float fChunkStepDist, unsigned int nLodLevels)
{
	// Check given sizes		
	if (!IsPowerOfTwo(segments) || (segments % chunkSegments) > 0)
		return S_INVALIDPARAM;


	m_pEngine = pEngine;

	m_chunkSegs = chunkSegments;
	m_nSegments = segments;

	m_fChunkStepDist = fChunkStepDist;

	m_fSize = size;
	m_fSegSz = size / (float)segments;
	m_fTexSz = 1.0f / (float)segments;

	m_nLodLevels = nLodLevels;
	m_pLodLevels = new ITerrain::LodLevel[m_nLodLevels];
	printf("Ter: Allocated LodLevels buffer with %u elements\n", m_nLodLevels);

	// Create chunk vertex array for each lod level
	for (unsigned int iLodLvl = 0; iLodLvl < m_nLodLevels; ++iLodLvl)
	{
		ITerrain::LodLevel& lodLvl = m_pLodLevels[iLodLvl];
		lodLvl.quadSegs = max(PowerOfTwo(iLodLvl), 1);
		lodLvl.chunkQuads = m_chunkSegs / lodLvl.quadSegs;
		lodLvl.nChunkVertices = pow2(lodLvl.chunkQuads + 1);
		lodLvl.pChunkVertices = new SVertex[lodLvl.nChunkVertices];		
		float fQuadSz = lodLvl.quadSegs * m_fSegSz;

		/*
		printf("Ter: Inited lodLevel #%u: quadSegs=%u, chunkQuads=%u, nChunkVerts=%u\n",
			lodLvl.quadSegs, lodLvl.chunkQuads, lodLvl.nChunkVertices);
		*/

		for (unsigned int z = 0; z <= lodLvl.chunkQuads; ++z)
		{
			for (unsigned int x = 0; x <= lodLvl.chunkQuads; ++x)
			{
				lodLvl.pChunkVertices[z * (lodLvl.chunkQuads + 1) + x] =
					SVertex(x * fQuadSz, baseHeight, z * fQuadSz, 0, 1.0f, 0, 1.0f, 0, 0,
						x * lodLvl.quadSegs * m_fTexSz, z * lodLvl.quadSegs * m_fTexSz);
			}
		}
	}	

	// To initialize, generate a flat heightmap
	GenerateFlatVertexHeightmap(baseHeight);

	// Initialize chunk array
	unsigned long nChunks = pow2(m_nSegments / m_chunkSegs);
	m_pChunks = new STerrainChunk[nChunks];

	return S_SUCCESS;
}



///////////////////////////////////////////////////////////////////////////////////////////////
S_API void Terrain::GenLodLevelChunks(SCamera* pCamera)
{
	if (!IS_VALID_PTR(m_pEngine))
		return;

	/*
	printf("Terrain::GenLodLevelChunks(pCamera=%p)\n", pCamera);
	*/

	IResourcePool* pResources = m_pEngine->GetResources();
	if (!IS_VALID_PTR(pResources))
		return;

	// Clear the chunk list of each lod level
	for (unsigned int iLodLvl = 0; iLodLvl < m_nLodLevels; ++iLodLvl)
		m_pLodLevels[iLodLvl].chunks.clear();

	/*
	printf("Terrain: Cleared chunk vectors of %u lodLevels\n", m_nLodLevels);
	*/

	// Determine LOD Level for each chunk and add to the according lod Level
	unsigned long nChunks = m_nSegments / m_chunkSegs;
	/*
	printf("Ter: nChunks per side: %u, which makes %u overall chunks\n", nChunks, nChunks * nChunks);
	*/
	for (unsigned int cz = 0; cz < nChunks; ++cz)
	{
		for (unsigned int cx = 0; cx < nChunks; ++cx)
		{
			STerrainChunk* pChunk = &m_pChunks[cz * nChunks + cx];						
			pChunk->cx = cx;
			pChunk->cz = cz;
			pChunk->fSize = m_fSegSz * m_chunkSegs;
			pChunk->curLodLevel = pChunk->DetermineLODLevelByView(pCamera, m_nLodLevels, m_fChunkStepDist);
			pChunk->quadSegs = max(PowerOfTwo(pChunk->curLodLevel), 1);
			pChunk->chunkSegments = m_chunkSegs;			
			
			float quadSize = (pChunk->fSize / pChunk->chunkSegments) * pChunk->quadSegs;
			pChunk->chunkQuads = (pChunk->chunkSegments / pChunk->quadSegs);
			float coreOffset = quadSize;
			pChunk->coreQuads = (pChunk->chunkQuads < 2U) ? 0 : (pChunk->chunkQuads - 2U);			

			m_pLodLevels[pChunk->curLodLevel].chunks.push_back(pChunk);

			/*
			printf("Ter: Set Chunk[%u] cx=%u, cz=%u, fSz=%.2f, curLodLvl=%u, quadSegs=%u, chunkSegs=%u\n",
				cz * nChunks + cx, cx, cz, pChunk->fSize, pChunk->curLodLevel, pChunk->quadSegs, pChunk->chunkSegments);
			*/
		}
	}

	for (unsigned int iLodLvl = 0; iLodLvl < m_nLodLevels; ++iLodLvl)
	{
		ITerrain::LodLevel& lodLvl = m_pLodLevels[iLodLvl];		

		/*
		printf("Ter: Lod #%u. chunkQuads=%u, chunks.size()=%u, nChunkVerts=%u, pChunkVerts=%p, quadSegs=%u\n",
			iLodLvl, lodLvl.chunkQuads, lodLvl.chunks.size(), lodLvl.nChunkVertices, lodLvl.pChunkVertices, lodLvl.quadSegs);
		*/

		// Clear previous vertices and indices
		SP_SAFE_DELETE_ARR(lodLvl.pVertices, 1);
		SP_SAFE_DELETE_ARR(lodLvl.pIndices, 1);

		// Release old hardware vb and ib
		if (IS_VALID_PTR(lodLvl.pVB)) pResources->RemoveVertexBuffer(&lodLvl.pVB);
		if (IS_VALID_PTR(lodLvl.pIB)) pResources->RemoveIndexBuffer(&lodLvl.pIB);

		// Create the merged buffers for the lod level.
		// The IB is created with maxmimum size. It may happen, that payload is smaller than IB size.
		// To prevent false rendering, a maximum-index-accumulator is used.
		unsigned long lodLevelQuads = (unsigned long)lodLvl.chunks.size() * pow2(lodLvl.chunkQuads);		
		lodLvl.nIndices = lodLevelQuads * 6;		
		lodLvl.nVertices = lodLvl.chunks.size() * lodLvl.nChunkVertices;

		if (lodLvl.nIndices > 0 && lodLvl.nVertices > 0)
		{
			lodLvl.pIndices = new SLargeIndex[lodLvl.nIndices];						
			lodLvl.pVertices = new SVertex[lodLvl.nVertices];			
		}
		else
		{
			lodLvl.pIndices = 0;
			lodLvl.pVertices = 0;
			continue;
		}

		/*
		printf("Ter: Allocated buffers. nIndices=%u, nVerts=%u, pIndices=%p, pVerts=%p\n",
			lodLvl.nIndices, lodLvl.nVertices, lodLvl.pIndices, lodLvl.pVertices);
		*/

		unsigned long idxAccum = 0;
		unsigned long iChunk = 0;
		unsigned long lodLvlVtxRowShift = lodLvl.chunkQuads + 1;		
		for (auto itChunk = lodLvl.chunks.begin(); itChunk != lodLvl.chunks.end(); itChunk++)
		{			
			unsigned long chunkVtxOffset = iChunk * lodLvl.nChunkVertices;
			STerrainChunk* pChunk = *itChunk;			

			// Create the core
			pChunk->CreateCoreBasedOnItsLODLevel(lodLvl.pIndices, chunkVtxOffset, lodLvlVtxRowShift, idxAccum);

			// Determine neighbors
			// 0 - top, 1 - right, 2 - bottom, 3 - left
			STerrainChunk* chunkNeighbors[4];			
			chunkNeighbors[0] = (pChunk->cz >= nChunks - 1) ? 0 : &m_pChunks[(pChunk->cz + 1) * nChunks + pChunk->cx];
			chunkNeighbors[1] = (pChunk->cx >= nChunks - 1) ? 0 : &m_pChunks[pChunk->cz * nChunks + pChunk->cx + 1];
			chunkNeighbors[2] = (pChunk->cz == 0) ? 0 : &m_pChunks[(pChunk->cz - 1) * nChunks + pChunk->cx];
			chunkNeighbors[3] = (pChunk->cx == 0) ? 0 : &m_pChunks[pChunk->cz * nChunks + pChunk->cx - 1];									

			// Determine borders
			STerrainChunkBorder borders[] =
			{
				STerrainChunkBorder(eSIDE_TOP, chunkNeighbors[0]),
				STerrainChunkBorder(eSIDE_RIGHT, chunkNeighbors[1]),
				STerrainChunkBorder(eSIDE_BOTTOM, chunkNeighbors[2]),
				STerrainChunkBorder(eSIDE_LEFT, chunkNeighbors[3])
			};					

			for (unsigned int iBorder = 0; iBorder < 4; ++iBorder)
			{
				STerrainChunkBorder& border = borders[iBorder];				
				unsigned long oldIdxAccum = idxAccum;
				pChunk->CreateBorder(lodLvl.pIndices, border.side, chunkVtxOffset, border.neighbor, lodLvlVtxRowShift, idxAccum);				

				unsigned int iNextBorder = (iBorder + 1) % 4;

				STerrainChunkCorner corner;
				corner.neighbor1 = borders[iBorder].neighbor;
				corner.neighbor2 = borders[iNextBorder].neighbor;
				corner.side1 = border.side;			
				g_pLodLevel = &m_pLodLevels[iLodLvl];
				
				oldIdxAccum = idxAccum;
				pChunk->CreateCorner(corner, lodLvl.pIndices, chunkVtxOffset, lodLvlVtxRowShift, idxAccum);
			}			

			// Add vertices
			for (unsigned long iVtx = 0; iVtx < lodLvl.nChunkVertices; ++iVtx)
			{
				SVertex* pVtx = &lodLvl.pVertices[chunkVtxOffset + iVtx];				
				memcpy(pVtx, &lodLvl.pChunkVertices[iVtx], sizeof(SVertex));
				pVtx->position.x += pChunk->cx * pChunk->fSize;
				pVtx->position.z += pChunk->cz * pChunk->fSize;
				pVtx->textureCoords[0].u += (float)(pChunk->cx * m_chunkSegs) * m_fTexSz;
				pVtx->textureCoords[0].v += (float)(pChunk->cz * m_chunkSegs) * m_fTexSz;
			}			
			
			iChunk++;
		}		

		// Now create lodLevel's Hardware VB and Hardware IB
		pResources->AddVertexBuffer(&lodLvl.pVB);
		lodLvl.pVB->Initialize(m_pEngine, m_pEngine->GetRenderer(), eVBUSAGE_STATIC, lodLvl.pVertices, lodLvl.nVertices);

		pResources->AddIndexBuffer(&lodLvl.pIB);		
		lodLvl.nActualIndices = idxAccum;
		lodLvl.pIB->Initialize(m_pEngine, m_pEngine->GetRenderer(), eIBUSAGE_STATIC, lodLvl.nActualIndices, S_INDEXBUFFER_32, lodLvl.pIndices);			
	}
}



///////////////////////////////////////////////////////////////////////////////////////////////
S_API void Terrain::SetHeightmap(ITexture* heightmap)
{
	if (!IS_VALID_PTR(heightmap))
	{
		CLog::Log(S_ERROR, "Failed set terrain heightmap to 0x%p (invalid)", (void*)heightmap);
		return;
	}

	if (!heightmap->IsStaged())
	{
		CLog::Log(S_ERROR, "Cannot set terrain heightmap texture that is not staged!");
		return;
	}

	if (!m_bCustomHeightmapSet && IS_VALID_PTR(m_pVtxHeightMap) && IS_VALID_PTR(m_pEngine))
	{
		IResourcePool* pResources = m_pEngine->GetResources();
		pResources->RemoveTexture(&m_pVtxHeightMap);
	}

	m_pVtxHeightMap = heightmap;
	m_bCustomHeightmapSet = true;			

	CalculateMinMaxHeights();

	//UpdateCollisionMesh();
}


///////////////////////////////////////////////////////////////////////////////////////////////
S_API float Terrain::GetMinHeight() const
{
	return m_fMinHeight * m_HeightScale;
}

///////////////////////////////////////////////////////////////////////////////////////////////
S_API float Terrain::GetMaxHeight() const
{
	return m_fMaxHeight * m_HeightScale;
}

///////////////////////////////////////////////////////////////////////////////////////////////
S_API void Terrain::CalcMinMaxHeightAfterChange(Vec2f areaMin /*= Vec2f(0, 0)*/, Vec2f areaMax /*= Vec2f(1.0f, 1.0f)*/)
{
	if (!IS_VALID_PTR(m_pVtxHeightMap))
	{
		CLog::Log(S_ERROR, "Failed compute terrain min max heights: Heightmap not set!");
		return;
	}

	// Just to make sure...
	areaMin.x = max(min(areaMin.x, 1.0f), 0.0f);
	areaMin.y = max(min(areaMin.y, 1.0f), 0.0f);
	areaMax.x = max(min(areaMax.x, 1.0f), 0.0f);
	areaMax.y = max(min(areaMax.y, 1.0f), 0.0f);

	// Find new min and max height
	unsigned int heightmapSz[2];
	m_pVtxHeightMap->GetSize(&heightmapSz[0], &heightmapSz[1]);
	float* pStagedData = static_cast<float*>(m_pVtxHeightMap->GetStagedData());
	if (!IS_VALID_PTR(pStagedData))
	{
		CLog::Log(S_ERROR, "Could not determine terrain min and max heights: Staged data invalid!");
		return;
	}

	unsigned int area[] =
	{
		(unsigned int)(areaMin.x * (float)heightmapSz[0]),
		(unsigned int)(areaMin.y * (float)heightmapSz[1]),
		(unsigned int)(areaMax.x * (float)heightmapSz[0]),
		(unsigned int)(areaMax.y * (float)heightmapSz[1])
	};

	m_fMaxHeight = FLT_MIN;
	m_fMinHeight = FLT_MAX;
	for (unsigned int y = area[1]; y < area[3]; ++y)
	{
		for (unsigned int x = area[0]; x < area[2]; ++x)
		{
			const float& h = pStagedData[y * heightmapSz[1] + x];
			if (h < m_fMinHeight)
				m_fMinHeight = h;

			if (h > m_fMaxHeight)
				m_fMaxHeight = h;
		}
	}	
}

///////////////////////////////////////////////////////////////////////////////////////////////
S_API void Terrain::CalculateMinMaxHeights()
{
	CalcMinMaxHeightAfterChange();
}


///////////////////////////////////////////////////////////////////////////////////////////////
S_API SResult Terrain::GenerateFlatVertexHeightmap(float baseHeight)
{
	if (!IS_VALID_PTR(m_pEngine))
		return S_NOTINIT;

	IResourcePool* pRes = m_pEngine->GetResources();	

	// Remove old texture, if there was one
	if (IS_VALID_PTR(m_pVtxHeightMap) && !m_bCustomHeightmapSet)
		pRes->RemoveTexture(&m_pVtxHeightMap);

	m_bCustomHeightmapSet = false;

	// Create new Texture
	float baseHeightScaled = baseHeight / m_HeightScale;
	SColor baseColor(baseHeightScaled, baseHeightScaled, baseHeightScaled);	

	SResult res = pRes->AddTexture(m_nSegments + 1, m_nSegments + 1, "terrain_vtxheightmap", eTEXTURE_R32_FLOAT, baseColor, &m_pVtxHeightMap, true, true);
	if (Failure(res))
		return res;

	unsigned int rowPitch;
	float *pPixels;
	unsigned int nPixels;
	if (Success(m_pVtxHeightMap->Lock((void**)&pPixels, &nPixels, &rowPitch)))
	{
		rowPitch = (unsigned int)(rowPitch / 4);
		for (unsigned int x = 0; x < m_nSegments + 1; ++x)
		{
			for (unsigned int y = 0; y < m_nSegments + 1; ++y)
			{				
				/*float val = sinf((float)x / 10.0f) * sinf((float)y / 10.0f);				
				pPixels[y * rowPitch + x] = (val + 1.0f) * 0.5f;*/
				pPixels[y * rowPitch + x] = baseHeight;
			}
		}

		m_pVtxHeightMap->Unlock();
	}
	else
	{
		EngLog(S_ERROR, m_pEngine, "Failed lock vtx height map for terrain!");
	}

	CalculateMinMaxHeights();

	return S_SUCCESS;
}

///////////////////////////////////////////////////////////////////////////////////////////////
S_API float Terrain::SampleHeight(const Vec2f& texcoords, bool bilinear /* = false */) const
{
	if (!IS_VALID_PTR(m_pVtxHeightMap) || !m_pVtxHeightMap->IsStaged())
		return FLT_MIN;

	float val;
	if (!bilinear && Failure(m_pVtxHeightMap->SampleStaged(texcoords, &val)))
		return FLT_MIN;
	else if (bilinear && Failure(m_pVtxHeightMap->SampleStagedBilinear(texcoords, &val)))
		return FLT_MIN;

	return val * m_HeightScale;
}


///////////////////////////////////////////////////////////////////////////////////////////////
S_API bool Terrain::RayHeightmapIntersectionRec(float maxHeight, float minHeight, const SRay& ray, const unsigned int recDepth,
	const float step, Vec3f& intersection, const unsigned int curDepth) const
{
	//CLog::Log(S_DEBUG, "RecFunc cd=%u, step=%.4f, min=%.4f max=%.4f", curDepth, step, minHeight, maxHeight);

	// Create top and bottom planes from heights
	SPlane minPlane = SPlane::FromHeight(minHeight);
	SPlane maxPlane = SPlane::FromHeight(maxHeight);

	// Calculate plane intersections
	Vec3f minPlaneInt, maxPlaneInt;
	if (!GeomIntersects(minPlane, ray, &minPlaneInt))
	{
		CLog::Log(S_DEBUG, "Ray not intersecting with min plane (h=%.2f, i=%.2f,%.2f,%.2f)", minHeight, minPlaneInt.x, minPlaneInt.y, minPlaneInt.z);
		return false;
	}

	if (!GeomIntersects(maxPlane, ray, &maxPlaneInt))
	{
		CLog::Log(S_DEBUG, "Ray not intersecting with max plane (h=%.2f, i=%.2f,%.2f,%.2f)", maxHeight, maxPlaneInt.x, maxPlaneInt.y, maxPlaneInt.z);
		return false;
	}

	// Calculate step vector     
	Vec3f stepVec = Vec3Normalize(ray.v) * step;

	Vec2f terrainDimensions = GetMaxXZ() - GetMinXZ();

	// Loop through steps from max height to min height
	float lastSampledHeight = SampleHeight(Vec2f(maxPlaneInt.x, maxPlaneInt.z) / terrainDimensions, true);
	float lastHeight = maxHeight;
	//CLog::Log(S_DEBUG, "lh=%.2f lsh=%.2f", lastHeight, lastSampledHeight);
	unsigned int iStep = 0;
	bool bGoFurther = true;
	for (Vec3f curPos = maxPlaneInt + stepVec; bGoFurther; curPos += stepVec)
	{
		float newSampledHeight = SampleHeight(Vec2f(curPos.x, curPos.z) / terrainDimensions, true);
		//CLog::Log(S_DEBUG, "Step %u. nsh=%.4f nh=%.4f lsh=%.4f lh=%.4f", iStep, newSampledHeight, curPos.y, lastSampledHeight, lastHeight);
		++iStep;		

		if ((lastSampledHeight <= lastHeight && newSampledHeight >= curPos.y)
			|| (lastSampledHeight >= lastHeight && newSampledHeight <= curPos.y))
		{
			//CLog::Log(S_DEBUG, "intersects. cd=%u rd=%u", curDepth, recDepth);
			
			// if not reached max recursive depth, use a smaller step, otherwise we found the intersection						
			if (curDepth < recDepth)
			{
				if (RayHeightmapIntersectionRec(lastHeight, curPos.y, ray, recDepth, step * 0.5f, intersection, curDepth + 1))
					return true;
			}
			else
			{
				// found intersection
				//CLog::Log(S_DEBUG, "intersection.");
				intersection.x = curPos.x;
				intersection.y = (lastSampledHeight + newSampledHeight) * 0.5f;
				intersection.z = curPos.z;
				return true;
			}
		}

		// need to go further, store (sampled) height
		lastHeight = curPos.y;		
		lastSampledHeight = newSampledHeight;
		//CLog::Log(S_DEBUG, "lh=%.2f lsh=%.2f", lastHeight, lastSampledHeight);
		
		Vec3f testNextPos = curPos + stepVec;

		bGoFurther = (testNextPos.y >= minHeight || fabsf(testNextPos.y - minHeight) < 0.0001f)
			&& (testNextPos.y <= maxHeight || fabsf(testNextPos.y - maxHeight) < 0.0001f);

		//if (!(testNextPos.y >= minHeight && testNextPos.y <= maxHeight))
		//	CLog::Log(S_DEBUG, "cp.y=%.4f np.y=%.4f min=%.4f max=%.4f -> dont go further", curPos.y, testNextPos.y, minHeight, maxHeight);
	}

	// found nothing
	//CLog::Log(S_DEBUG, "Found no intersection. min=%.2f max=%.2f; minpi(%.1f,%.1f,%.1f) maxpi(%.1f,%.1f,%.1f)",
	//	minHeight, maxHeight, minPlaneInt.x, minPlaneInt.y, minPlaneInt.z, maxPlaneInt.x, maxPlaneInt.y, maxPlaneInt.z);	
	return false;
}

S_API bool Terrain::RayHeightmapIntersection(const SRay& ray, const unsigned int recDepth, const float step, Vec3f& intersection) const
{
	float maxHeight = GetMaxHeight();
	float minHeight = GetMinHeight();	
	return RayHeightmapIntersectionRec(maxHeight, minHeight - 1.0f, ray, recDepth, step, intersection, 0);
}







S_API STerrainRenderDesc* Terrain::GetUpdatedRenderDesc(SCamera* pCamera)
{
	m_RenderDesc.bRender = false;
	if (!m_bRequireRender)
	{
		return &m_RenderDesc;
	}

	m_RenderDesc.pColorMap = m_pColorMap;
	m_RenderDesc.pVtxHeightMap = m_pVtxHeightMap;
	m_RenderDesc.bRender = true;
	SMatrixIdentity(&m_RenderDesc.transform.scale);

	m_RenderDesc.bUpdateCB = false;

	if (m_bRequireCBUpdate)
	{
		m_RenderDesc.bUpdateCB = true;
		m_RenderDesc.constants.fTerrainDMFadeRadius = m_pEngine->GetSettings()->Get().render.fTerrainDMFadeRange;
		m_RenderDesc.constants.fTerrainMaxHeight = m_HeightScale;
		m_RenderDesc.constants.vtxHeightMapSz = m_nSegments + 1;
		m_RenderDesc.constants.segmentSize = m_fSegSz;
		m_bRequireCBUpdate = false;
	}


	// Prepare layer arrays
	if (!IS_VALID_PTR(m_RenderDesc.pDetailMaps) || (IS_VALID_PTR(m_RenderDesc.pDetailMaps) && m_RenderDesc.nLayers != m_Layers.size()))
	{
		if (IS_VALID_PTR(m_RenderDesc.pDetailMaps))
			delete[] m_RenderDesc.pDetailMaps;

		m_RenderDesc.pDetailMaps = new ITexture*[m_Layers.size()];
	}

	if (!IS_VALID_PTR(m_RenderDesc.pLayerMasks) || (IS_VALID_PTR(m_RenderDesc.pLayerMasks) && m_RenderDesc.nLayers != m_Layers.size()))
	{
		if (IS_VALID_PTR(m_RenderDesc.pLayerMasks))
			delete[] m_RenderDesc.pLayerMasks;

		m_RenderDesc.pLayerMasks = new ITexture*[m_Layers.size()];
	}

	// Fill layer arrays
	unsigned int iLayer = 0;
	for (auto itLayer = m_Layers.begin(); itLayer != m_Layers.end(); itLayer++)
	{
		STerrainLayer* pLayer = *itLayer;
		if (!IS_VALID_PTR(pLayer))
		{
			m_pEngine->LogE("Invalid terrain layer pointer!");
			return &m_RenderDesc;
		}

		m_RenderDesc.pLayerMasks[iLayer] = pLayer->pAlphaMask;
		m_RenderDesc.pDetailMaps[iLayer] = pLayer->pDetailMap;
		++iLayer;
	}

	m_RenderDesc.nLayers = iLayer;


	if (IS_VALID_PTR(m_RenderDesc.pDrawCallDescs) && m_RenderDesc.nDrawCallDescs < m_nLodLevels)
	{
		delete[] m_RenderDesc.pDrawCallDescs;
		m_RenderDesc.pDrawCallDescs = 0;
	}

	if (!IS_VALID_PTR(m_RenderDesc.pDrawCallDescs))
	{
		m_RenderDesc.pDrawCallDescs = new STerrainDrawCallDesc[m_nLodLevels];
		m_RenderDesc.nDrawCallDescs = m_nLodLevels;
	}

	for (unsigned int iLodLvl = 0; iLodLvl < m_nLodLevels; ++iLodLvl)
	{
		STerrainDrawCallDesc* dcd = &m_RenderDesc.pDrawCallDescs[iLodLvl];
		ITerrain::LodLevel* lodLvl = &m_pLodLevels[iLodLvl];

		if (!IS_VALID_PTR(lodLvl->pIndices) || !IS_VALID_PTR(lodLvl->pVertices) || lodLvl->nIndices == 0 || lodLvl->nVertices == 0)
		{
			dcd->bRender = false;
		}
		else
		{
			dcd->bRender = true;
			dcd->pVertexBuffer = lodLvl->pVB;
			dcd->pIndexBuffer = lodLvl->pIB;
			dcd->iStartVBIndex = 0;
			dcd->iEndVBIndex = lodLvl->nVertices - 1;
			dcd->iStartIBIndex = 0;
			dcd->iEndIBIndex = lodLvl->nActualIndices - 1;
			dcd->primitiveType = PRIMITIVE_TYPE_TRIANGLELIST;

			if (!IS_VALID_PTR(dcd->pVertexBuffer) || !IS_VALID_PTR(dcd->pIndexBuffer))
			{
				EngLog(S_ERROR, m_pEngine, "VB or IB of lod lvl #%u not inited!", iLodLvl);
				continue;
			}
		}
	}



	return &m_RenderDesc;
}




///////////////////////////////////////////////////////////////////////////////////////////////
S_API SResult Terrain::SetColorMap(ITexture* pColorMap)
{
	if (!IS_VALID_PTR(pColorMap))
		EngLog(S_INVALIDPARAM, m_pEngine, "Invalid ptr passed to Terrain::SetColorMap!");

	m_pColorMap = pColorMap;
	return S_SUCCESS;
}

///////////////////////////////////////////////////////////////////////////////////////////////
S_API ITexture* Terrain::GetColorMap() const
{
	return m_pColorMap;
}

///////////////////////////////////////////////////////////////////////////////////////////////
S_API unsigned int Terrain::AddLayer(const STerrainLayer& layer)
{
	m_Layers.push_back(new STerrainLayer(layer));
	return m_Layers.size() - 1;
}

///////////////////////////////////////////////////////////////////////////////////////////////
S_API unsigned int Terrain::GetLayerCount() const 
{
	return m_Layers.size();
}

///////////////////////////////////////////////////////////////////////////////////////////////
S_API STerrainLayer* Terrain::GetLayer(unsigned int index)
{
	if (m_Layers.size() <= index)
		return 0;

	return m_Layers.at(index);
}

///////////////////////////////////////////////////////////////////////////////////////////////
S_API void Terrain::Clear(void)
{	
	IResourcePool* pResources = 0;
	if (IS_VALID_PTR(m_pEngine))
		pResources = m_pEngine->GetResources();

	// Destruct lod levels
	if (IS_VALID_PTR(m_pLodLevels))
	{
		for (unsigned int iLodLvl = 0; iLodLvl < m_nLodLevels; ++iLodLvl)
		{
			ITerrain::LodLevel& lodLvl = m_pLodLevels[iLodLvl];			

			if (IS_VALID_PTR(pResources))
			{
				if (IS_VALID_PTR(lodLvl.pVB)) pResources->RemoveVertexBuffer(&lodLvl.pVB);
				if (IS_VALID_PTR(lodLvl.pIB)) pResources->RemoveIndexBuffer(&lodLvl.pIB);
			}

			SP_SAFE_DELETE_ARR(lodLvl.pChunkVertices, 1);
			SP_SAFE_DELETE_ARR(lodLvl.pIndices, 1);
			SP_SAFE_DELETE_ARR(lodLvl.pVertices, 1);
		}

		delete[] m_pLodLevels;		
	}
	m_pLodLevels = 0;

	// Delete chunk array
	SP_SAFE_DELETE_ARR(m_pChunks, 1);
		
	m_nLodLevels = 0;
	m_nChunks = 0;
	m_pColorMap = 0;


	if (m_Layers.size() > 0)
	{
		for (auto itLayer = m_Layers.begin(); itLayer != m_Layers.end(); itLayer++)
		{
			STerrainLayer* pLayer = *itLayer;
			if (IS_VALID_PTR(pLayer))				
				delete pLayer;
		}
	}

	m_Layers.clear();


	if (IS_VALID_PTR(pResources) && IS_VALID_PTR(m_pVtxHeightMap))
		pResources->RemoveTexture(&m_pVtxHeightMap);

	m_pVtxHeightMap = 0;
}


SP_NMSPACE_END