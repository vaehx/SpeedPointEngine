// SpeedPoint Terrain

#include "Terrain.h"
#include "..\IMaterial.h"
#include "C3DEngine.h"
#include <Renderer\IRenderer.h>
#include <Renderer\IVertexBuffer.h>
#include <Renderer\IIndexBuffer.h>
#include <Renderer\IResourcePool.h>
#include <Renderer\ITexture.h>
#include <Common\SVertex.h>

SP_NMSPACE_BEG

inline Vec2f _ScaleToTexCoords(float x, float y, const Vec2f& worldExtents)
{
	return Vec2f(fmodf(x, worldExtents.x), fmodf(y, worldExtents.y)) / worldExtents;
}



static ITerrain::LodLevel* g_pLodLevel = 0;

unsigned long g_MaxIdxAccum = 0;

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
			assert(idxAccum < g_MaxIdxAccum - 3);
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
				assert(idxAccum < g_MaxIdxAccum - 3);
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
			assert(idxAccum < g_MaxIdxAccum - 3);
		}
	}

	assert(idxAccum < g_MaxIdxAccum - 3);
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

	assert(idxAccum <= g_MaxIdxAccum - 3);

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

			assert(idxAccum <= g_MaxIdxAccum);
		}
	}

	pIB[idxAccum] = startVtxIdx + neighborVtx1;
	pIB[idxAccum + 1] = startVtxIdx + intVtx;
	pIB[idxAccum + 2] = startVtxIdx + cornerVtx;
	idxAccum += 3;	
	assert(idxAccum <= g_MaxIdxAccum);

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
			assert(idxAccum < g_MaxIdxAccum);
		}
	}

	pIB[idxAccum] = startVtxIdx + neighborVtx2;
	pIB[idxAccum + 1] = startVtxIdx + cornerVtx;
	pIB[idxAccum + 2] = startVtxIdx + intVtx;
	idxAccum += 3;
	assert(idxAccum <= g_MaxIdxAccum);
}






///////////////////////////////////////////////////////////////////////////////////////////////
S_API Terrain::Terrain()
	: m_pRenderer(nullptr),
	m_pColorMap(nullptr),
	m_bRequireCBUpdate(true),
	m_pLodLevels(0),
	m_pChunks(0),
	m_HeightScale(100.0f),
	m_pVtxHeightMap(nullptr),
	m_bCustomHeightmapSet(false),
	m_pTextureMaps(0),
	m_pNormalMaps(0),
	m_pGlossinessMaps(0),
	m_nLayers(0),
	m_pLayersUsed(0)
{
}

///////////////////////////////////////////////////////////////////////////////////////////////
S_API Terrain::~Terrain()
{
	Clear();
}

