// ********************************************************************************************

//	SpeedPoint Terrain

// ********************************************************************************************

#pragma once

#include <SPrerequisites.h>
#include "Vector3.h"
#include "IIndexBuffer.h"
#include "IVertexBuffer.h"

SP_NMSPACE_BEG

struct S_API IGameEngine;
struct S_API ITexture;

struct S_API STerrainDescription
{
	unsigned int segsX, segsZ;
	float w, d;
	float baseHeight;
	unsigned int lodLevels;
	unsigned int chunkSegsX, chunkSegsZ;
	bool bDynamic;

	STerrainDescription()
		: bDynamic(false)
	{
	}

	STerrainDescription(const STerrainDescription& tdsc)
		: segsX(tdsc.segsX), segsZ(tdsc.segsZ),
		w(tdsc.w), d(tdsc.d),
		baseHeight(tdsc.baseHeight),
		lodLevels(tdsc.lodLevels),
		chunkSegsX(tdsc.chunkSegsX), chunkSegsZ(tdsc.chunkSegsZ),
		bDynamic(tdsc.bDynamic)
	{
	}

	inline bool IsValid() const
	{
		return (segsX > 0 && segsZ > 0 && chunkSegsX > 0 && chunkSegsZ > 0 && w > 0.0f && d > 0.0f);
	}
};

struct S_API STerrainChunkCounts
{
	unsigned int indices, vertices;
	unsigned int quadsX, quadsZ; // verts{X,Y} = quads{X,Y} + 1
};

struct S_API STerrainLodCounts
{
	unsigned int indices, vertices;
};

struct S_API STerrainChunkLodCounts
{
	unsigned long vertexOffset, indexOffset;
	unsigned long vertexCount, indexCount;
	unsigned int quadsX, quadsZ;
};

struct S_API STerrainChunk
{
	unsigned int nSegsX, nSegsZ; // on high res grid
	float fWidth, fDepth;
	float fXOffs, fZOffs;
	float fUOffs, fVOffs;
	float fSegUSz, fSegVSz;
	SIndex** pIndices; // (SIndex[nIndices])[nLodLevels]
	SVertex** pVertices; // (SVertex[nIndices])(nLodLevels)
	STerrainChunkLodCounts* pLodCounts;
	unsigned int nLodLevels;

	STerrainChunk()
		: pIndices(0),
		pVertices(0),
		pLodCounts(0)
	{
	}

	~STerrainChunk()
	{
		SP_SAFE_DELETE_ARR(pVertices, nLodLevels);
		SP_SAFE_DELETE_ARR(pIndices, nLodLevels);
		SP_SAFE_DELETE_ARR(pLodCounts, nLodLevels);

		nLodLevels = 0;
	}

	inline static unsigned int CountSideParts(unsigned int l, unsigned int lodLvl)
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
	
	inline static unsigned int GetLodLevelQuadStartSz(unsigned int lodLevel)
	{
		return (lodLevel == 0) ? 1 : lodLevel * 2;
	}	

	static STerrainChunkCounts PrecalcCounts(unsigned int lodLevel, unsigned int segsX, unsigned int segsZ)
	{
		STerrainChunkCounts counts;
		counts.quadsX = CountSideParts(segsX, lodLevel);
		counts.quadsZ = CountSideParts(segsZ, lodLevel);

		if (segsX * segsZ > 0)
		{
			counts.indices = counts.quadsX * counts.quadsZ * 6;
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
		unsigned long iStartVtx, unsigned long iStartIndex, unsigned long vtxRowShift,
		float baseHeight, float xoffs, float zoffs, float uoffs, float voffs);

	// startSz - Size of biggest quad
	void GenerateLodLevel(unsigned int lodLevel, unsigned int startSz, float baseHeight);
};


// ==================================================================================================================

// SpeedPoint Terrain Solid (abstract)
struct S_API ITerrain
{
public:
	virtual ~ITerrain() {}

	virtual bool IsInited() const = 0;

	// Initialize with the engine
	// nX and nZ is the resolution
	virtual SResult Initialize(IGameEngine* pEngine) = 0;

	// Create a planar terrain using given terrain description	
	//virtual SResult CreatePlanar(const STerrainDescription& tdsc) = 0;


	virtual void Generate(const STerrainDescription& tdsc) = 0;


	virtual SResult RecalculateNormals(unsigned int lodLevel = 0) = 0;

	virtual SResult SetColorMap(ITexture* pColorMap) = 0;
	virtual SResult SetDetailMap(ITexture* pDetailMap) = 0;

	virtual IVertexBuffer* GetVertexBuffer(unsigned int lodLevel) = 0;
	virtual IIndexBuffer* GetIndexBuffer(unsigned int lodLevel) = 0;
	
	// Summary:
	//	Flags that the terrain Constant Buffer has to be updated.
	//	Be careful to not call this during rendering of the terrain, as it might be reset before taking effect.
	virtual void RequireCBUpdate() = 0;

	/////////////////////
	///// TODO: Implement function to add / remove assigned texture
	/// this is not really clear as of now:
	///	- Do we support multiple texture using?
	///	- Or only support a single texture + color map for the whole terrain that has to be rendered outside?
	///	- Do we add a single function for runtime LOD change or do we implement that into the render function?
	///////////////////

	virtual void RequireRender() = 0;

	// Render the Terrain. Fails if pointer to SpeedPointEngine is not correct
	virtual SResult RenderTerrain( const SVector3& camPos ) = 0;

	// Clearout the terrain data BUT not the heightmap
	virtual SResult Clear( void ) = 0;
};

SP_NMSPACE_END