// ********************************************************************************************

//	SpeedPoint Basic Terrain

// ********************************************************************************************

#pragma once
#include <Abstract\SPrerequisites.h>
#include <Abstract\ITerrain.h>


SP_NMSPACE_BEG


// SpeedPoint Terrain
class S_API Terrain : public ITerrain
{
private:
	IRenderer* m_pRenderer;

	bool bDynamic;

	bool m_bRequireRender;			
	bool m_bRequireCBUpdate;	

	// -----
	float m_fBaseHeight;
	float m_HeightScale;
	ITexture* m_pVtxHeightMap;	
	bool m_bCustomHeightmapSet;	

	bool m_bCenter; // true to use (0,0) as the CENTER of the terrain

	ITexture* m_pColorMap;
	std::vector<STerrainLayer*> m_Layers;

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

	// ------

	SMesh m_ProxyMesh;

	SResult GenerateFlatVertexHeightmap(float baseHeight);

	ILINE bool RayHeightmapIntersectionRec(float maxHeight, float minHeight, const SRay& ray, const unsigned int recDepth,
		const float step, Vec3f& intersection, const unsigned int curDepth) const;

public:
	Terrain()		
		: m_pRenderer(nullptr),
		m_pColorMap(nullptr),
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
		return (IS_VALID_PTR(m_pRenderer) && IS_VALID_PTR(m_pColorMap) && m_Layers.size() > 0);
	}
	
	virtual SResult Init(IRenderer* pRenderer, const STerrainInfo& info);

	virtual STerrainInfo GetInfo() const;

	virtual void CalculateProxyMesh(unsigned int maxKTreeRecDepth = 4);

	virtual void UpdateRenderDesc(STerrainRenderDesc* pTerrainRenderDesc);

	virtual void GenLodLevelChunks(SCamera* pCamera);	
	virtual void SetHeightmap(ITexture* heightmap);

	virtual ITexture* GetHeightmap() const
	{
		return m_pVtxHeightMap;
	}

	virtual float SampleHeight(const Vec2f& texcoords, bool bilinear = false) const;

	virtual float GetMinHeight() const;	
	virtual float GetMaxHeight() const;
	
	ILINE virtual void MarkDirtyArea(Vec2f areaMin = Vec2f(0, 0), Vec2f areaMax = Vec2f(1.0f, 1.0f));

	ILINE virtual Vec2f GetMinXZ() const;
	ILINE virtual Vec2f GetMaxXZ() const;

	ILINE virtual Vec2f XZToTexCoords(float x, float z) const;

	ILINE virtual bool RayHeightmapIntersection(const SRay& ray, const unsigned int recDepth, const float step, Vec3f& intersection) const;

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
	ILINE virtual ITexture* GetColorMap() const;

	ILINE virtual unsigned int AddLayer(const STerrainLayer& layer);
	ILINE virtual STerrainLayer* GetLayer(unsigned int index);
	ILINE virtual unsigned int GetLayerCount() const;

	virtual void RequireCBUpdate() { m_bRequireCBUpdate = true; }

	virtual void RequireRender() { m_bRequireRender = true; }
	
	virtual void Clear(void);

	ILINE virtual const SMesh* GetProxyMesh() const
	{
		return &m_ProxyMesh;
	}
};

SP_NMSPACE_END