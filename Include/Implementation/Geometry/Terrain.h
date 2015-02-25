// ********************************************************************************************

//	SpeedPoint Basic Terrain

// ********************************************************************************************

#pragma once
#include <SPrerequisites.h>
#include <Abstract\ITerrain.h>
#include "Geometry.h"
#include <Abstract\IObject.h>


SP_NMSPACE_BEG


// SpeedPoint Terrain
class S_API Terrain : public ITerrain
{
private:
	//STerrainDescription m_TerrainDsc;

	bool m_bRequireRender;

	float m_fDMTexScaleU,
		m_fDMTexScaleV;
		
	//unsigned int m_nChunkBigQuads; // big quads in a chunk in second lod level
	//unsigned int m_nChunkSmallQuads; // number of quads on highest lod grid in a chunk


	//Geometry m_Geometry;
	IVertexBuffer** m_pHWVertexBuffers;	// IVertexBuffer[nLodLevels]
	IIndexBuffer** m_pHWIndexBuffers;		// IIndexBuffer[nLodLevels]


	IGameEngine* m_pEngine;
	ITexture* m_pColorMap;
	ITexture* m_pDetailMap;	

	bool m_bRequireCBUpdate;

	bool m_bUseVBCulledRendering;

	// -----
	float m_MaxHeight;	
	ITexture* m_pVtxHeightMap;

	SVertex** pVertexBuffers; // one VB per lod level
	SIndex** pIndexBuffers; // one IB per lod level
	unsigned int nSegsX, nSegsZ;
	unsigned int nChunkSegsX, nChunkSegsZ;
	unsigned int nLodLevels;	
	STerrainLodCounts* pLodLevelCounts;	// array of terrain chunk costs
	float fWidth, fDepth;
	bool bDynamic;

	STerrainChunk* pTerrainChunks;
	unsigned int nChunksX, nChunksZ; // including rest chunks
	unsigned int nRestChunksX, nRestChunksZ; // essentially 0 or 1    

	// Top rest chunk: (nChunkSegsX, nZRestChunkSegs)
	// Right rest chunk: (nXRestChunkSegs, nChunkSegsZ)
	// Fill rest chunk: (nXRestChunkSegs, nZRestChunkSegs)    
	unsigned int nXRestChunkSegs, nZRestChunkSegs;

	// Keeps indices, in order to properly RecalculateNormals afterwards.
	void ClearTemporaryGenerationVertices();

	SResult GenerateFlatVertexHeightmap(float baseHeight);

public:
	Terrain()		
		: m_fDMTexScaleU(1.0f),
		m_fDMTexScaleV(1.0f),
		m_pEngine(nullptr),
		m_pColorMap(nullptr),
		m_pDetailMap(nullptr),
		m_bRequireCBUpdate(true),
		pVertexBuffers(nullptr),
		pIndexBuffers(nullptr),
		pTerrainChunks(nullptr),
		m_bUseVBCulledRendering(true),
		m_MaxHeight(100.0f),		
		m_pVtxHeightMap(nullptr)
	{	
	}

	virtual ~Terrain();

	virtual bool IsInited() const
	{
		return (IS_VALID_PTR(m_pEngine) && IS_VALID_PTR(m_pColorMap) && IS_VALID_PTR(m_pDetailMap));
	}

	// Initialize with the engine
	// nX and nZ is the resolution
	virtual SResult Initialize(IGameEngine* pEngine);

	virtual SResult RecalculateNormals(unsigned int lodLevel = 0);

	// Create a planar terrain with Size fW x fD
	//virtual SResult CreatePlanar(const STerrainDescription& tdsc);	

	virtual void Generate(const STerrainDescription& tdsc);	

	// Create and fill vertex and index buffers
	SResult FillVertexAndIndexBuffers();

	virtual float GetMaxHeight() const
	{
		return m_MaxHeight;
	}

	virtual void SetMaxHeight(float f)
	{
		m_MaxHeight = f;
		RequireCBUpdate();
		RequireRender();
	}


	virtual SResult SetColorMap(ITexture* pColorMap);
	virtual SResult SetDetailMap(ITexture* pDetailMap);

	virtual IVertexBuffer* GetVertexBuffer(unsigned int lodLevel);
	virtual IIndexBuffer* GetIndexBuffer(unsigned int lodLevel);

	virtual void RequireCBUpdate() { m_bRequireCBUpdate = true; }

	virtual void RequireRender() { m_bRequireRender = true; }

	virtual SResult RenderTerrain(const SVector3& camPos);	
	virtual SResult Clear(void);
};

SP_NMSPACE_END