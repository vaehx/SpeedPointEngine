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
#include <Abstract\ChunkPool.h>

SP_NMSPACE_BEG


struct S_API IGameEngine;

// SpeedPoint DirectX11 Resource Pool
class S_API DirectX11ResourcePool : public IResourcePool
{
private:
	IGameEngine* m_pEngine;
	DirectX11Renderer* m_pDXRenderer;

	ChunkPool<DirectX11IndexBuffer>	m_plIndexBuffers;
	ChunkPool<DirectX11VertexBuffer> m_plVertexBuffers;
	ChunkPool<DirectX11Effect> m_plShaders;
	ChunkPool<DirectX11Texture> m_plTextures;

public:		
	virtual SResult Initialize(IGameEngine* pEngine, IRenderer* pRenderer);
	virtual SResult ClearAll();



	virtual SResult AddVertexBuffer(IVertexBuffer** pVBuffer);	
	virtual SResult RemoveVertexBuffer(IVertexBuffer** pVB);



	virtual SResult AddIndexBuffer(IIndexBuffer** pIBuffer);
	virtual SResult RemoveIndexBuffer(IIndexBuffer** pIB);





	virtual SResult LoadTexture(const SString& src, UINT w, UINT h, const SString& spec, ITexture** pTex);
	virtual SResult AddTexture(UINT w, UINT h, const SString& spec, const ETextureType& ty, ITexture** pTex);

	// is case sensitive
	virtual ITexture* GetTexture(const SString& spec);
	virtual SString GetTextureSpecification(const ITexture* pTex) const;

	virtual SResult RemoveTexture(ITexture** pTex);
	virtual SResult ForEachTexture(IForEachHandler<ITexture*>* pForEachHandler);	
};


SP_NMSPACE_END