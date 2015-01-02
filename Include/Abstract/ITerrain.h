// ********************************************************************************************

//	SpeedPoint Terrain Solid

//	This is a virtual class. You cannot instantiate it.
//	Please use Specific Implementation to instantiate.

// ********************************************************************************************

#pragma once

#include <SPrerequisites.h>


SP_NMSPACE_BEG

struct S_API IGameEngine;
struct S_API ITexture;
struct S_API IVertexBuffer;
struct S_API IIndexBuffer;

// SpeedPoint Terrain Solid (abstract)
struct S_API ITerrain
{
public:
	virtual ~ITerrain() {}

	virtual bool IsInited() const = 0;

	// Initialize with the engine
	// nX and nZ is the resolution
	virtual SResult Initialize( IGameEngine* pEngine, unsigned int nX, unsigned int nZ ) = 0;

	// Create a planar terrain with Size fW x fD and resolution nX, nY	
	virtual SResult CreatePlanar( float fW, float fD, float baseHeight, bool bDynamic = false ) = 0;

	virtual SResult RecalculateNormals() = 0;

	virtual SResult SetColorMap(ITexture* pColorMap) = 0;
	virtual SResult SetDetailMap(ITexture* pDetailMap) = 0;

	virtual IVertexBuffer* GetVertexBuffer() = 0;
	virtual IIndexBuffer* GetIndexBuffer() = 0;
	
	// Summary:
	//	Flags that the terrain Constant Buffer has to be updated.
	//	Be careful to not call this during rendering of the terrain, as it might be reset before taking effect.
	virtual void RequireCBUpdate() = 0;

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

SP_NMSPACE_END