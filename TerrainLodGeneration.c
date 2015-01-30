
o-----o-----o-----o-----o-----o-----o-----o-----o-----o-----o
|   / |   / |   / |   / |   / |   / |   / |   / |   / |   / |
| /   | /   | /   | /   | /   | /   | /   | /   | /   | /   |
o-----o-----o-----o-----o-----o-----o-----o-----o-----o-----o
|   / |   / |   / |   / |   / |   / |   / |   / |   / |   / |
| /   | /   | /   | /   | /   | /   | /   | /   | /   | /   |
o-----o-----o-----o-----o-----o-----o-----o-----o-----o-----o
|   / |   / |   / |   / |   / |   / |   / |   / |   / |   / |
| /   | /   | /   | /   | /   | /   | /   | /   | /   | /   |
o-----o-----o-----o-----o-----o-----o-----o-----o-----o-----o
|   / |   / |   / |   / |   / |   / |   / |   / |   / |   / |
| /   | /   | /   | /   | /   | /   | /   | /   | /   | /   |
o-----o-----o-----o-----o-----o-----o-----o-----o-----o-----o
|   / |   / |   / |   / |   / |   / |   / |   / |   / |   / |
| /   | /   | /   | /   | /   | /   | /   | /   | /   | /   |
o-----o-----o-----o-----o-----o-----o-----o-----o-----o-----o
|   / |   / |   / |   / |   / |   / |   / |   / |   / |   / |
| /   | /   | /   | /   | /   | /   | /   | /   | /   | /   |
o-----o-----o-----o-----o-----o-----o-----o-----o-----o-----o
|   / |   / |   / |   / |   / |   / |   / |   / |   / |   / |
| /   | /   | /   | /   | /   | /   | /   | /   | /   | /   |
o-----o-----o-----o-----o-----o-----o-----o-----o-----o-----o
|   / |   / |   / |   / |   / |   / |   / |   / |   / |   / |
| /   | /   | /   | /   | /   | /   | /   | /   | /   | /   |
o-----o-----o-----o-----o-----o-----o-----o-----o-----o-----o






Lod Level 0:
Rest = 0 --> nChunkSegsOneSide 1x1-Segs

o-----o-----o-----o-----o-----o-----o-----o-----o-----o-----o-----o



Lod Level 1:
Rest = nChunkSegsOneSide % 2 --> nChunkSegsOneSide % 2

o-----------o-----------o-----------o-----------o-----------o-----o
n1x1SegsXY = nChunkSegsXY % 2
n2x2SegsXY = nChunkSegsXY - n1x1SegsXY



Lod Level 2:
Rest = nChunkSegsOneSide % 4 

o-----------------------o-----------------------o-----------o-----o
n4x4RestXY = n8x8RestXY % 4; n4x4SegsXY = n8x8RestXY - n4x4RestXY = nChunkSegsXY - (nChunkSegsXY % 4);
n2x2RestXY = n4x4RestXY % 2; n2x2SegsXY = n4x4RestXY - n2x2RestXY = n4x4RestXY - (n4x4RestXY % 2);
n1x1RestXY = 0; n1x1SegsXY = n2x2RestXY - n1x1RestXY = n2x2RestXY - 0 = n2x2RestXY



Lod Level 3:

o-----------------------------------------------o-----------o-----o
n8x8RestXY = n16x16RestXY % 8; n8x8SegsXY = n16x16RestXY - n8x8RestXY = n16x16RestXY - (n16x16RestXY % 8);
n4x4RestXY = n8x8RestXY % 4; n4x4SegsXY = n8x8RestXY - n4x4RestXY = nChunkSegsXY - (nChunkSegsXY % 4);
n2x2RestXY = n4x4RestXY % 2; n2x2SegsXY = n4x4RestXY - n2x2RestXY = n4x4RestXY - (n4x4RestXY % 2);
n1x1RestXY = 0; n1x1SegsXY = n2x2RestXY - n1x1RestXY = n2x2RestXY - 0 = n2x2RestXY











