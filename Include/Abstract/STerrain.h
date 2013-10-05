// ********************************************************************************************

//	SpeedPoint Terrain Solid

//	This is a virtual class. You cannot instantiate it.
//	Please use Specific Implementation to instantiate.

// ********************************************************************************************

#pragma once

#include <SPrerequisites.h>

namespace SpeedPoint
{
	// SpeedPoint Terrain Solid (abstract)
	class S_API STerrain
	{
	public:
		// Initialize with the engine
		// nX and nZ is the resolution
		virtual SResult Initialize( SpeedPointEngine* pEngine, int nX, int nZ ) = 0;

		// Get pointers to the resolution to edit them
		virtual void GetResolution( int* nX, int* nZ ) = 0;

		// Create a planar one
		// fW and fD the actual world size
		virtual SResult CreatePlanar( float fW, float fD ) = 0;

		// Load a terrain from a heightmap
		// fW and fD is the actual world size
		virtual SResult CreateFromHeightMap( SP_ID iTexture, float fW, float fD ) = 0;


		/////////////////////
		///// TODO: Implement function to add / remove assigned texture
		/// this is not really clear as of now:
		///	- Do we support multiple texture using?
		///	- Or only support a single texture + color map for the whole terrain that has to be rendered outside?
		///	- Do we add a single function for runtime LOD change or do we implement that into the render function?
		///////////////////

		// Render the Terrain. Fails if pointer to SpeedPointEngine is not correct
		virtual SResult RenderTerrain( void ) = 0;

		// Clearout the terrain data BUT not the heightmap
		virtual SResult Clear( void ) = 0;
	};
}