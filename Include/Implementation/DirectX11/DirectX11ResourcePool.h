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

	string m_BasePath;

	ChunkPool<DirectX11IndexBuffer>	m_plIndexBuffers;
	ChunkPool<DirectX11VertexBuffer> m_plVertexBuffers;
	ChunkPool<DirectX11Effect> m_plShaders;
	ChunkPool<DirectX11Texture> m_plTextures;	

public:		
	virtual SResult Initialize(IGameEngine* pEngine, IRenderer* pRenderer, const string& resourceBasePath = "");
	virtual SResult ClearAll();



	virtual SResult AddVertexBuffer(IVertexBuffer** pVBuffer);	
	virtual SResult RemoveVertexBuffer(IVertexBuffer** pVB);



	virtual SResult AddIndexBuffer(IIndexBuffer** pIBuffer);
	virtual SResult RemoveIndexBuffer(IIndexBuffer** pIB);





	virtual SResult LoadTexture(const string& specification, ITexture** pTex, const string& file = "", UINT w = 0, UINT h = 0, bool bDynamic = false, bool bStaged = false);
	
	virtual SResult LoadCubeTexture(const string& specification, ITexture** pTex, const string& file = "", UINT w = 0, UINT h = 0);

	virtual SResult AddTexture(const string& specification, ITexture** pTex, UINT w, UINT h, const ETextureType& ty, const SColor& clearcolor, bool bDynamic = false, bool bStaged = false);

	// is case sensitive
	virtual ITexture* GetTexture(const string& spec);

	virtual SResult RemoveTexture(ITexture** pTex);
	virtual SResult ForEachTexture(IForEachHandler<ITexture*>* pForEachHandler);
	virtual void ListTextures(vector<string>& list) const;
};


SP_NMSPACE_END