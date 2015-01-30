
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
    unsigned int nChunksX, nChunksZ;
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
    
    // generate chunks buffer
    pTerrainChunks = new STerrainChunk[nChunksX * nChunksZ];
        
    // Precalculate vertex and index count, create the buffers and create chunks
    pLodLevelCounts = new unsigned int[nLodLevels][2];
    pVertexBuffers = new SVertex*[nLodLevels];
    pIndexBuffers = new SIndex*[nLodLevels];
    for (unsigned int lodLvl = 0; lodLvl < nLodLevels; ++lodLvl)
    {        
        unsigned int &vtxCnt = pLodLevelCounts[lodLvl][0];
        unsigned int &idxCnt = pLodLevelCounts[lodLvl][1];
        vtxCnt = 0;
        idxCnt = 0;                
        unsigned int regChunkVtxCnt = STerrainChunk::PrecalcVertexCount(lodLvl, nChunkSegsX, nChunkSegsZ);
        unsigned int regChunkIdxCnt = STerrainChunk::PrecalcIndexCount(lodLvl, nChunkSegsX, nChunkSegsZ);
        
        vtxCnt += regChunkVtxCnt * (nChunksX - nRestChunksX) * (nChunksZ - nRestChunksZ);
        idxCnt += regChunkIdxCnt * (nChunksX - nRestChunksX) * (nChunksZ - nRestChunksZ);


        unsigned int restXChunkCnt = STerrainChunk::PrecalcVertexCount(lodLvl, nXRestChunkSegs, nChunkSegsZ);
        unsigned int restZChunkCnt = STerrainChunk::PrecalcVertexCount(lodLvl, nChunkSegsX, nZRestChunkSegs);
        unsigned int fillChunkCnt = STerrainChunk::PrecalcVertexCount(lodLvl, nXRestChunkSegs, nZRestChunkSegs);
        
        unsigned int restXChunkCnt = STerrainChunk::PrecalcIndexCount(lodLvl, nXRestChunkSegs, nChunkSegsZ);
        unsigned int restZChunkCnt = STerrainChunk::PrecalcIndexCount(lodLvl, nChunkSegsX, nZRestChunkSegs);
        unsigned int fillChunkCnt = STerrainChunk::PrecalcIndexCount(lodLvl, nXRestChunkSegs, nZRestChunkSegs);

        if (nRestChunksX > 0 || nRestChunksZ > 0)
        {
            vtxCnt += nRestChunksX * (nChunksZ - nRestChunksZ) * restXChunkCnt;                               
            vtxCnt += nRestChunksZ * (nChunksX - nRestChunksX) * restZChunkCnt;
            vtxCnt += nRestChunksX * nRestChunksZ * fillChunkCnt;                                                            
        
            idxCnt += nRestChunksX * (nChunksZ - nRestChunksZ) * restXChunkCnt;                               
            idxCnt += nRestChunksZ * (nChunksX - nRestChunksX) * restZChunkCnt;
            idxCnt += nRestChunksX * nRestChunksZ * fillChunkCnt;    
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
                if (lodLvl == 0)
                {
                    pChunk->pVertices = new SIndex*[nLodLevels];
                    pChunk->pIndices = new SIndex*[nLodLevels];
                    pChunk->nLodLevels = nLodLevels;
                    
                    pChunk->nSegsX = (bIsXRest) ? nXRestChunkSegs : nChunkSegsX;
                    pChunk->nSegsZ = (bIsZRest) ? nZRestChunkSegs : nChunkSegsZ;                                                               
                }
                
                pChunk->pVertices[lodLevel] = &pVertexBuffers[lodLvl][iCurVtxPos];
                pChunk->pIndices[lodLevel] = &pIndexBuffers[lodLvl][iCurIdxPos];                                                  
                
                pChunk->GenerateLodLevel(lodLvl, (lodLvl == 0) ? 1 : lodLvl * 2);                            
            }
        }                                                                                                                          
    }                              
}


struct STerrainChunk
{
	unsigned int nSegsX, nSegsZ; // on high res grid
    SIndex** pIndices; // (SIndex[nIndices])[nLodLevels]
    SVertex** pVertices; // (SVertex[nIndices])(nLodLevels)
    unsigned int nLodLevels;    
    
    static unsigned int PrecalcIndexCount(unsigned int lodLevel, unsigned int segsX, unsigned int segsZ);
    static unsigned int PrecalcVertexCount(unsigned int lodLevel, unsigned int segsX, unsigned int segsZ);
    void AddQuad(unsigned int lodLevel, unsigned int w, unsigned int d, unsigned int iStartVtx);
    void GenerateLodLevel(unsigned int lodLevel, unsigned int startSz);
};


static unsigned int STerrainChunk::PrecalcIndexCount(unsigned int lodLevel, unsigned int segsX, unsigned int segsZ)
{
}

static unsigned int STerrainChunk::PrecalcVertexCount(unsigned int lodLevel, unsigned int segsX, unsigned int segsZ)
{
}


void STerrainChunk::AddQuad(unsigned int lodLevel, unsigned int w, unsigned int d, unsigned long iStartVtx)
{
    pIndices[curIdx] = 
}


// startSz needs to be odd, or 1
void STerrainChunk::GenerateLodLevel(unsigned int lodLvl, unsigned int startSz /* start quad size */)
{
	if (startSz % 2 > 0 && startSz > 1)
		return;

	unsigned int sz[2] = { startSz, startSz };
	unsigned int w = nSegsX, d = nSegsZ;

	for (unsigned int z = 0; z < cd;)
	{
		// find next quad depth
		while (d - sz[1] < 0)
			sz[1] = (sz[1] > 1) ? (unsigned int)((double)sz[1] * 0.5) : 0;
            
        if (sz[1] == 0)
            break; // should not happen tho, because z == cd then            

		for (unsigned int x = 0; x < cw;)
		{
			// find next quad width
            while (w - sz[0] < 0)
                sz[0] = (sz[0] > 1) ? (unsigned int)((double)sz[0] * 0.5) : 0;
                
            if (sz[0] == 0)
                break; // should not happen tho, because x == cw then
            
            addQuad()                            
            
            w -= sz[0];
            x += sz[0];
		}
        
        d -= sz[1];
        z += sz[1];
	}
}