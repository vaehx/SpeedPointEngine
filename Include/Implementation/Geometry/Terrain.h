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
	STerrainDescription m_TerrainDsc;

	float m_fDMTexScaleU, m_fDMTexScaleV;
		
	unsigned int m_nChunkBigQuads; // big quads in a chunk in second lod level
	unsigned int m_nChunkSmallQuads; // number of quads on highest lod grid in a chunk


	//Geometry m_Geometry;
	IVertexBuffer* m_pVertexBuffer;
	IIndexBuffer* m_pIndexBuffer[2];


	IGameEngine* m_pEngine;
	ITexture* m_pColorMap;
	ITexture* m_pDetailMap;	

	bool m_bRequireCBUpdate;	

public:
	Terrain()		
		: m_fDMTexScaleU(1.0f),
		m_fDMTexScaleV(1.0f),
		m_pEngine(nullptr),
		m_pColorMap(nullptr),
		m_pDetailMap(nullptr),
		m_bRequireCBUpdate(true),
		m_pVertexBuffer(nullptr),
		chunkStartDiff(0)
	{
		for (unsigned int iIndexBuffer = 0; iIndexBuffer < 2; ++iIndexBuffer)
			m_pIndexBuffer[iIndexBuffer] = 0;
	}

	virtual ~Terrain();

	virtual bool IsInited() const
	{
		return (IS_VALID_PTR(m_pEngine) && IS_VALID_PTR(m_pColorMap) && IS_VALID_PTR(m_pDetailMap));
	}

	// Initialize with the engine
	// nX and nZ is the resolution
	virtual SResult Initialize(IGameEngine* pEngine);

	virtual SResult RecalculateNormals();

	// Create a planar terrain with Size fW x fD
	virtual SResult CreatePlanar(const STerrainDescription& tdsc);	

	virtual SResult SetColorMap(ITexture* pColorMap);
	virtual SResult SetDetailMap(ITexture* pDetailMap);

	virtual IVertexBuffer* GetVertexBuffer();
	virtual IIndexBuffer* GetIndexBuffer(unsigned int lodLevel);

	virtual void RequireCBUpdate() { m_bRequireCBUpdate = true; }

	virtual SResult RenderTerrain(const SVector3& lodCenterPos);	
	virtual SResult Clear(void);
};

SP_NMSPACE_END