struct STerrain
{
    SVertex** pVertexBuffers; // one VB per lod level
    SIndex** pIndexBuffers; // one IB per lod level
    unsigned int nSegsX, nSegsZ;    
    unsigned int nLodLevels;
    unsigned int (*pLodLevelCounts)[2]; // [0]: Vtx Count, [1]: index count
    float fWidth, fDepth;
    
    STerrainChunk* pTerrainChunks;
    unsigned int nChunksX, nChunksZ; // including rest chunks
    unsigned int nRestChunksX, nRestChunksZ; // essentially 0 or 1    
    
    // Top rest chunk: (nChunkSegsX, nZRestChunkSegs)
    // Right rest chunk: (nXRestChunkSegs, nChunkSegsZ)
    // Fill rest chunk: (nXRestChunkSegs, nZRestChunkSegs)    
    unsigned int nXRestChunkSegs, nZRestChunkSegs;

    void Generate(unsigned int segsX, unsigned int segsZ,
        float w, float d,
        unsigned int lodLevels,
        unsigned int chunkSegsX, unsigned int chunkSegsZ);                
};

void STerrain::Generate(unsigned int segsX, unsigned int segsZ,
    float w, float d,
    unsigned int lodLevels,
    unsigned int chunkSegsX, unsigned int chunkSegsZ)
{
    nSegsX = segsX;
    nSegsZ = segsZ;    
    fWidth = w;
    fDepth = d;
    nLodLevels = lodLevels;
    
    nChunkSegsX = chunkSegsX;
    nChunkSegsZ = chunkSegsZ;
    
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
    
    // calculate segment sizes
    float segWidth = w / nSegsX;
    float segDepth = d / nSegsZ;
    float segUSz = segWidth;
    float segVSz = segDepth;        
    
    // generate chunks buffer
    pTerrainChunks = new STerrainChunk[nChunksX * nChunksZ];
        
    // Precalculate vertex and index count, create the buffers and create chunks
    pLodLevelCounts = new STerrainChunkCounts[nLodLevels];
    pVertexBuffers = new SVertex*[nLodLevels];
    pIndexBuffers = new SIndex*[nLodLevels];
    for (unsigned int lodLvl = 0; lodLvl < nLodLevels; ++lodLvl)
    {   
        STerrainChunkCounts regChunkCounts = STerrainChunk::PrecalcCounts(lodLvl, nChunkSegsX, nChunkSegsZ);
        STerrainChunkCounts restXChunkCnt = STerrainChunk::PrecalcCounts(lodLvl, nXRestChunkSegs, nChunkSegsZ);
        STerrainChunkCounts restZChunkCnt = STerrainChunk::PrecalcCounts(lodLvl, nChunkSegsX, nZRestChunkSegs);
        STerrainChunkCounts fillChunkCnt = STerrainChunk::PrecalcCounts(lodLvl, nXRestChunkSegs, nZRestChunkSegs);
                                  
        vtxCnt = regChunkCounts.vertices * (nChunksX - nRestChunksX) * (nChunksZ - nRestChunksZ);
        idxCnt = regChunkCounts.indices * (nChunksX - nRestChunksX) * (nChunksZ - nRestChunksZ);
        
        if (nRestChunksX > 0 || nRestChunksZ > 0)
        {
            vtxCnt += nRestChunksX * (nChunksZ - nRestChunksZ) * restXChunkCnt.vertices;                               
            vtxCnt += nRestChunksZ * (nChunksX - nRestChunksX) * restZChunkCnt.vertices;
            vtxCnt += nRestChunksX * nRestChunksZ * fillChunkCnt.vertices;                                                            
        
            idxCnt += nRestChunksX * (nChunksZ - nRestChunksZ) * restXChunkCnt.indices;                               
            idxCnt += nRestChunksZ * (nChunksX - nRestChunksX) * restZChunkCnt.indices;
            idxCnt += nRestChunksX * nRestChunksZ * fillChunkCnt.vertices;    
        }
        
        pVertexBuffers[lodLvl] = new SVertex[vtxCnt];
        pIndexBuffers[lodLvl] = new SIndex[idxCnt];
        
                        
        
        // fill chunks
        unsigned long iCurVtxPos = 0, iCurIdxPos = 0;                
        for (unsigned int cz = 0; cz < nChunksZ; ++cz)
        {
            bool bIsZRest = (cz == nChunksZ - 1) && (nRestChunksZ > 0);
            for (unsigned int cx = 0; cx < nChunksX; ++cx)
            {
                bool bIsXRest = (cx == nChunksX - 1) && (nRestChunksX > 0); 
        
                STerrainChunk* pChunk = pTerrainChunks[cz * nChunksX + cx];
                
                // Initialize chunk
                if (lodLvl == 0)
                {
                    pChunk->pVertices = new SIndex*[nLodLevels];
                    pChunk->pIndices = new SIndex*[nLodLevels];
                    pChunk->nLodLevels = nLodLevels;                    
                    
                    pChunk->nSegsX = (bIsXRest) ? nXRestChunkSegs : nChunkSegsX;
                    pChunk->nSegsZ = (bIsZRest) ? nZRestChunkSegs : nChunkSegsZ;
                    pChunk->fWidth = nSegsX * segWidth;
                    pChunk->fDepth = nSegsZ * segDepth;
                    pChunk->fSegUSz = segUSz;
                    pChunk->fSegVSz = segVSz;
                    pChunk->fXOffs = cx * (segWidth * nChunkSegsX);
                    pChunk->fZOffs = cz * (segDepth * nChunkSegsZ);
                    pChunk->fUOffs = pChunk->fXOffs;
                    pChunk->fVOffs = pChunk->fZOffs;                                                               
                }
                
                pChunk->pVertices[lodLevel] = &pVertexBuffers[lodLvl][iCurVtxPos];
                pChunk->pIndices[lodLevel] = &pIndexBuffers[lodLvl][iCurIdxPos];                                                  
                
                pChunk->GenerateLodLevel(lodLvl, STerrainChunk::GetLodLevelQuadStartSz(lodLvl));                            
            }
        }                                                                                                                          
    }                              
}






