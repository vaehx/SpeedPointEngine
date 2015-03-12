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
	IGameEngine* m_pEngine;

	bool bDynamic;

	bool m_bRequireRender;			
	bool m_bRequireCBUpdate;				

	// -----
	float m_HeightScale;
	ITexture* m_pVtxHeightMap;	
	bool m_bCustomHeightmapSet;	

	ITexture* m_pColorMap;
	ITexture* m_pDetailMap;



	STerrainChunk* m_pChunks;
	unsigned int m_nChunks;

	LodLevel* m_pLodLevels;
	unsigned int m_nLodLevels;

	unsigned long m_chunkSegs;	// how many segments per chunk side
	unsigned long m_nSegments;	// all segments on one side
	float m_fSize;	// per side
	float m_fSegSz;
	float m_fTexSz; // texcoord size of a segment

	float m_fChunkStepDist;
	
	float m_fMaxHeight; // w/o height scale
	float m_fMinHeight; // w/o height scale
					


	void CalculateMinMaxHeights();

	SResult GenerateFlatVertexHeightmap(float baseHeight);

public:
	Terrain()		
		: m_pEngine(nullptr),
		m_pColorMap(nullptr),
		m_pDetailMap(nullptr),
		m_bRequireCBUpdate(true),
		m_pLodLevels(0),
		m_pChunks(0),		
		m_HeightScale(100.0f),		
		m_pVtxHeightMap(nullptr),
		m_bCustomHeightmapSet(false)		
	{			
	}

	virtual ~Terrain();

	virtual bool IsInited() const
	{
		return (IS_VALID_PTR(m_pEngine) && IS_VALID_PTR(m_pColorMap) && IS_VALID_PTR(m_pDetailMap));
	}
	
	virtual SResult Init(IGameEngine* pEngine, unsigned int segments, unsigned int chunkSegments, float size, float baseHeight = 0, float fChunkStepDist = 15.0f, unsigned int nLodLevels = 4);

	virtual void GenLodLevelChunks(SCamera* pCamera);
	virtual SResult Render(SCamera* pCamera);
	virtual void SetHeightmap(ITexture* heightmap);

	virtual ITexture* GetHeightmap() const
	{
		return m_pVtxHeightMap;
	}

	virtual float SampleHeight(const Vec2f& texcoords) const;
	
	virtual float GetMinHeight() const;	
	virtual float GetMaxHeight() const;

	// Create and fill vertex and index buffers
	SResult FillVertexAndIndexBuffers();

	virtual float GetHeightScale() const
	{
		return m_HeightScale;
	}

	virtual void SetHeightScale(float f)
	{
		m_HeightScale = f;
		RequireCBUpdate();
		RequireRender();
	}


	virtual SResult SetColorMap(ITexture* pColorMap);
	virtual SResult SetDetailMap(ITexture* pDetailMap);	

	virtual void RequireCBUpdate() { m_bRequireCBUpdate = true; }

	virtual void RequireRender() { m_bRequireRender = true; }
	
	virtual void Clear(void);
};

SP_NMSPACE_END