///////////////////////////////////////////////////////////////////////////////////////////////
S_API SResult Terrain::Init(IRenderer* pRenderer, const STerrainParams& params)
{
	m_pRenderer = pRenderer;
	m_Params = params;

	// Check given sizes		
	if (!IsPowerOfTwo(params.segments) || params.chunkSegments == 0 || (params.segments % params.chunkSegments) > 0)
		return S_INVALIDPARAM;

	if (params.chunkSegments < (unsigned int)PowerOfTwo(params.nLodLevels - 1) * 2)
		return CLog::Log(S_INVALIDPARAM, "Terrain::Init(): Too many lod levels (%u) for %u segments per quad", params.nLodLevels, params.chunkSegments);

	if (params.maskSz[0] == 0 || params.maskSz[1] == 0)
		return CLog::Log(S_INVALIDPARAM, "Terrain::Init(): Invalid layer mask size given in params (%d, %d)!", params.maskSz[0], params.maskSz[1]);

	m_fSegSz = params.size / (float)params.segments;
	m_fTexSz = 1.0f / (float)params.segments;

	m_pLodLevels = new ITerrain::LodLevel[params.nLodLevels];
	printf("Ter: Allocated LodLevels buffer with %u elements\n", params.nLodLevels);

	// Create chunk vertex array for each lod level
	for (unsigned int iLodLvl = 0; iLodLvl < params.nLodLevels; ++iLodLvl)
	{
		ITerrain::LodLevel& lodLvl = m_pLodLevels[iLodLvl];
		lodLvl.quadSegs = max(PowerOfTwo(iLodLvl), 1);
		lodLvl.chunkQuads = params.chunkSegments / lodLvl.quadSegs;
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
					SVertex(x * fQuadSz, params.baseHeight, z * fQuadSz, 0, 1.0f, 0, 1.0f, 0, 0,
						x * lodLvl.quadSegs * m_fTexSz, z * lodLvl.quadSegs * m_fTexSz);
			}
		}
	}

	// To initialize, generate a flat heightmap
	GenerateFlatVertexHeightmap(params.baseHeight);

	// Initialize chunk array
	unsigned long nChunks = pow2(params.segments / params.chunkSegments);
	m_pChunks = new STerrainChunk[nChunks];

	// Initialize empty layer textures
	IResourcePool* pResourcePool = m_pRenderer->GetResourcePool();
	m_nLayers = 5;
	m_pLayersUsed = new bool[m_nLayers];
	for (unsigned int i = 0; i < m_nLayers; ++i)
		m_pLayersUsed[i] = false;

	m_pLayerDescs = new STerrainLayerDesc[m_nLayers];
	
	m_pTextureMaps = pResourcePool->GetTexture("$terrain_textures");
	m_pTextureMaps->CreateEmptyArray(m_nLayers, params.textureSz[0], params.textureSz[1], eTEXTURE_R8G8B8A8_UNORM, 0);

	m_pNormalMaps = pResourcePool->GetTexture("$terrain_normals");
	m_pNormalMaps->CreateEmptyArray(m_nLayers, params.textureSz[0], params.textureSz[1], eTEXTURE_R8G8B8A8_UNORM);

	m_pGlossinessMaps = pResourcePool->GetTexture("$terrain_roughness");
	m_pGlossinessMaps->CreateEmptyArray(m_nLayers, params.textureSz[0], params.textureSz[1], eTEXTURE_R8G8B8A8_UNORM);

	// Initialize empty layer mask
	m_pLayermask = pResourcePool->GetTexture("$terrain_mask");
	m_pLayermask->CreateEmptyArray(m_nLayers, params.maskSz[0], params.maskSz[1], eTEXTURE_R32_FLOAT); // TODO: We probably don't need 32 bit float here....

	return S_SUCCESS;
}


///////////////////////////////////////////////////////////////////////////////////////////////
S_API const STerrainParams& Terrain::GetParams() const
{
	return m_Params;
}

