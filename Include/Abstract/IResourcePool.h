// ********************************************************************************************

//	SpeedPoint Resource Pool

//	This is a virtual class. You cannot instantiate it.
//	Please use Specific Implementation to instantiate.

// ********************************************************************************************

#pragma once

#include <SPrerequisites.h>

SP_NMSPACE_BEG

struct S_API IGameEngine;
struct S_API IVertexBuffer;
struct S_API IIndexBuffer;
struct S_API ITexture;
struct S_API IRenderer;


// SpeedPoint Resource Pool (abstract)
struct S_API IResourcePool
{
public:
	// **************************************************************************
	//				GENERAL
	// **************************************************************************

	// Initialize the Resource pool
	virtual SResult Initialize( IGameEngine* pEngine, IRenderer* pRenderer ) = 0;

	// **************************************************************************
	//				VertexBuffer
	// **************************************************************************

	// Add a new VertexBuffer
	virtual SResult	AddVertexBuffer( IVertexBuffer** pVBuffer, SP_ID* pUID ) = 0;

	// Get a pointer to a VertexBuffer by id
	virtual IVertexBuffer* GetVertexBuffer( SP_ID iUID ) = 0;

	// Remove a VertexBuffer by id from the pool
	virtual SResult RemoveVertexBuffer( SP_ID iUID ) = 0;		

	// **************************************************************************
	//				IndexBuffer
	// **************************************************************************

	// Add a new IndexBuffer
	virtual SResult	AddIndexBuffer( IIndexBuffer** pIBuffer, SP_ID* pUID ) = 0;

	// Get a pointer to a IndexBuffer by id
	virtual IIndexBuffer* GetIndexBuffer( SP_ID iUID ) = 0;

	// Remove a IndexBuffer by id from the pool
	virtual SResult RemoveIndexBuffer( SP_ID iUID ) = 0;		

	// **************************************************************************
	//				Texturing
	// **************************************************************************

	// Add a new Texture
	virtual SResult AddTexture( SString src, UINT w, UINT h, SString spec, ITexture** pTex, SP_ID* pUID ) = 0;

	virtual SResult AddTexture( UINT w, UINT h, SString spec, S_TEXTURE_TYPE ty, ITexture** pTex, SP_ID* pUID ) = 0;

	// Get a Texture Instance by its id
	virtual ITexture* GetTexture( SP_ID iUID ) = 0;

	// Get a Texture Instance by its specification
	virtual ITexture* GetTexture( SString spec ) = 0;

	// Get Texture Specification
	virtual char* GetTextureSpecification( SP_ID iUID ) = 0;

	// Get Texture Unique ID by specification
	virtual SP_ID GetTextureUID( SString spec ) = 0;

	// Remove Texture
	virtual SResult RemoveTexture( SP_ID iUID ) = 0;

	// ForEach Texture
	virtual SResult ForEachTexture( void (*iterationFunc)(ITexture*, const SP_ID&) ) = 0;

	// **************************************************************************
	//				All
	// **************************************************************************

	// Clear all Pools
	virtual SResult	ClearAll ( void ) = 0;
};


SP_NMSPACE_END