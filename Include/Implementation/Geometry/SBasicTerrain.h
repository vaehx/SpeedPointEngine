// ********************************************************************************************

//	SpeedPoint Basic Terrain

// ********************************************************************************************

#pragma once
#include <SPrerequisites.h>
#include <Abstract\ITerrain.h>
#include "SBasicSolid.h"

namespace SpeedPoint
{
	// SpeedPoint Terrain
	class S_API SBasicTerrain : public ITerrain, public SBasicSolid
	{
	public:
		int		nXResolution;
		int		nZResolution;		

		SResult Initialize( SpeedPointEngine* pEngine, int nX, int nZ );
		void GetResolution( int* nX, int* nZ );
		SResult CreatePlanar( float fW, float fD );
		SResult CreateFromHeightMap( SP_ID iTexture, float fW, float fD );
		SResult RenderTerrain( void );
		SResult Clear( void );
	};
}