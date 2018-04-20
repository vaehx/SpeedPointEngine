// ********************************************************************************************

//	SpeedPoint Basic Terrain

// ********************************************************************************************

#pragma once
#include "..\ITerrain.h"
#include <Renderer\IRenderer.h>
#include <Common\geo.h>
#include <Common\SPrerequisites.h>
#include <vector>
#include <map>

using std::vector;
using std::map;

SP_NMSPACE_BEG

// SpeedPoint Terrain
class S_API Terrain : public ITerrain
{
private:
	IRenderer* m_pRenderer;
	STerrainParams m_Params;

	bool bDynamic;

	bool m_bRequireRender;
	bool m_bRequireCBUpdate;

	// -----
	float m_HeightScale;
	ITexture* m_pVtxHeightMap;
	bool m_bCustomHeightmapSet;

	ITexture* m_pColorMap;
	ITexture* m_pLayermask;
	ITexture* m_pTextureMaps;
	ITexture* m_pNormalMaps;
	ITexture* m_pGlossinessMaps;
	bool* m_pLayersUsed;
	STerrainLayerDesc* m_pLayerDescs;
	unsigned int m_nLayers;

	STerrainChunk* m_pChunks;
	unsigned int m_nChunks;

	LodLevel* m_pLodLevels;

	float m_fSegSz;
	float m_fTexSz; // colormap texcoord size of a segment

	float m_fMaxHeight; // cached calculated, w/o height scale
	float m_fMinHeight; // cached calculated, w/o height scale

	// ------

	SResult GenerateFlatVertexHeightmap(float baseHeight);

	ILINE bool RayHeightmapIntersectionRec(float maxHeight, float minHeight, const geo::ray& ray, const unsigned int recDepth,
		const float step, Vec3f& intersection, const unsigned int curDepth) const;

public:
	Terrain();
	virtual ~Terrain();

	virtual bool IsInited() const
	{
		return (IS_VALID_PTR(m_pRenderer) && IS_VALID_PTR(m_pColorMap) && m_nLayers > 0);
	}

	virtual SResult Init(IRenderer* pRenderer, const STerrainParams& params);

	virtual const STerrainParams& GetParams() const;

	virtual void UpdateRenderDesc(STerrainRenderDesc* pTerrainRenderDesc);

	virtual void GenLodLevelChunks(SCamera* pCamera);
	virtual void SetHeightmap(ITexture* heightmap);
	virtual ITexture* GetHeightmap() const { return m_pVtxHeightMap; }
	virtual const float* GetHeightData() const;
	virtual float SampleHeight(const Vec2f& texcoords, bool bilinear = false) const;

	virtual float GetMinHeight() const;
	virtual float GetMaxHeight() const;
	ILINE virtual void MarkDirty();

	ILINE virtual Vec2f GetMinXZ() const;
	ILINE virtual Vec2f GetMaxXZ() const;

	ILINE virtual Vec2f XZToTexCoords(float x, float z) const;

	ILINE virtual bool RayHeightmapIntersection(const geo::ray& ray, const unsigned int recDepth, const float step, Vec3f& intersection) const;

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
	ILINE virtual ITexture* GetColorMap() const { return m_pColorMap; }
	ILINE virtual ITexture* GetLayerMask() const { return m_pLayermask; }

	ILINE virtual unsigned int AddLayer(const STerrainLayerDesc& desc);
	ILINE virtual void RemoveLayer(unsigned int id);
	ILINE virtual unsigned int GetLayerCount() const;
	ILINE virtual STerrainLayerDesc GetLayerDesc(unsigned int i) const;

	virtual void RequireCBUpdate() { m_bRequireCBUpdate = true; }
	virtual void RequireRender() { m_bRequireRender = true; }
	
	virtual void Clear(void);
};

SP_NMSPACE_END
