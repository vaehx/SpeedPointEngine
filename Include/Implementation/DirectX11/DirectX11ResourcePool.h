//////////////////////////////////////////////////////////////////////////////////////////////////
//
//	This file is part of the SpeedPoint Game Engine
//
//	written by Pascal R. aka iSmokiieZz
//	(c) 2011-2014, All rights reserved.
//
//////////////////////////////////////////////////////////////////////////////////////////////////

#pragma once
#include <Abstract\IResourcePool.h>
#include "DirectX11IndexBuffer.h"
#include "DirectX11VertexBuffer.h"
#include "DirectX11Effect.h"
#include "DirectX11Texture.h"
#include <Util\SPool.h>

SP_NMSPACE_BEG


// SpeedPoint DirectX11 Resource Pool
class S_API DirectX11ResourcePool : public IResourcePool
{
public:
	SpeedPointEngine*		pEngine;
	DirectX11Renderer*		pDXRenderer;

	SPool<DirectX11IndexBuffer>	plIndexBuffers;
	SPool<DirectX11VertexBuffer>	plVertexBuffers;
	SPool<DirectX11Effect>		plShaders;
	SPool<DirectX11Texture>		plTextures;

	// **************************************************************************
	//				GENERAL
	// **************************************************************************

	// Initialize the Resource pool
	SResult Initialize(SpeedPointEngine* pEngine, IRenderer* pRenderer);

	// **************************************************************************
	//				VertexBuffer
	// **************************************************************************

	// Add a new VertexBuffer
	SResult	AddVertexBuffer(IVertexBuffer** pVBuffer, SP_ID* pUID);

	// Get a pointer to a VertexBuffer by id
	IVertexBuffer* GetVertexBuffer(SP_ID iUID);

	// Remove a VertexBuffer by id from the pool
	SResult RemoveVertexBuffer(SP_ID iUID);

	// **************************************************************************
	//				IndexBuffer
	// **************************************************************************

	// Add a new IndexBuffer
	SResult	AddIndexBuffer(IIndexBuffer** pIBuffer, SP_ID* pUID);

	// Get a pointer to a IndexBuffer by id
	IIndexBuffer* GetIndexBuffer(SP_ID iUID);

	// Remove a IndexBuffer by id from the pool
	SResult RemoveIndexBuffer(SP_ID iUID);

	// **************************************************************************
	//				Texturing
	// **************************************************************************

	// Add a new Texture
	SResult AddTexture(SString src, UINT w, UINT h, SString spec, ITexture** pTex, SP_ID* pUID);

	SResult AddTexture(UINT w, UINT h, SString spec, S_TEXTURE_TYPE ty, ITexture** pTex, SP_ID* pUID);

	// Get a Texture Instance by its id
	ITexture* GetTexture(SP_ID iUID);

	// Get a Texture Instance by its specification
	ITexture* GetTexture(SString spec);

	// Get Texture Specification
	char* GetTextureSpecification(SP_ID iUID);

	// Get Texture Unique ID by specification
	SP_ID GetTextureUID(SString spec);

	// Remove Texture
	SResult RemoveTexture(SP_ID iUID);

	// ForEach Texture
	SResult ForEachTexture(void(*iterationFunc)(ITexture*, const SP_ID&));

	// **************************************************************************
	//				All
	// **************************************************************************

	// Clear all Pools
	SResult	ClearAll(VOID);
};


SP_NMSPACE_END