struct STerrainChunkCounts
{
    unsigned int indices, vertices;
    unsigned int quadsX, quadsZ;    
};

struct STerrainChunk
{
	unsigned int nSegsX, nSegsZ; // on high res grid
    float fWidth, fDepth;
    float fXOffs, fZOffs;
    float fUOffs, fVOffs;
    float fSegUSz, fSegVSz;
    SIndex** pIndices; // (SIndex[nIndices])[nLodLevels]
    SVertex** pVertices; // (SVertex[nIndices])(nLodLevels)
    unsigned int nLodLevels;    
    
    static unsigned int CountSideParts(unsigned int w, unsigned int startsz)
    inline static unsigned int GetLodLevelQuadStartSz(unsigned int lodLevel)
    {
        return (lodLevel == 0) ? 1 : lodLvl * 2;
    }
        
    static STerrainChunkCounts PrecalcCounts(unsigned int lodLevel, unsigned int segsX, unsigned int segsZ);
        
    void AddQuad(unsigned int lodLevel, unsigned int w, unsigned int d, unsigned int iStartVtx, float baseHeight);
    
    // startSz - Size of biggest quad
    void GenerateLodLevel(unsigned int lodLevel, unsigned int startSz, float baseHeight);
};


static unsigned int CountSideParts(unsigned int l, unsigned int lodLvl)
{
	unsigned int sz = GetLodLevelQuadStartSz(lodLvl);
	unsigned int accumulator = 0;
	while (l > 0)
	{
		while ((int)l - (int)sz < 0)
			sz = (sz > 1) ? (unsigned int)((double)sz * 0.5) : 0;

		if (sz == 0)
			break;

		accumulator++;
		l -= sz;
	}

	return accumulator;
}

static STerrainChunkCounts STerrainChunk::PrecalcCounts(unsigned int lodLevel, unsigned int segsX, unsigned int segsZ)
{
    STerrainChunkCounts counts;
    counts.quadsX = CountSideParts(segsX, lodLevel);
    counts.quadsZ = CountSideParts(segsZ, lodLevel);
    
    if (segsX * segsZ > 0)
    {
        counts.indices = counts.quadsX * counts.quadsZ;
        counts.vertices = (counts.quadsX + 1) * (counts.quadsZ + 1);
    }
    else
    {
        counts.indices = 0;
        counts.vertices = 0;
    }
    
    return counts;                   
}


