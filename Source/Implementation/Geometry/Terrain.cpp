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
	if (!IS_VALID_PTR(m_pEngine) || !tdsc.IsValid())
		return S_INVALIDPARAM;
	

	unsigned long nVertices = (tdsc.nX + 1) * (tdsc.nZ + 1);
	unsigned long nIndices[2];
	nIndices[0] = tdsc.nX * tdsc.nZ * 6;

	SVertex* pVertices = new SVertex[nVertices];
	SIndex* pIndices[2];
	pIndices[0] = new SIndex[nIndices[0]]; // lod Level 0	

	//initGeom.vertexUsage = eGEOMUSE_MODIFY_FREQUENTLY;

	float diffX = tdsc.width / (float)tdsc.nX,
		diffZ = tdsc.depth / (float)tdsc.nZ;

	float ratioU = 1.0f / (float)(tdsc.nX),
		ratioV = 1.0f / (float)(tdsc.nZ);

	m_fDMTexScaleU = diffX;
	m_fDMTexScaleV = diffZ;

	float posOffsetX = tdsc.width * 0.5f, posOffsetZ = tdsc.depth * 0.5f;
	 
	//   6-----7-----8
	//   |   / |   / |
	//   | /   | /   |
	//   3-----4-----5
	//   | 1 / | 3 / |
	//   | / 0 | / 2 |
	//   0-----1-----2
	//	

	// Create Vertices and Highest LOD level
	for (unsigned int z = 0; z <= tdsc.nZ; ++z)
	{
		for (unsigned int x = 0; x <= tdsc.nX; ++x)
		{
			unsigned int iVtxCur = z * (tdsc.nX + 1) + x;
			float h = tdsc.baseHeight + (sinf(8.0f * SP_PI * ratioU * x) + sinf(8.0f * SP_PI * ratioV * z)) * 0.7f;
			pVertices[iVtxCur] = SVertex(
				x * diffX - posOffsetX, h, z * diffZ - posOffsetZ,		// position
				0.0f, 1.0f, 0.0f,				// normal
				-1.0f, 0.0f, 0.0f,				// tangent
				x * ratioU, z * ratioV);			// texcoord [0;1]
			
			if (x < tdsc.nX && z < tdsc.nZ)
			{
				unsigned int idxCur = (z * tdsc.nX + x) * 6;
				pIndices[0][idxCur] = iVtxCur;
				pIndices[0][idxCur + 1] = iVtxCur + 1 + (tdsc.nX + 1);
				pIndices[0][idxCur + 2] = iVtxCur + 1;
				pIndices[0][idxCur + 3] = iVtxCur;
				pIndices[0][idxCur + 4] = iVtxCur + (tdsc.nX + 1);
				pIndices[0][idxCur + 5] = iVtxCur + 1 + (tdsc.nX + 1);
			}
		}
	}			



	// ======================== SECOND LOD LEVEL ====================================
	
	//	One Chunk:
	//	LOD 0
	//
	// o-----o-----o-----o-----o-----o-----o-----o-----o-----o
	// |   / |   / |   / |   / |   / |   / |   / |   / |   / |
	// | /   | /   | /   | /   | /   | /   | /   | /   | /   |
	// o-----o-----o-----o-----o-----o-----o-----o-----o-----o
	// |   / |   / |   / |   / |   / |   / |   / |   / |   / |
	// | /   | /   | /   | /   | /   | /   | /   | /   | /   |
	// o-----o-----o-----o-----o-----o-----o-----o-----o-----o
	// |   / |   / |   / |   / |   / |   / |   / |   / |   / |
	// | /   | /   | /   | /   | /   | /   | /   | /   | /   |
	// o-----o-----o-----o-----o-----o-----o-----o-----o-----o
	// |   / |   / |   / |   / |   / |   / |   / |   / |   / |
	// | /   | /   | /   | /   | /   | /   | /   | /   | /   |
	// o-----o-----o-----o-----o-----o-----o-----o-----o-----o
	// |   / |   / |   / |   / |   / |   / |   / |   / |   / |
	// | /   | /   | /   | /   | /   | /   | /   | /   | /   |
	// o-----o-----o-----o-----o-----o-----o-----o-----o-----o
	// |   / |   / |   / |   / |   / |   / |   / |   / |   / |
	// | /   | /   | /   | /   | /   | /   | /   | /   | /   |
	// o-----o-----o-----o-----o-----o-----o-----o-----o-----o
	// |   / |   / |   / |   / |   / |   / |   / |   / |   / |
	// | /   | /   | /   | /   | /   | /   | /   | /   | /   |
	// o-----o-----o-----o-----o-----o-----o-----o-----o-----o
	// |   / |   / |   / |   / |   / |   / |   / |   / |   / |
	// | /   | /   | /   | /   | /   | /   | /   | /   | /   |
	// o-----o-----o-----o-----o-----o-----o-----o-----o-----o
	// |   / |   / |   / |   / |   / |   / |   / |   / |   / |
	// | /   | /   | /   | /   | /   | /   | /   | /   | /   |
	// o-----o-----o-----o-----o-----o-----o-----o-----o-----o

	//
	//
	//	LOD 1
	//
	// o-----o-----o-----o-----o-----o-----o-----o-----o-----o
	// |   / |   / |   / |   / |   / |   / |   / |   / |   / |
	// | /   | /   | /   | /   | /   | /   | /   | /   | /   |
	// o-----o-----o-----o-----o-----o-----o-----o-----o-----o
	// |         / |         / |         / |         / |   / |
	// |       /   |       /   |       /   |       /   | /   |d
	// o     o     o     o     o     o     o     o     o-----o
	// |   /       |   /       |   /       |   /       |   / |
	// | /         | /         | /         | /         | /   |
	// o-----o-----o-----o-----o-----o-----o-----o-----o-----o
	// |         / |         / |         / |         / |   / |
	// |       /   |       /   |       /   |       /   | /   |
	// o     o     o     o     o     o     o     o     o-----o
	// |   /       |   /       |   /       |   /       |   / |
	// | /         | /         | /         | /         | /   |
	// o-----o-----o-----o-----o-----o-----o-----o-----o-----o
	// |         / |         / |         / |         / |   / |
	// |       /   |       /   |       /   |       /   | /   |
	// o     o     o     o     o     o     o     o     o-----o
	// |   /       |   /       |   /       |   /       |   / |
	// | /         | /         | /         | /         | /   |
	// o-----o-----o-----o-----o-----o-----o-----o-----o-----o
	// |         / |         / |         / |         / |   / |
	// |       /   |       /   |       /   |       /   | /   |
	// o     o     o     o     o     o     o     o     o-----o
	// |   /       |   /       |   /       |   /       |   / |
	// | /         | /         | /         | /         | /   |
	// o-----o-----o-----o-----o-----o-----o-----o-----o-----o




	//				13 x 13 Segments
	//				Using 5x5 chunks
	//
	// o-----o-----o-----o-----o-----o  -----o-----o-----o-----o-----o  -----o-----o-----o
	// |   / |   / |   / |   / |   / |     / |   / |   / |   / |   / |     / |   / |   / |
	// | /   | /   | /   | /   | /   |   /   | /   | /   | /   | /   |   /   | /   | /   |
	// o-----o-----o-----o-----o-----o  -----o-----o-----o-----o-----o  -----o-----o-----o
	// |         / |         / |   / |           / |         / |   / |           / |   / |
	// |       /   |       /   | /   |         /   |       /   | /   |         /   | /   |
	// o     o     o     o     o-----o       o     o     o     o-----o       o     o-----o
	// |   /       |   /       |   / |     /       |   /       |   / |     /       |   / |
	// | /         | /         | /   |   /         | /         | /   |   /         | /   |

	// o-----o-----o-----o-----o-----o  -----o-----o-----o-----o-----o  -----o-----o-----o
	// |   / |   / |   / |   / |   / |     / |   / |   / |   / |   / |     / |   / |   / |
	// | /   | /   | /   | /   | /   |   /   | /   | /   | /   | /   |   /   | /   | /   |
	// o-----o-----o-----o-----o-----o  -----o-----o-----o-----o-----o  -----o-----o-----o
	// |         / |         / |   / |           / |         / |   / |           / |   / |
	// |       /   |       /   | /   |         /   |       /   | /   |         /   | /   |
	// o     o     o     o     o-----o       o     o     o     o-----o       o     o-----o
	// |   /       |   /       |   / |     /       |   /       |   / |     /       |   / |
	// | /         | /         | /   |   /         | /         | /   |   /         | /   |
	// o-----o-----o-----o-----o-----o  -----o-----o-----o-----o-----o  -----o-----o-----o
	// |         / |         / |   / |           / |         / |   / |           / |   / |
	// |       /   |       /   | /   |         /   |       /   | /   |         /   | /   |
	// o     o     o     o     o-----o       o     o     o     o-----o       o     o-----o
	// |   /       |   /       |   / |     /       |   /       |   / |     /       |   / |
	// | /         | /         | /   |   /         | /         | /   |   /         | /   |

	// o-----o-----o-----o-----o-----o  -----o-----o-----o-----o-----o  -----o-----o-----o	
	// |18 / |20 / |21 / |23 / |26 / |  44 / |46 / |48 / |50 / |52 / |     / |   / |   / |
	// | /17 | /19 | /22 | /24 | /25 |   / 43| / 45| / 47| / 49| / 51|   /   | /   | /   |
	// o-----o-----o-----o-----o-----o  -----o-----o-----o-----o-----o  -----o-----o-----o
	// |         / |         / |16 / |           / |         / |42 / |           / |   / |
	// | 10    /   | 12    /   | /15 |   36    /   | 38    /   | / 41|         /   | /   |
	// o     o     o     o     o-----o       o     o     o     o-----o       o     o-----o
	// |   /   9   |   /   11  |14 / |     /       |   /   37  |40 / |     /       |   / |
	// | /         | /         | /13 |   /    35   | /         | / 39|   /         | /   |
	// o-----o-----o-----o-----o-----o  -----o-----o-----o-----o-----o  -----o-----o-----o
	// |         / |         / |8  / |           / |         / |34 / |           / |   / |
	// | 2     /   |  4    /   | / 7 |   28    /   | 30    /   | / 33|    54   /   | /   |
	//14    15    16    17    18----19      20    21    22    23----24      25    26----27
	// |   /    1  |   /    3  |6  / |     /  27   |   /   29  |32 / |     /  53   |   / |
	// | /         | /         | / 5 |   /         | /         | / 31|   /         | /   |
	// 0-----1-----2-----3-----4-----5  -----6-----7-----8-----9----10  ----11----12----13
	//
	// nRestX = 3, nRestZ = 3	
	// nRestBigQuadsX = 1, nRestBigQuadsZ = 1
	// nRestQuadsX = 1, nRestQuadsZ = 1
	// nRestFillBigQuads = 1
	// nRestBigQuads = 8 + nRestFillBigQuads = 9	
	// nChunkRest = 1

	// Vertex buffer: stays continoues, as is
	// Index buffer: 0 - (13*13 * 6 - 1)

	// number of faces that cannot be put into a chunk
	// may range from 0 to tdsc.nChunkSideSegments - 1
	unsigned int nRestX = tdsc.nX % tdsc.nChunkSideSegments,
		nRestZ = tdsc.nZ % tdsc.nChunkSideSegments;

	unsigned int nRestBigQuadsX = (nRestX - (nRestX % 2)) * 0.5f,
		nRestBigQuadsZ = (nRestZ - (nRestZ % 2)) * 0.5f;

	// small quads in the rest in one dimension. The rest of the rest.
	unsigned int nRestQuadsX = nRestX - (nRestBigQuadsX * 2),
		nRestQuadsZ = nRestZ - (nRestBigQuadsZ * 2);



	// check for rest face inside a single chunk
	unsigned int nChunkRest = tdsc.nChunkSideSegments % 2;

	// floor(tdsc.nX / tdsc.nChunkSideSegments)
	unsigned int nChunksX = (tdsc.nX - nRestX) / tdsc.nChunkSideSegments,
		nChunksZ = (tdsc.nZ - nRestZ) / tdsc.nChunkSideSegments;

	unsigned int nChunkBigQuadsOneSide = (tdsc.nChunkSideSegments - nChunkRest) * 0.5f;
	unsigned int nChunkBigQuads = nChunkBigQuadsOneSide * nChunkBigQuadsOneSide;

	unsigned int nChunkRestQuadsOneSide = (2 * nChunkRest) * nChunkBigQuadsOneSide;
	unsigned int nChunkRestQuads = nChunkBigQuadsOneSide * 2 + nChunkRest * nChunkRest;	

	m_nChunkBigQuads = nChunkBigQuads;
	m_nChunkSmallQuads = tdsc.nChunkSideSegments * tdsc.nChunkSideSegments - nChunkBigQuads * 4; // 1 big quad = 4 small quads


	// Calculate counts
	unsigned int nRestFillBigQuads = nRestBigQuadsX * nRestBigQuadsZ;	
	unsigned int nRestBigQuadsX = nRestBigQuadsX * nChunkBigQuadsOneSide * nChunksZ,
		nRestBigQuadsZ = nRestBigQuadsZ * nChunkBigQuadsOneSide * nChunksX;
	unsigned int nRestBigQuads = nRestBigQuadsX * nRestBigQuadsZ + nRestFillBigQuads;	

	// calc all rest quads then subtract big quads. One big quad covers 4 small quads.
	unsigned int nRestQuads = (nRestX + nRestZ) + (nRestX * nRestZ) - (nRestBigQuads * 4);


	unsigned long nBigQuads = nRestBigQuads + nChunkBigQuads;
	unsigned long nQuads = nRestQuads + nChunkRestQuads;

	nIndices[1] = (nBigQuads + nQuads) * 6;
	pIndices[1] = new SIndex[nIndices[1]];
	
	// vertex offset for the rest row, where chnkZ == nChunksZ - 1
	unsigned long zChnkRestVtxOffset = (nChunksZ * tdsc.nChunkSideSegments) * (tdsc.nX + 1);
	unsigned long fillChunkVtxOffset = zChnkRestVtxOffset + (nChunksX * tdsc.nChunkSideSegments);

	unsigned long iCurIndex = 0;

	unsigned long rowShift = tdsc.nX + 1;

	for (unsigned int chnkZ = 0; chnkZ < nChunksZ; ++chnkZ)
	{
		// vertex offset for the first vertex in the current chunk-row
		unsigned long vtxZOffset = (chnkZ * tdsc.nChunkSideSegments) * (tdsc.nX + 1);		

		for (unsigned int chnkX = 0; chnkX < nChunksX; ++chnkX)
		{
			unsigned long vtxXOffset = chnkX * tdsc.nChunkSideSegments;
			unsigned long iVtx = vtxZOffset + vtxXOffset;

			// Offset to the first vertex in the rest row (iBigQuadZ == nChunkBigQuadsOneSide - 1) inside one chunk
			unsigned long zRestVtxOffset = tdsc.nChunkSideSegments * (tdsc.nChunkSideSegments + 1);

			// Add one chunk			
			for (unsigned int iBigQuadZ = 0; iBigQuadZ < nChunkBigQuadsOneSide; ++iBigQuadZ)
			{
				for (unsigned int iBigQuadX = 0; iBigQuadX < nChunkBigQuadsOneSide; ++iBigQuadX)
				{
					// calculate current position on highpoly-grid in the current chunk
					unsigned int iChnkVtx = iVtx + (iBigQuadZ * 2) * (tdsc.nX + 1) + iBigQuadX * 2;

					// o-----o-----o-----o-----o-----o
					// |18 / |20 / |21 / |23 / |26 / |
					// | /17 | /19 | /22 | /24 | /25 |
					// o-----o-----o-----o-----o-----o
					// |         / |         / |16 / |
					// | 10    /   | 12    /   | /15 |
					// o     o     o     o     o-----o
					// |   /   9   |   /   11  |14 / |
					// | /         | /         | /13 |
					// o-----o-----o-----o-----o-----o
					// |         / |         / |8  / |
					// | 2     /   |  4    /   | / 7 |
					//14    15    16    17    18----19
					// |   /    1  |   /    3  |6  / |
					// | /         | /         | / 5 |
					// 0-----1-----2-----3-----4-----5
                
					// Add chunk-innner big quad
					pIndices[1][iCurIndex] = iChnkVtx;
					pIndices[1][iCurIndex + 1] = iChnkVtx + 2 * (tdsc.nX + 1) + 2;
					pIndices[1][iCurIndex + 2] = iChnkVtx + 2;
					pIndices[1][iCurIndex + 3] = iChnkVtx;
					pIndices[1][iCurIndex + 4] = iChnkVtx + 2 * (tdsc.nX + 1);
					pIndices[1][iCurIndex + 5] = iChnkVtx + 2 * (tdsc.nX + 1) + 2;
					iCurIndex += 6;					

					if (nChunkRest > 0)
					{
						bool bIsChnkXRest = (iBigQuadX == nChunkBigQuadsOneSide - 1),
							bIsChnkZRest = (iBigQuadZ == nChunkBigQuadsOneSide - 1);

						if (bIsChnkXRest && bIsChnkZRest)
						{
							// add fill small quad of chunk:

							// note: need to start at the upper right corner of the current big quad
							pIndices[1][iCurIndex] = iChnkVtx + 2 + 2 * (tdsc.nX + 1);
							pIndices[1][iCurIndex + 1] = pIndices[1][iCurIndex] + (tdsc.nX + 1) + 1;
							pIndices[1][iCurIndex + 2] = pIndices[1][iCurIndex] + 1;
							pIndices[1][iCurIndex + 3] = pIndices[1][iCurIndex];
							pIndices[1][iCurIndex + 4] = pIndices[1][iCurIndex] + (tdsc.nX + 1);
							pIndices[1][iCurIndex + 5] = pIndices[1][iCurIndex] + (tdsc.nX + 1) + 1;
							iCurIndex += 6;
						}
						else if (bIsChnkXRest)
						{
							// add two right rest small quads of chunk

							// start at bottom right corner of current bigquad
							pIndices[1][iCurIndex] = iChnkVtx + 2;
							pIndices[1][iCurIndex + 1] = iChnkVtx + 2 + (tdsc.nX + 1) + 1;
							pIndices[1][iCurIndex + 2] = iChnkVtx + 2 + 1;
							pIndices[1][iCurIndex + 3] = iChnkVtx + 2;
							pIndices[1][iCurIndex + 4] = iChnkVtx + 2 + (tdsc.nX + 1);
							pIndices[1][iCurIndex + 5] = iChnkVtx + 2 + (tdsc.nX + 1) + 1;
							iCurIndex += 6;

							for (unsigned long ii = 0; ii < 6; ++ii)
								pIndices[1][iCurIndex + ii] = pIndices[1][iCurIndex + ii - 6] + rowShift;

							iCurIndex += 6;
						}
						else if (bIsChnkZRest)
						{
							// add two top small rest quads of chunk, left first using zRestVtxOffset							
							pIndices[1][iCurIndex] = zRestVtxOffset + iBigQuadX * 2;
							pIndices[1][iCurIndex + 1] = pIndices[1][iCurIndex] + rowShift + 1;
							pIndices[1][iCurIndex + 2] = pIndices[1][iCurIndex] + 1;
							pIndices[1][iCurIndex + 3] = pIndices[1][iCurIndex];
							pIndices[1][iCurIndex + 4] = pIndices[1][iCurIndex] + rowShift;
							pIndices[1][iCurIndex + 5] = pIndices[1][iCurIndex] + rowShift + 1;
							iCurIndex += 6;

							for (unsigned long ii = 0; ii < 6; ++ii)
								pIndices[1][iCurIndex + ii] = pIndices[1][iCurIndex + ii - 6] + 1;

							iCurIndex += 6;
						}
					}
				}				
			}


			bool bIsZRest = chnkZ == nChunksZ - 1,
				bIsXRest = chnkX == nChunksX - 1;

			if (bIsXRest && bIsZRest)
			{
				// Add the fill section
				for (unsigned int iFillBigQuadZ = 0; iFillBigQuadZ < nRestBigQuadsZ; ++iFillBigQuadZ)
				{
					for (unsigned int iFillBigQuadX = 0; iFillBigQuadX < nRestBigQuadsX; ++iFillBigQuadX)
					{
						// compute position of the current point of the fill section on the high-poly grid of the terrain
						unsigned long iCurFillVtx = fillChunkVtxOffset + (iFillBigQuadZ * 2) * rowShift + iFillBigQuadX * 2;

						// add big quad
						pIndices[1][iCurIndex] = iCurFillVtx;
						pIndices[1][iCurIndex + 1] = iCurFillVtx + 2 * rowShift + 2;
						pIndices[1][iCurIndex + 2] = iCurFillVtx + 2;
						pIndices[1][iCurIndex + 3] = iCurFillVtx;
						pIndices[1][iCurIndex + 4] = iCurFillVtx + 2 * rowShift;
						pIndices[1][iCurIndex + 5] = iCurFillVtx + 2 * rowShift + 2;
						iCurIndex += 6;

						// add rest small quads
						if (nRestBigQuadsX * nRestBigQuadsZ > 0)
						{
							bool bFillRestX = (iFillBigQuadX == nRestBigQuadsX - 1),								
								bFillRestZ = (iFillBigQuadZ == nRestBigQuadsZ - 1);

							if (bFillRestX && bFillRestZ)
							{
								// add fill section fill small quad. iCurFillVtx is at top right big quad
								pIndices[1][iCurIndex] = iCurFillVtx + 2 * rowShift + 2;
								pIndices[1][iCurIndex + 1] = pIndices[1][iCurIndex] + rowShift + 1;
								pIndices[1][iCurIndex + 2] = pIndices[1][iCurIndex] + 1;
								pIndices[1][iCurIndex + 3] = pIndices[1][iCurIndex];
								pIndices[1][iCurIndex + 4] = pIndices[1][iCurIndex] + rowShift;
								pIndices[1][iCurIndex + 5] = pIndices[1][iCurIndex] + rowShift + 1;
								iCurIndex += 6;
							}
							else if (bFillRestX)
							{
								// add two right fill rest small quads
								pIndices[1][iCurIndex] = iCurFillVtx + 2;
								pIndices[1][iCurIndex + 1] = pIndices[1][iCurIndex] + rowShift + 1;
								pIndices[1][iCurIndex + 2] = pIndices[1][iCurIndex] + 1;
								pIndices[1][iCurIndex + 3] = pIndices[1][iCurIndex];
								pIndices[1][iCurIndex + 4] = pIndices[1][iCurIndex] + rowShift;
								pIndices[1][iCurIndex + 5] = pIndices[1][iCurIndex] + rowShift + 1;
								iCurIndex += 6;

								for (unsigned long ii = 0; ii < 6; ++ii)
									pIndices[1][iCurIndex + ii] = pIndices[1][iCurIndex + ii - 6] + rowShift;

								iCurIndex += 6;
							}
							else if (bFillRestZ)
							{
								// add two top fill rest small quads
								pIndices[1][iCurIndex] = iCurFillVtx + 2 * rowShift;
								pIndices[1][iCurIndex + 1] = pIndices[1][iCurIndex] + rowShift + 1;
								pIndices[1][iCurIndex + 2] = pIndices[1][iCurIndex] + 1;
								pIndices[1][iCurIndex + 3] = pIndices[1][iCurIndex];
								pIndices[1][iCurIndex + 4] = pIndices[1][iCurIndex] + rowShift;
								pIndices[1][iCurIndex + 5] = pIndices[1][iCurIndex] + rowShift + 1;
								iCurIndex += 6;

								for (unsigned long ii = 0; ii < 6; ++ii)
									pIndices[1][iCurIndex + ii] = pIndices[1][iCurIndex + ii - 6] + 1;

								iCurIndex += 6;
							}
						}
					} // iFillBigQuadX
				} // iFillBigQuadZ
			}
			else if (bIsXRest)
			{
				// Index of the vertex of the bottom left corner of the right rest.
				// Use current chunk offset and add the width of the chunk. On high poly grid.
				unsigned long iCurChnkXRestOffset = iVtx + tdsc.nChunkSideSegments;

				for (unsigned int iXRestBigQuadZ = 0; iXRestBigQuadZ < nChunkRestQuadsOneSide; ++iXRestBigQuadZ)
				{
					for (unsigned int iXRestBigQuadX = 0; iXRestBigQuadX < nRestBigQuadsX; ++iXRestBigQuadX)
					{
						// calculate position of the current right-rest big quad on high poly grid
						unsigned long iCurXRestBigQuadVtx = iCurChnkXRestOffset + iXRestBigQuadZ * (2 * rowShift) + iXRestBigQuadX * 2;

						if (nChunkRest * nRestQuadsX > 0)
						{
							bool bIsXRest = (iXRestBigQuadX == nRestBigQuadsX - 1),
								bIsZRest = (iXRestBigQuadZ == nRestBigQuadsZ - 1);

							if (bIsXRest && bIsZRest)
							{
								// add the fill small quad at the top right corner of a right rest section
								// iCurChnkXRestOffset is at to topmost rightmost bigquad of the right rest section

								pIndices[1][iCurIndex] = iCurChnkXRestOffset + 2 * rowShift + 2;
								pIndices[1][iCurIndex + 1] = pIndices[1][iCurIndex] + rowShift + 1;								
								pIndices[1][iCurIndex + 2] = pIndices[1][iCurIndex] + 1;
								pIndices[1][iCurIndex + 3] = pIndices[1][iCurIndex];
								pIndices[1][iCurIndex + 4] = pIndices[1][iCurIndex] + rowShift;
								pIndices[1][iCurIndex + 5] = pIndices[1][iCurIndex] + rowShift + 1;
								iCurIndex += 6;
							}
							else if (bIsXRest)
							{
								// add two right rest small quads of right-rest-section

								pIndices[1][iCurIndex] = iCurChnkXRestOffset + 2;
								pIndices[1][iCurIndex + 1] = pIndices[1][iCurIndex] + rowShift + 1;
								pIndices[1][iCurIndex + 2] = pIndices[1][iCurIndex] + 1;
								pIndices[1][iCurIndex + 3] = pIndices[1][iCurIndex];
								pIndices[1][iCurIndex + 4] = pIndices[1][iCurIndex] + rowShift;
								pIndices[1][iCurIndex + 5] = pIndices[1][iCurIndex] + rowShift + 1;
								iCurIndex += 6;

								for (unsigned long ii = 0; ii < 6; ++ii)
									pIndices[1][iCurIndex + ii] = pIndices[1][iCurIndex + ii - 6] + rowShift;

								iCurIndex += 6;
							}
							else if (bIsZRest)
							{
								// add two top rest small quads of right-rest-section

								pIndices[1][iCurIndex] = iCurChnkXRestOffset + 2 * rowShift;
								pIndices[1][iCurIndex + 1] = pIndices[1][iCurIndex] + rowShift + 1;
								pIndices[1][iCurIndex + 2] = pIndices[1][iCurIndex] + 1;
								pIndices[1][iCurIndex + 3] = pIndices[1][iCurIndex];
								pIndices[1][iCurIndex + 4] = pIndices[1][iCurIndex] + rowShift;
								pIndices[1][iCurIndex + 5] = pIndices[1][iCurIndex] + rowShift + 1;
								iCurIndex += 6;

								for (unsigned long ii = 0; ii < 6; ++ii)
									pIndices[1][iCurIndex + ii] = pIndices[1][iCurIndex + ii - 6] + 1;

								iCurIndex += 6;
							}
						}						
					} // iXRestBigQuadX
				} // iXRestBigQuadZ
			}
			else if (bIsZRest)
			{
				// Index of the vertex of the bottom left corner of the right rest.
				// Use current chunk offset and add the width of the chunk. On high poly grid.
				unsigned long iCurChnkZRestOffset = iVtx + tdsc.nChunkSideSegments * rowShift;

				for (unsigned int iZRestBigQuadZ = 0; iZRestBigQuadZ < nChunkRestQuadsOneSide; ++iZRestBigQuadZ)
				{
					for (unsigned int iZRestBigQuadX = 0; iZRestBigQuadX < nRestBigQuadsX; ++iZRestBigQuadX)
					{
						// calculate position of the current right-rest big quad on high poly grid
						unsigned long iCurZRestBigQuadVtx = iCurChnkZRestOffset + iZRestBigQuadZ * (2 * rowShift) + iZRestBigQuadX * 2;

						if (nChunkRest * nRestQuadsX > 0)
						{
							bool bIsXRest = (iZRestBigQuadX == nRestBigQuadsX - 1),
								bIsZRest = (iZRestBigQuadZ == nRestBigQuadsZ - 1);

							if (bIsXRest && bIsZRest)
							{
								// add the fill small quad at the top right corner of a right rest section
								// iCurChnkXRestOffset is at to topmost rightmost bigquad of the right rest section

								pIndices[1][iCurIndex] = iCurChnkZRestOffset + 2 * rowShift + 2;
								pIndices[1][iCurIndex + 1] = pIndices[1][iCurIndex] + rowShift + 1;
								pIndices[1][iCurIndex + 2] = pIndices[1][iCurIndex] + 1;
								pIndices[1][iCurIndex + 3] = pIndices[1][iCurIndex];
								pIndices[1][iCurIndex + 4] = pIndices[1][iCurIndex] + rowShift;
								pIndices[1][iCurIndex + 5] = pIndices[1][iCurIndex] + rowShift + 1;
								iCurIndex += 6;
							}
							else if (bIsXRest)
							{
								// add two right rest small quads of right-rest-section

								pIndices[1][iCurIndex] = iCurChnkZRestOffset + 2;
								pIndices[1][iCurIndex + 1] = pIndices[1][iCurIndex] + rowShift + 1;
								pIndices[1][iCurIndex + 2] = pIndices[1][iCurIndex] + 1;
								pIndices[1][iCurIndex + 3] = pIndices[1][iCurIndex];
								pIndices[1][iCurIndex + 4] = pIndices[1][iCurIndex] + rowShift;
								pIndices[1][iCurIndex + 5] = pIndices[1][iCurIndex] + rowShift + 1;
								iCurIndex += 6;

								for (unsigned long ii = 0; ii < 6; ++ii)
									pIndices[1][iCurIndex + ii] = pIndices[1][iCurIndex + ii - 6] + rowShift;

								iCurIndex += 6;
							}
							else if (bIsZRest)
							{
								// add two top rest small quads of right-rest-section

								pIndices[1][iCurIndex] = iCurChnkZRestOffset + 2 * rowShift;
								pIndices[1][iCurIndex + 1] = pIndices[1][iCurIndex] + rowShift + 1;
								pIndices[1][iCurIndex + 2] = pIndices[1][iCurIndex] + 1;
								pIndices[1][iCurIndex + 3] = pIndices[1][iCurIndex];
								pIndices[1][iCurIndex + 4] = pIndices[1][iCurIndex] + rowShift;
								pIndices[1][iCurIndex + 5] = pIndices[1][iCurIndex] + rowShift + 1;
								iCurIndex += 6;

								for (unsigned long ii = 0; ii < 6; ++ii)
									pIndices[1][iCurIndex + ii] = pIndices[1][iCurIndex + ii - 6] + 1;

								iCurIndex += 6;
							}
						}
					} // iZRestBigQuadX
				} // iZRestBigQuadZ
			}
		} // chnkX		
	} // chnkZ




	// Fill vertex buffer

	if (Failure(m_pEngine->GetResources()->AddVertexBuffer(&m_pVertexBuffer)))
		return EngLog(S_ERROR, m_pEngine, "Failed add vertex buffer for terrain!");

	if (Failure(m_pVertexBuffer->Initialize(m_pEngine, m_pEngine->GetRenderer(), tdsc.bDynamic ? eVBUSAGE_DYNAMIC_RARE : eVBUSAGE_STATIC, 0, 0)))
		return EngLog(S_ERROR, m_pEngine, "Failed init Vertex Buffer for Terrain!");

	if (Failure(m_pVertexBuffer->Fill(pVertices, nVertices)))
		return EngLog(S_ERROR, m_pEngine, "Failed fill vertex buffer for terrain!");



	// Fill index buffers

	for (unsigned int iIndexBuffer = 0; iIndexBuffer < 2; ++iIndexBuffer)
	{
		if (Failure(m_pEngine->GetResources()->AddIndexBuffer(&m_pIndexBuffer[iIndexBuffer])))
			return EngLog(S_ERROR, m_pEngine, "Failed add index buffer #%d for terrain!", iIndexBuffer);

		if (Failure(m_pIndexBuffer[iIndexBuffer]->Initialize(m_pEngine, m_pEngine->GetRenderer(), eIBUSAGE_STATIC, nIndices[iIndexBuffer], 0)))
			return EngLog(S_ERROR, m_pEngine, "Failed init index buffer #%d for terrain!", iIndexBuffer);

		if (Failure(m_pIndexBuffer[iIndexBuffer]->Fill(pIndices[iIndexBuffer], nIndices[iIndexBuffer], false)))
			return EngLog(S_ERROR, m_pEngine, "Failed fill index buffer #%d for terrain!", iIndexBuffer);
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