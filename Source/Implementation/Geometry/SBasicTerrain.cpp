// SpeedPoint Basic Terrain

#include <Implementation\Geometry\SBasicTerrain.h>

namespace SpeedPoint
{
	// **********************************************************************************
	
	S_API SResult SBasicTerrain::Initialize( SpeedPointEngine* pEngine, int nX, int nZ )
	{
		return S_ERROR;
	}

	// **********************************************************************************

	S_API void SBasicTerrain::GetResolution( int* nX, int* nZ )
	{
		return;
	}

	// **********************************************************************************

	S_API SResult SBasicTerrain::CreatePlanar( float fW, float fD )
	{
		return S_ERROR;
	}

	// **********************************************************************************

	S_API SResult SBasicTerrain::CreateFromHeightMap( SP_ID iTexture, float fW, float fD )
	{
		return S_ERROR;
	}

	// **********************************************************************************

	S_API SResult SBasicTerrain::RenderTerrain( void )
	{
		return S_ERROR;
	}


	// **********************************************************************************

	S_API SResult SBasicTerrain::Clear( void )
	{
		return S_ERROR;
	}
}