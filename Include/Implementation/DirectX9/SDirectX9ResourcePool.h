// ********************************************************************************************

//	SpeedPoint DirectX9 Resource Pool

// ********************************************************************************************

#pragma once
#include <Abstract\SResourcePool.h>
#include "SDirectX9IndexBuffer.h"
#include "SDirectX9VertexBuffer.h"
#include "SDirectX9Shader.h"
#include "SDirectX9Texture.h"
#include <Util\SPool.h>

namespace SpeedPoint
{
	// SpeedPoint DirectX9 Resource Pool
	class S_API SDirectX9ResourcePool : public SResourcePool
	{
	public:
		SpeedPointEngine*		pEngine;
		SDirectX9Renderer*		pDXRenderer;

		SPool<SDirectX9IndexBuffer>	plIndexBuffers;
		SPool<SDirectX9VertexBuffer>	plVertexBuffers;
		SPool<SDirectX9Shader>		plShaders;
		SPool<SDirectX9Texture>		plTextures;

		// **************************************************************************
		//				GENERAL
		// **************************************************************************

		// Initialize the Resource pool
		SResult Initialize( SpeedPointEngine* pEngine, SRenderer* pRenderer );

		// **************************************************************************
		//				VertexBuffer
		// **************************************************************************

		// Add a new VertexBuffer
		SResult	AddVertexBuffer( SVertexBuffer** pVBuffer, SP_ID* pUID );

		// Get a pointer to a VertexBuffer by id
		SVertexBuffer* GetVertexBuffer( SP_ID iUID );

		// Remove a VertexBuffer by id from the pool
		SResult RemoveVertexBuffer( SP_ID iUID );		

		// **************************************************************************
		//				IndexBuffer
		// **************************************************************************

		// Add a new IndexBuffer
		SResult	AddIndexBuffer( SIndexBuffer** pIBuffer, SP_ID* pUID );

		// Get a pointer to a IndexBuffer by id
		SIndexBuffer* GetIndexBuffer( SP_ID iUID );

		// Remove a IndexBuffer by id from the pool
		SResult RemoveIndexBuffer( SP_ID iUID );		

		// **************************************************************************
		//				Texturing
		// **************************************************************************

		// Add a new Texture
		SResult AddTexture( SString src, UINT w, UINT h, SString spec, STexture** pTex, SP_ID* pUID );

		SResult AddTexture( UINT w, UINT h, SString spec, S_TEXTURE_TYPE ty, STexture** pTex, SP_ID* pUID );

		// Get a Texture Instance by its id
		STexture* GetTexture( SP_ID iUID );

		// Get a Texture Instance by its specification
		STexture* GetTexture( SString spec );

		// Get Texture Specification
		char* GetTextureSpecification( SP_ID iUID );

		// Get Texture Unique ID by specification
		SP_ID GetTextureUID( SString spec );

		// Remove Texture
		SResult RemoveTexture( SP_ID iUID );

		// ForEach Texture
		SResult ForEachTexture( void (*iterationFunc)(STexture*, const SP_ID&) );

		// **************************************************************************
		//				All
		// **************************************************************************

		// Clear all Pools
		SResult	ClearAll ( VOID );
	};
}