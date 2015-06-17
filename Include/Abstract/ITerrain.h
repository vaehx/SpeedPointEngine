// ********************************************************************************************

//	SpeedPoint Terrain

// ********************************************************************************************

#pragma once

#include <SPrerequisites.h>
#include "MathGeom.h"
//#include "Vector3.h"
#include "Vector2.h"
#include "IIndexBuffer.h"
#include "IVertexBuffer.h"
#include "Camera.h"

SP_NMSPACE_BEG

struct S_API IGameEngine;
struct S_API ITexture;

struct S_API STerrainChunk;

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




//===================================================================================================
//		Terrain Chunk
//===================================================================================================

enum ESide
{
	eSIDE_TOP,
	eSIDE_RIGHT,
	eSIDE_BOTTOM,
	eSIDE_LEFT
};

struct STerrainChunkBorder
{
	ESide side;
	STerrainChunk* neighbor;

	STerrainChunkBorder()
		: neighbor(0)
	{
	}

	STerrainChunkBorder(ESide s, STerrainChunk* n)
		: side(s),
		neighbor(n)
	{
	}

	STerrainChunkBorder(const STerrainChunkBorder& rhs)
		: side(rhs.side),
		neighbor(rhs.neighbor)
	{
	}

	~STerrainChunkBorder()
	{
		neighbor = 0;
	}
};

struct STerrainChunkCorner
{
	STerrainChunk* neighbor1;
	STerrainChunk* neighbor2;
	ESide side1;
};


struct S_API STerrainChunk
{
	float fSize; // per side
	unsigned int chunkSegments; // per side, whole chunk (not only the core)
	unsigned int quadSegs;
	unsigned int chunkQuads, coreQuads;
	unsigned int curLodLevel;
	unsigned int cx, cz;

	ILINE void CreateCoreBasedOnItsLODLevel(SLargeIndex* pIB, unsigned long startVtxIdx, unsigned long vtxRowShift, unsigned long& idxAccum);
	ILINE void AddCoreQuad(SLargeIndex* pIB, unsigned long startVtxIdx, unsigned long vtxRowShift, unsigned long& idxAccum);
	ILINE void CreateBorder(SLargeIndex* pIB, ESide side, unsigned long startVtxIdx, const STerrainChunk* neighbor, unsigned long vtxRowShift, unsigned long& idxAccum);
	ILINE void CreateCorner(const STerrainChunkCorner& border, SLargeIndex* pIB, unsigned long startVtxIdx, unsigned long vtxRowShift, unsigned long& idxAccum);

	ILINE unsigned int DetermineLODLevelByView(const SCamera* pCamera, unsigned int nLodLevels, float chunkStepDist)
	{
		// assuming fSize is the same for each chunk:
		float fXOffs = cx * fSize;
		float fZOffs = cz * fSize;
		
		SVector3 chunkCenter(fXOffs + 0.5f * fSize, pCamera->position.y, fZOffs + 0.5f * fSize);
		float camChunkDist = (pCamera->position - chunkCenter).Length();

		unsigned int lodLvl = (unsigned int)floor(camChunkDist / chunkStepDist);
		if (lodLvl >= nLodLevels)
			lodLvl = nLodLevels - 1;

		return lodLvl;
	}
};


// ==================================================================================================================

struct S_API STerrainLayer
{
	ITexture* pAlphaMask;
	ITexture* pDetailMap;

	STerrainLayer()
		: pAlphaMask(0), pDetailMap(0) {}

	STerrainLayer(const STerrainLayer& layer)
		: pAlphaMask(layer.pAlphaMask), pDetailMap(layer.pDetailMap) {}
};


struct S_API ITerrain
{
public:
	struct LodLevel
	{
		unsigned long quadSegs; // Segments on one side of a quad in a chunk of this lod level
		unsigned long chunkQuads; // quads on one side of a chunk in this lod level

		std::vector<STerrainChunk*> chunks;

		SVertex* pChunkVertices;
		unsigned long nChunkVertices; // all (not only one side)

		SLargeIndex* pIndices;
		unsigned long nIndices;
		unsigned long nActualIndices;

		SVertex* pVertices;
		unsigned long nVertices;

		IVertexBuffer* pVB;
		IIndexBuffer* pIB;		

		LodLevel()
			: quadSegs(0),
			chunkQuads(0),
			pChunkVertices(0),
			nChunkVertices(0),
			pIndices(0),
			nIndices(0),
			nActualIndices(0),
			pVertices(0),
			nVertices(0),
			pVB(0),
			pIB(0)
		{
		}
	};

	virtual ~ITerrain() {}

	virtual bool IsInited() const = 0;
		
	virtual SResult Init(IGameEngine* pEngine, unsigned int segments, unsigned int chunkSegments, float size, float baseHeight = 0, float fChunkStepDist = 15.0f, unsigned int nLodLevels = 4) = 0;
	virtual void GenLodLevelChunks(SCamera* pCamera) = 0;	
	virtual void SetHeightmap(ITexture* heightmap) = 0;
	virtual ITexture* GetHeightmap() const = 0;
	virtual float SampleHeight(const Vec2f& texcoords, bool bilinear = false) const = 0;
	
	virtual STerrainRenderDesc* GetUpdatedRenderDesc(SCamera* pCamera) = 0;

	// Does not clear the heightmap
	virtual void Clear() = 0;

	virtual float GetHeightScale() const = 0;
	virtual void SetHeightScale(float f) = 0;	

	// Get the minimum height in the vtx heightmap
	virtual float GetMinHeight() const = 0;

	// Get the maximum height in the vtx heightmap
	virtual float GetMaxHeight() const = 0;

	ILINE virtual void CalcMinMaxHeightAfterChange(Vec2f areaMin = Vec2f(0, 0), Vec2f areaMax = Vec2f(1.0f, 1.0f)) = 0;

	ILINE virtual bool RayHeightmapIntersection(const SRay& ray, const unsigned int recDepth, const float step, Vec3f& intersection) const = 0;

	ILINE virtual Vec2f GetMinXZ() const = 0;
	ILINE virtual Vec2f GetMaxXZ() const = 0;

	//virtual SResult RecalculateNormals(unsigned int lodLevel = 0) = 0;

	virtual SResult SetColorMap(ITexture* pColorMap) = 0;
	ILINE virtual ITexture* GetColorMap() const = 0;

	// Returns index of added layer
	ILINE virtual unsigned int AddLayer(const STerrainLayer& layer) = 0;

	ILINE virtual STerrainLayer* GetLayer(unsigned int index) = 0;

	ILINE virtual unsigned int GetLayerCount() const = 0;	
	
	// Summary:
	//	Flags that the terrain Constant Buffer has to be updated.
	//	Be careful to not call this during rendering of the terrain, as it might be reset before taking effect.
	virtual void RequireCBUpdate() = 0;

	virtual void RequireRender() = 0;	
};

SP_NMSPACE_END