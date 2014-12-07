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
	unsigned int m_nX, m_nZ;
	float m_fDMTexScaleU, m_fDMTexScaleV;
	Geometry m_Geometry;
	IGameEngine* m_pEngine;
	ITexture* m_pColorMap;
	ITexture* m_pDetailMap;	

	bool m_bRendererdOnce;

public:
	Terrain()
		: m_nX(0), m_nZ(0),
		m_fDMTexScaleU(1.0f),
		m_fDMTexScaleV(1.0f),
		m_pEngine(nullptr),
		m_pColorMap(nullptr),
		m_pDetailMap(nullptr),
		m_bRendererdOnce(false)
		{}

	virtual ~Terrain();

	// Initialize with the engine
	// nX and nZ is the resolution
	virtual SResult Initialize(IGameEngine* pEngine, unsigned int nX, unsigned int nZ);

	// Create a planar terrain with Size fW x fD
	virtual SResult CreatePlanar(float fW, float fD, float baseHeight);	

	virtual SResult SetColorMap(ITexture* pColorMap);
	virtual SResult SetDetailMap(ITexture* pDetailMap);

	virtual SResult RenderTerrain(void);	
	virtual SResult Clear(void);
};

SP_NMSPACE_END