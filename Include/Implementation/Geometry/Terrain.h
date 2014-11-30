// ********************************************************************************************

//	SpeedPoint Basic Terrain

// ********************************************************************************************

#pragma once
#include <SPrerequisites.h>
#include <Abstract\ITerrain.h>
#include "Geometry.h"


SP_NMSPACE_BEG


// SpeedPoint Terrain
class S_API Terrain : public ITerrain
{
private:
	unsigned int m_nX, m_nZ;
	float m_DetailTextureScale;
	Geometry m_Geometry;
	IGameEngine* m_pEngine;

public:
	Terrain()
		: m_nX(0), m_nZ(0), m_DetailTextureScale(0.1f), m_pEngine(nullptr) {}

	virtual ~Terrain();

	// Initialize with the engine
	// nX and nZ is the resolution
	virtual SResult Initialize(IGameEngine* pEngine, unsigned int nX, unsigned int nZ);

	// Create a planar terrain with Size fW x fD
	virtual SResult CreatePlanar(float fW, float fD, float baseHeight);	

	virtual SResult RenderTerrain(void);	
	virtual SResult Clear(void);
};

SP_NMSPACE_END