///////////////////////////////////////////////////////////////////////////////////////////////
S_API void Terrain::GenLodLevelChunks(SCamera* pCamera)
{
	if (!IS_VALID_PTR(m_pRenderer))
		return;

	/*
	printf("Terrain::GenLodLevelChunks(pCamera=%p)\n", pCamera);
	*/

	IResourcePool* pResources = m_pRenderer->GetResourcePool();
	if (!IS_VALID_PTR(pResources))
		return;

	// Clear the chunk list of each lod level
	for (unsigned int iLodLvl = 0; iLodLvl < m_Params.nLodLevels; ++iLodLvl)
		m_pLodLevels[iLodLvl].chunks.clear();

	/*
	printf("Terrain: Cleared chunk vectors of %u lodLevels\n", m_Params.nLodLevels);
	*/

	// Determine LOD Level for each chunk and add to the according lod Level
	unsigned long nChunks = m_Params.segments / m_Params.chunkSegments;
	/*
	printf("Ter: nChunks per side: %u, which makes %u overall chunks\n", nChunks, nChunks * nChunks);
	*/

	Vec3f camPos = pCamera->position;
	if (m_Params.center)
		camPos += Vec3f(m_Params.size * 0.5f, 0, m_Params.size * 0.5f);

	for (unsigned int cz = 0; cz < nChunks; ++cz)
	{
		for (unsigned int cx = 0; cx < nChunks; ++cx)
		{
			STerrainChunk* pChunk = &m_pChunks[cz * nChunks + cx];						
			pChunk->cx = cx;
			pChunk->cz = cz;
			pChunk->fSize = m_fSegSz * m_Params.chunkSegments;
			pChunk->curLodLevel = pChunk->DetermineLODLevelByView(camPos, m_Params.nLodLevels, m_Params.fChunkStepDist);
			pChunk->quadSegs = max(PowerOfTwo(pChunk->curLodLevel), 1);
			pChunk->chunkSegments = m_Params.chunkSegments;
			
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

	for (unsigned int iLodLvl = 0; iLodLvl < m_Params.nLodLevels; ++iLodLvl)
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
		SP_SAFE_RELEASE(lodLvl.pVB);
		SP_SAFE_RELEASE(lodLvl.pIB);

		// Create the merged buffers for the lod level.
		// The IB is created with maxmimum size. It may happen, that payload is smaller than IB size.
		// To prevent false rendering, a maximum-index-accumulator is used.
		unsigned long lodLevelQuads = (unsigned long)lodLvl.chunks.size() * pow2(lodLvl.chunkQuads);		
		lodLvl.nIndices = lodLevelQuads * 6;		
		lodLvl.nVertices = lodLvl.chunks.size() * lodLvl.nChunkVertices;

g_MaxIdxAccum = lodLvl.nIndices;

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
			assert(idxAccum < lodLvl.nIndices - 3);

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
				pChunk->CreateBorder(lodLvl.pIndices, border.side, chunkVtxOffset, border.neighbor, lodLvlVtxRowShift, idxAccum);				
				assert(idxAccum < lodLvl.nIndices - 3);

				unsigned int iNextBorder = (iBorder + 1) % 4;

				STerrainChunkCorner corner;
				corner.neighbor1 = borders[iBorder].neighbor;
				corner.neighbor2 = borders[iNextBorder].neighbor;
				corner.side1 = border.side;			
				g_pLodLevel = &m_pLodLevels[iLodLvl];
				
				pChunk->CreateCorner(corner, lodLvl.pIndices, chunkVtxOffset, lodLvlVtxRowShift, idxAccum);
				assert(idxAccum <= lodLvl.nIndices);
			}			

			// Add vertices
			float halfSz = m_Params.size * 0.5f;
			for (unsigned long iVtx = 0; iVtx < lodLvl.nChunkVertices; ++iVtx)
			{
				SVertex* pVtx = &lodLvl.pVertices[chunkVtxOffset + iVtx];				
				
				memcpy(pVtx, &lodLvl.pChunkVertices[iVtx], sizeof(SVertex));

				if (m_Params.center)
				{
					pVtx->x -= halfSz;
					pVtx->z -= halfSz;
				}

				pVtx->position.x += pChunk->cx * pChunk->fSize;
				pVtx->position.z += pChunk->cz * pChunk->fSize;
				pVtx->textureCoords[0].u += (float)(pChunk->cx * m_Params.chunkSegments) * m_fTexSz;
				pVtx->textureCoords[0].v += (float)(pChunk->cz * m_Params.chunkSegments) * m_fTexSz;
			}			
			
			iChunk++;
		}

		// Now create lodLevel's Hardware VB and Hardware IB
		pResources->AddVertexBuffer(&lodLvl.pVB);
		lodLvl.pVB->Initialize(m_pRenderer, eVBUSAGE_STATIC, lodLvl.pVertices, lodLvl.nVertices);

		pResources->AddIndexBuffer(&lodLvl.pIB);		
		lodLvl.nActualIndices = idxAccum;
		lodLvl.pIB->Initialize(m_pRenderer, eIBUSAGE_STATIC, S_INDEXBUFFER_32, lodLvl.pIndices, lodLvl.nActualIndices);
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

	if (!m_bCustomHeightmapSet && IS_VALID_PTR(m_pVtxHeightMap) && IS_VALID_PTR(m_pRenderer))
	{
		m_pVtxHeightMap->Release();
	}

	unsigned int heightmapSz[2];
	heightmap->GetSize(&heightmapSz[0], &heightmapSz[1]);
	if (heightmapSz[0] != m_Params.segments
		&& heightmapSz[0] != m_Params.segments + 1) // World Machine outputs with size + 1
	{
		CLog::Log(S_WARNING, "Terrain heightmap resolution (%dx%d) imcompatible with terrain segments per side (%dx%d) for %s",
			heightmapSz[0], heightmapSz[1], m_Params.segments, m_Params.segments, heightmap->GetSpecification().c_str());
	}

	m_pVtxHeightMap = heightmap;
	m_bCustomHeightmapSet = true;

	MarkDirty();
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
S_API const float* Terrain::GetHeightData() const
{
	if (!m_pVtxHeightMap || !m_pVtxHeightMap->IsStaged())
		return 0;

	return static_cast<float*>(m_pVtxHeightMap->GetStagedData());
}

///////////////////////////////////////////////////////////////////////////////////////////////
S_API void Terrain::MarkDirty()
{
	if (!IS_VALID_PTR(m_pVtxHeightMap))
	{
		CLog::Log(S_ERROR, "Failed mark terrain dirty area: Heightmap not set!");
		return;
	}

	// Find new min and max height
	unsigned int heightmapSz[2];
	m_pVtxHeightMap->GetSize(&heightmapSz[0], &heightmapSz[1]);
	float* pStagedData = static_cast<float*>(m_pVtxHeightMap->GetStagedData());
	if (!IS_VALID_PTR(pStagedData))
	{
		CLog::Log(S_ERROR, "Could not determine terrain min and max heights: Staged data invalid!");
		return;
	}

	m_fMaxHeight = -FLT_MAX;
	m_fMinHeight = FLT_MAX;
	for (unsigned int y = 0; y < heightmapSz[1]; ++y)
	{
		for (unsigned int x = 0; x < heightmapSz[0]; ++x)
		{
			const float& h = pStagedData[y * heightmapSz[0] + x];
			if (h < m_fMinHeight)
				m_fMinHeight = h;

			if (h > m_fMaxHeight)
				m_fMaxHeight = h;
		}
	}
}


///////////////////////////////////////////////////////////////////////////////////////////////
S_API SResult Terrain::GenerateFlatVertexHeightmap(float baseHeight)
{
	if (!IS_VALID_PTR(m_pRenderer))
		return S_NOTINIT;

	IResourcePool* pRes = m_pRenderer->GetResourcePool();

	// Remove old texture, if there was one
	if (IS_VALID_PTR(m_pVtxHeightMap) && !m_bCustomHeightmapSet)
		m_pVtxHeightMap->Release();

	m_bCustomHeightmapSet = false;

	// Create new Texture
	float baseHeightScaled = baseHeight / m_HeightScale;
	SColor baseColor(baseHeightScaled, baseHeightScaled, baseHeightScaled);	

	m_pVtxHeightMap = pRes->GetTexture("terrain_vtxheightmap");
	SResult res = m_pVtxHeightMap->CreateEmpty(m_Params.segments + 1, m_Params.segments + 1, 1, eTEXTURE_R32_FLOAT, baseColor);
	if (Failure(res))
		return res;

	unsigned int rowPitch;
	float *pPixels;
	unsigned int nPixels;
	if (Success(m_pVtxHeightMap->Lock((void**)&pPixels, &nPixels, &rowPitch)))
	{
		rowPitch = (unsigned int)(rowPitch / 4);
		for (unsigned int x = 0; x < m_Params.segments + 1; ++x)
		{
			for (unsigned int y = 0; y < m_Params.segments + 1; ++y)
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
		CLog::Log(S_ERROR, "Failed lock vtx height map for terrain!");
	}

	MarkDirty();

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
S_API bool Terrain::RayHeightmapIntersectionRec(float maxHeight, float minHeight, const geo::ray& ray, const unsigned int recDepth,
	const float step, Vec3f& intersection, const unsigned int curDepth) const
{
	//CLog::Log(S_DEBUG, "RecFunc cd=%u, step=%.4f, min=%.4f max=%.4f", curDepth, step, minHeight, maxHeight);

	I3DEngine* p3DEngine = C3DEngine::Get();

	Vec2f minXZ = GetMinXZ(), maxXZ = GetMaxXZ();
	Vec2f terrainDimensions = maxXZ - minXZ;

	// Create top and bottom planes from heights
	geo::plane minPlane(Vec3f(0, 1.0f, 0), minHeight), maxPlane(Vec3f(0, 1.0f, 0), maxHeight);

	// Calculate plane intersections
	geo::SIntersection minPlaneInt, maxPlaneInt;
	if (!geo::_Intersection(&minPlane, &ray, &minPlaneInt)) {
		CLog::Log(S_DEBUG, "Ray not intersecting with min plane (h=%.2f, i=%.2f,%.2f,%.2f)", minHeight, minPlaneInt.p.x, minPlaneInt.p.y, minPlaneInt.p.z);
		return false;
	}
	if (!geo::_Intersection(&maxPlane, &ray, &maxPlaneInt)) {
		CLog::Log(S_DEBUG, "Ray not intersecting with max plane (h=%.2f, i=%.2f,%.2f,%.2f)", maxHeight, maxPlaneInt.p.x, maxPlaneInt.p.y, maxPlaneInt.p.z);
		return false;
	}

	//p3DEngine->AddHelper<CSphereHelper>(CSphereHelper::Params(minPlaneInt.p, 0.5f), true)->SetColor(SColor::Red());

	// If start point is not inside terrain aabb, find the first intersection of the ray with it.
	// If the line segment does not intersect the terrain AABB, we don't have an intersection
	Vec3f startPos = maxPlaneInt.p;
	
	if (curDepth == 0 && false)
	{
		AABB aabb(Vec3f(minXZ.x, minHeight, minXZ.y), Vec3f(maxXZ.x, maxHeight, maxXZ.y));
		Vec3f aabbCenter = 0.5f * (aabb.vMin + aabb.vMax);

		p3DEngine->AddHelper<CBoxHelper>(CBoxHelper::Params(aabbCenter,
			0.5f * (aabb.vMax.x - aabb.vMin.x), 0.5f * (aabb.vMax.y - aabb.vMin.y), 0.5f * (aabb.vMax.z - aabb.vMin.z)),
			true)->SetColor(SColor::Red());

		if (!aabb.ContainsPoint(startPos))
		{
			bool inters = false;
			float t_min = FLT_MAX;
			for (int i = 0; i < 3; ++i)
				for (int j = -1; j <= 1; j += 2)
				{
					Vec3f n;
					n[i] = (float)j;
					geo::quotient t(Vec3Dot(n, (aabbCenter + n * 0.5f * (aabb.vMax[i] - aabb.vMin[i])) - startPos), Vec3Dot(n, ray.v));
					if (fabsf(t.d) < 0.001f)
						continue; // Ray parallel to side - degenerate

					float tt = t.val();
					if (tt >= 0)
					{
						t_min = min(t_min, t.val());
						inters = true;
					}
				}

			if (!inters)
				return false;

			startPos = startPos + ray.v * t_min;
		}
	}

	// Calculate step vector
	Vec3f stepVec = Vec3Normalize(ray.v) * step;

	// Loop through steps from max height to min height
	Vec3f lastPos = startPos;
	float lastSampledHeight = SampleHeight((Vec2f(lastPos.x, lastPos.z) - minXZ) / terrainDimensions, true);

	for (Vec3f curPos = startPos + stepVec; ; curPos += stepVec)
	{
		float newSampledHeight = SampleHeight((Vec2f(curPos.x, curPos.z) - minXZ) / terrainDimensions, true);

		//p3DEngine->AddHelper<CSphereHelper>(CSphereHelper::Params(Vec3f(curPos.x, newSampledHeight, curPos.z), 1.0f), true)->SetColor(SColor::Blue());

		if ((lastSampledHeight <= lastPos.y && newSampledHeight >= curPos.y)
			|| (lastSampledHeight >= lastPos.y && newSampledHeight <= curPos.y))
		{
			// There is an intersection in this segment. Approximate where.

			// if not reached max recursive depth, use a smaller step, otherwise we reached best possible approximation
			if (curDepth < recDepth)
			{
				RayHeightmapIntersectionRec(lastPos.y, curPos.y, ray, recDepth, step * 0.5f, intersection, curDepth + 1);
			}
			else
			{
				float halfSz = m_Params.size * 0.5f;
				intersection.x = curPos.x;
				intersection.y = (lastSampledHeight + newSampledHeight) * 0.5f;
				intersection.z = curPos.z;
			}

			return true;
		}

		lastPos = curPos;
		lastSampledHeight = newSampledHeight;

		if (curPos.y < minHeight || curPos.y > maxHeight)
			break;
	}

	// found nothing
	CLog::Log(S_DEBUG, "Found no intersection. min=%.2f max=%.2f; minpi(%.1f,%.1f,%.1f) maxpi(%.1f,%.1f,%.1f)",
		minHeight, maxHeight, minPlaneInt.p.x, minPlaneInt.p.y, minPlaneInt.p.z, maxPlaneInt.p.x, maxPlaneInt.p.y, maxPlaneInt.p.z);	

	return false;
}

S_API bool Terrain::RayHeightmapIntersection(const geo::ray& ray, const unsigned int recDepth, const float step, Vec3f& intersection) const
{
	float maxHeight = GetMaxHeight();
	float minHeight = GetMinHeight();

	if (maxHeight - minHeight < 0.001f)
	{
		minHeight -= 0.5f;
		maxHeight += 0.5f;
	}

	return RayHeightmapIntersectionRec(maxHeight, minHeight - 1.0f, ray, recDepth, step, intersection, 0);
}







S_API void Terrain::UpdateRenderDesc(STerrainRenderDesc* pTerrainRenderDesc)
{
	if (!IS_VALID_PTR(pTerrainRenderDesc))
	{
		return;
	}

	pTerrainRenderDesc->bRender = false;	
	if (!m_bRequireRender || m_Params.nLodLevels == 0 || !m_pLodLevels)
	{
		return;
	}

	pTerrainRenderDesc->pColorMap = m_pColorMap;
	pTerrainRenderDesc->pVtxHeightMap = m_pVtxHeightMap;
	pTerrainRenderDesc->pLayerMask = m_pLayermask;
	pTerrainRenderDesc->pTextureMaps = m_pTextureMaps;
	pTerrainRenderDesc->pNormalMaps = m_pNormalMaps;
	pTerrainRenderDesc->pRoughnessMaps = m_pGlossinessMaps;

	pTerrainRenderDesc->bRender = true;
	SMatrixIdentity(pTerrainRenderDesc->transform.scale);

	pTerrainRenderDesc->bUpdateCB = false;

	if (m_bRequireCBUpdate)
	{
		pTerrainRenderDesc->bUpdateCB = true;
		pTerrainRenderDesc->constants.fTerrainDMFadeRadius = m_Params.detailmapRange;
		pTerrainRenderDesc->constants.fTerrainMaxHeight = m_HeightScale;
		
		unsigned int vtxHeightmapSz[2];
		m_pVtxHeightMap->GetSize(&vtxHeightmapSz[0], &vtxHeightmapSz[1]);
		pTerrainRenderDesc->constants.vtxHeightMapSz = vtxHeightmapSz[0]; // assuming its squared

		pTerrainRenderDesc->constants.segmentSize = m_fSegSz;
		pTerrainRenderDesc->constants.detailmapSz[0] = m_Params.detailmapSz[0];
		pTerrainRenderDesc->constants.detailmapSz[1] = m_Params.detailmapSz[1];
		pTerrainRenderDesc->constants.numLayers = m_nLayers;
		m_bRequireCBUpdate = false;
	}

	if (IS_VALID_PTR(pTerrainRenderDesc->pDrawCallDescs) && pTerrainRenderDesc->nDrawCallDescs < m_Params.nLodLevels)
	{
		delete[] pTerrainRenderDesc->pDrawCallDescs;
		pTerrainRenderDesc->pDrawCallDescs = 0;
	}

	if (!IS_VALID_PTR(pTerrainRenderDesc->pDrawCallDescs))
	{
		pTerrainRenderDesc->pDrawCallDescs = new STerrainDrawCallDesc[m_Params.nLodLevels];
		pTerrainRenderDesc->nDrawCallDescs = m_Params.nLodLevels;
	}

	for (unsigned int iLodLvl = 0; iLodLvl < m_Params.nLodLevels; ++iLodLvl)
	{
		STerrainDrawCallDesc* dcd = &pTerrainRenderDesc->pDrawCallDescs[iLodLvl];
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
				CLog::Log(S_ERROR, "VB or IB of lod lvl #%u not inited!", iLodLvl);
				continue;
			}
		}
	}
}


///////////////////////////////////////////////////////////////////////////////////////////////
S_API Vec2f Terrain::GetMinXZ() const
{
	if (m_Params.center)
	{
		float halfSz = m_Params.size * 0.5f;
		return Vec2f(-halfSz, -halfSz);
	}
	else
	{
		return Vec2f(0, 0);
	}
}

///////////////////////////////////////////////////////////////////////////////////////////////
S_API Vec2f Terrain::GetMaxXZ() const
{
	float sz = m_Params.segments * m_fSegSz;
	if (m_Params.center)
	{
		float halfSz = m_Params.size * 0.5f;
		return Vec2f(halfSz, halfSz);
	}
	else
	{
		return Vec2f(sz, sz);
	}
}

///////////////////////////////////////////////////////////////////////////////////////////////
S_API Vec2f Terrain::XZToTexCoords(float x, float z) const
{
	Vec2f worldSpaceExtents = GetMaxXZ() - GetMinXZ();
	return Vec2f(
		fmodf(x, worldSpaceExtents.x) / worldSpaceExtents.x,
		fmodf(z, worldSpaceExtents.y) / worldSpaceExtents.y);
}



///////////////////////////////////////////////////////////////////////////////////////////////
S_API SResult Terrain::SetColorMap(ITexture* pColorMap)
{
	if (!IS_VALID_PTR(pColorMap))
		return CLog::Log(S_INVALIDPARAM, "Invalid ptr passed to Terrain::SetColorMap!");

	m_pColorMap = pColorMap;
	return S_SUCCESS;
}

///////////////////////////////////////////////////////////////////////////////////////////////
S_API unsigned int Terrain::AddLayer(const STerrainLayerDesc& desc)
{
	C3DEngine* p3DEngine = C3DEngine::Get();
	IMaterial* pMaterial = p3DEngine->GetMaterialManager()->GetMaterial(desc.materialName);
	if (!pMaterial)
		return UINT_MAX;

	// Find unused layer
	unsigned int layer = m_nLayers;
	if (m_pLayersUsed)
	{
		for (unsigned int i = 0; i < m_nLayers; ++i)
		{
			if (!m_pLayersUsed[i])
			{
				layer = i;
				break;
			}
		}
	}

	// If there is no unused layer, allocate more layers
	if (layer == m_nLayers)
	{
		unsigned int newNumLayers = m_nLayers + 5;
		
		bool* pNewLayersUsed = new bool[newNumLayers];
		if (m_pLayersUsed)
		{
			for (unsigned int i = 0; i < newNumLayers; ++i)
			{
				if (i < m_nLayers)
					pNewLayersUsed[i] = m_pLayersUsed[i];
				else
					pNewLayersUsed[i] = false;
			}

			delete[] m_pLayersUsed;
		}

		STerrainLayerDesc* pNewLayerDescs = new STerrainLayerDesc[newNumLayers];
		if (m_pLayerDescs)
		{
			for (unsigned int i = 0; i < m_nLayers; ++i)
				pNewLayerDescs[i] = m_pLayerDescs[i];

			delete[] m_pLayerDescs;
		}

		m_nLayers = newNumLayers;
		m_pLayersUsed = pNewLayersUsed;
		m_pLayerDescs = pNewLayerDescs;
		m_pTextureMaps->ResizeArray(newNumLayers);
		m_pNormalMaps->ResizeArray(newNumLayers);
		m_pGlossinessMaps->ResizeArray(newNumLayers);
		m_pLayermask->ResizeArray(newNumLayers);
	}

	// Flag layer used
	m_pLayersUsed[layer] = true;

	m_bRequireCBUpdate = true;

	// Fill layer
	IResourcePool* pResourcePool = p3DEngine->GetRenderer()->GetResourcePool();
	SMaterialDefinition* pMatDef = pMaterial->GetDefinition();

	if (!pMatDef->textureMap.empty())
		m_pTextureMaps->LoadArraySliceFromFile(layer, pResourcePool->GetResourceSystemPath(pMatDef->textureMap));

	if (!pMatDef->normalMap.empty())
		m_pNormalMaps->LoadArraySliceFromFile(layer, pResourcePool->GetResourceSystemPath(pMatDef->normalMap));

	if (!pMatDef->glossinessMap.empty())
		m_pGlossinessMaps->LoadArraySliceFromFile(layer, pResourcePool->GetResourceSystemPath(pMatDef->glossinessMap));

	
	// TODO: Specular maps?



	// Fill mask
	bool clearMask = true;
	if (!desc.mask.empty())
	{
		bool attemptLoadMask = true;
		
		// Try to copy from existing resource
		ITexture* pExistingMask = pResourcePool->GetTexture(desc.mask, eTEX_ABSENT_NOOP);
		if (pExistingMask)
		{
			attemptLoadMask = false;
			if (m_pLayermask->CopyArraySliceFromTexture(layer, pExistingMask))
				clearMask = false;

			pExistingMask->Release();
			pExistingMask = 0;
		}
		
		// If no success yet, attempt to load the array slice directly from file
		if (attemptLoadMask)
		{
			if (Success(m_pLayermask->LoadArraySliceFromFile(layer, pResourcePool->GetResourceSystemPath(desc.mask))))
				clearMask = false;
		}
	}

	// If no mask file specified or any attempt to load it failed, use a cleared texture as alpha mask
	if (clearMask)
		m_pLayermask->FillArraySlice(layer, SColor::Black());

	m_pLayerDescs[layer] = desc;

	return layer;
}

///////////////////////////////////////////////////////////////////////////////////////////////
S_API unsigned int Terrain::GetLayerCount() const 
{
	return m_nLayers;
}

///////////////////////////////////////////////////////////////////////////////////////////////
S_API STerrainLayerDesc Terrain::GetLayerDesc(unsigned int i) const
{
	if (i >= m_nLayers)
		return STerrainLayerDesc();

	return m_pLayerDescs[i];
}

///////////////////////////////////////////////////////////////////////////////////////////////
S_API void Terrain::RemoveLayer(unsigned int id)
{
	if (id >= m_nLayers)
		return;

	m_pLayersUsed[id] = false;
}

///////////////////////////////////////////////////////////////////////////////////////////////
S_API void Terrain::Clear(void)
{
	// Destruct lod levels
	if (IS_VALID_PTR(m_pLodLevels))
	{
		for (unsigned int iLodLvl = 0; iLodLvl < m_Params.nLodLevels; ++iLodLvl)
		{
			ITerrain::LodLevel& lodLvl = m_pLodLevels[iLodLvl];			

			SP_SAFE_RELEASE(lodLvl.pVB);
			SP_SAFE_RELEASE(lodLvl.pIB);

			SP_SAFE_DELETE_ARR(lodLvl.pChunkVertices, 1);
			SP_SAFE_DELETE_ARR(lodLvl.pIndices, 1);
			SP_SAFE_DELETE_ARR(lodLvl.pVertices, 1);
		}

		delete[] m_pLodLevels;		
	}
	m_pLodLevels = 0;

	// Delete chunk array
	SP_SAFE_DELETE_ARR(m_pChunks, 1);
		
	m_Params.nLodLevels = 0;
	m_nChunks = 0;
	m_pColorMap = 0;

	// Clear layers
	if (m_pLayersUsed)
	{
		delete[] m_pLayersUsed;
		m_pLayersUsed = 0;
	}

	if (m_pLayerDescs)
	{
		delete[] m_pLayerDescs;
		m_pLayerDescs = 0;
	}

	m_nLayers = 0;

	SP_SAFE_RELEASE(m_pTextureMaps);
	SP_SAFE_RELEASE(m_pNormalMaps);
	SP_SAFE_RELEASE(m_pGlossinessMaps);

	SP_SAFE_RELEASE(m_pVtxHeightMap);
	SP_SAFE_RELEASE(m_pLayermask);
}


SP_NMSPACE_END