void STerrainChunk::AddQuad(unsigned int lodLevel, unsigned int w, unsigned int d,
    unsigned long iStartVtx, unsigned long iStartIndex, unsigned long vtxRowShift, float baseHeight, float xoffs, float zoffs, float uoffs, float voffs)
{
    // generate vertices
    if (!pVertices[lodLevel] || !pIndics[lodLevel])
        return; // unexpected critical
        
    float quadWidth = fWidth / w;
    float quadDepth = fDepth / d;
        
    float tangent[3] = { -1.0f, 0,0 };
        
    pVertices[iStartVtx] = SVertex(xoffs, baseHeight, zoffs, 0, 1.0f, 0, tangent[0], tangent[1], tangent[2], uoffs, voffs);                                
    pVertices[iStartVtx + 1] = SVertex(xoffs + quadWidth, baseHeight, zoffs, 0, 1.0f, 0, tangent[0], tangent[1], tangent[2], uoffs + fSegUSz, voffs);
    pVertices[iStartVtx + vtxRowShift] = SVertex(xoffs, baseHeight, zoffs + quadDepth, 0, 1.0f, 0, tangent[0], tangent[1], tangent[2], uoffs, voffs + fSegVSz);
    pVertices[iStartVtx + vtxRowShift + 1]  = SVertex(xoffs + quadWidth, baseHeight, zoffs + quadDepth, 0, 1.0f, 0, tangent[0], tangent[1], tangent[2], uoffs + fSegUSz, voffs + fSegVSz);
    
    pIndices[iStartIndex] = iStartVtx;
    pIndices[iStartIndex + 1] = iStartVtx + vtxRowShift + 1;
    pIndices[iStartIndex + 2] = iStartVtx + 1;
    pIndices[iStartIndex + 3] = iStartVtx;
    pIndices[iStartIndex + 4] = iStartVtx + vtxRowShift;
    pIndices[iStartIndex + 5] = iStartVtx + vtxRowShift + 1;    
}


// startSz needs to be odd, or 1
void STerrainChunk::GenerateLodLevel(unsigned int lodLvl, unsigned int startSz /* start quad size */, float baseHeight)
{
	if (startSz % 2 > 0 && startSz > 1)
		return;

	unsigned int sz[2] = { startSz, startSz };
	unsigned int w = nSegsX, d = nSegsZ;
    
    unsigned int vtxRowShift = nSegsX + 1;

    unsigned long iCurVtx = 0;
    unsigned long iCurIdx = 0;
    
    float quadWidth = fWidth / w;
    float quadDepth = fDepth / d;

	for (unsigned int z = 0; z < nSegsZ;)
	{
		// find next quad depth
		while ((int)d - (int)sz[1] < 0)
			sz[1] = (sz[1] > 1) ? (unsigned int)((double)sz[1] * 0.5) : 0;
            
        if (sz[1] == 0)
            break; // should not happen tho, because z == cd then            

		for (unsigned int x = 0; x < nSegsX;)
		{
			// find next quad width
            while ((int)w - (int)sz[0] < 0)
                sz[0] = (sz[0] > 1) ? (unsigned int)((double)sz[0] * 0.5) : 0;
                
            if (sz[0] == 0)
                break; // should not happen tho, because x == cw then
                       
            AddQuad(lodLvl, sz[0], sz[1], iCurVtx, iCurIdx, vtxRowShift, baseHeight,
                fXOffs + quadWidth * x, fZOffs + quadDepth * z, fUOffs + quadWidth * x, fVOffs + quadDepth * z);
            
            iCurVtx += (x == nSegsX - 1) ? 2 : 1;
            iCurIdx += 6;                            
            
            w -= sz[0];
            x += sz[0];
		}
        
        d -= sz[1];
        z += sz[1];
	}
}