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

struct S_API STerrainDescription
{
	unsigned int nX, nZ;
	unsigned int nChunkSideSegments;
	float width, depth;
	float baseHeight;
	bool bDynamic;

	STerrainDescription()		
		: bDynamic(false)
	{
	}

	STerrainDescription(const STerrainDescription& tdsc)
		: nX(tdsc.nX),	
		nZ(tdsc.nZ),
		nChunkSideSegments(tdsc.nChunkSideSegments),
		width(tdsc.width),
		depth(tdsc.depth),
		baseHeight(tdsc.baseHeight)
	{
	}

	inline bool IsValid() const
	{
		return (nX > 0 && nZ > 0 && nChunkSideSegments > 0 && width > 0 && depth > 0);
	}
};


// SpeedPoint Terrain Solid (abstract)
struct S_API ITerrain
{
public:
	virtual ~ITerrain() {}

	virtual bool IsInited() const = 0;

	// Initialize with the engine
	// nX and nZ is the resolution
	virtual SResult Initialize(IGameEngine* pEngine) = 0;

	// Create a planar terrain using given terrain description	
	virtual SResult CreatePlanar(const STerrainDescription& tdsc) = 0;

	virtual SResult RecalculateNormals() = 0;

	virtual SResult SetColorMap(ITexture* pColorMap) = 0;
	virtual SResult SetDetailMap(ITexture* pDetailMap) = 0;

	virtual IVertexBuffer* GetVertexBuffer() = 0;
	virtual IIndexBuffer* GetIndexBuffer(unsigned int lodLevel) = 0;
	
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
	virtual SResult RenderTerrain( const SVector3& lodCenterPos ) = 0;

	// Clearout the terrain data BUT not the heightmap
	virtual SResult Clear( void ) = 0;
};

SP_NMSPACE_END