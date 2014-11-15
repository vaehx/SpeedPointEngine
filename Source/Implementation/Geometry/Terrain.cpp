// SpeedPoint Basic Terrain

#include <Implementation\Geometry\Terrain.h>


SP_NMSPACE_BEG

// ------------------------------------------------------------------------------------	
S_API SResult Terrain::Initialize( SpeedPointEngine* pEngine, int nX, int nZ )
{
	return S_ERROR;
}

// ------------------------------------------------------------------------------------
S_API void Terrain::GetResolution( int* nX, int* nZ )
{
	return;
}

// ------------------------------------------------------------------------------------
S_API SResult Terrain::CreatePlanar( float fW, float fD )
{
	return S_ERROR;
}

// ------------------------------------------------------------------------------------
S_API SResult Terrain::CreateFromHeightMap( SP_ID iTexture, float fW, float fD )
{
	return S_ERROR;
}

// ------------------------------------------------------------------------------------
S_API SResult Terrain::RenderTerrain( void )
{
	return S_ERROR;
}


SP_NMSPACE_END