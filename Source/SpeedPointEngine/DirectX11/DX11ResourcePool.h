//////////////////////////////////////////////////////////////////////////////////////////////////
//
//	SpeedPoint Game Engine
//	Copyright (c) 2011-2016 Pascal Rosenkranz, All rights reserved.
//
//////////////////////////////////////////////////////////////////////////////////////////////////

#pragma once
#include "DX11IndexBuffer.h"
#include "DX11VertexBuffer.h"
#include "DX11Shader.h"
#include "DX11Texture.h"
#include <Abstract\IResourcePool.h>
#include <Abstract\ChunkPool.h>

SP_NMSPACE_BEG


struct S_API IGameEngine;

// SpeedPoint DirectX11 Resource Pool
class S_API DX11ResourcePool : public IResourcePool
{
private:
	IGameEngine* m_pEngine;
	DX11Renderer* m_pDXRenderer;

	ChunkPool<DX11IndexBuffer>	m_plIndexBuffers;
	ChunkPool<DX11VertexBuffer> m_plVertexBuffers;
	ChunkPool<DX11Shader> m_plShaders;
	ChunkPool<DX11Texture> m_plTextures;

	inline static string GetResourcePath(const string& file);

public:		
	virtual SResult Initialize(IGameEngine* pEngine, IRenderer* pRenderer);
	virtual SResult ClearAll();



	virtual SResult AddVertexBuffer(IVertexBuffer** pVBuffer);	
	virtual SResult RemoveVertexBuffer(IVertexBuffer** pVB);



	virtual SResult AddIndexBuffer(IIndexBuffer** pIBuffer);
	virtual SResult RemoveIndexBuffer(IIndexBuffer** pIB);



	virtual SResult AddTexture(const string& specification, ITexture** pTex, UINT w, UINT h, UINT miplevels = 1, const ETextureType& ty = eTEXTURE_R8G8B8A8_UNORM, const SColor& clearcolor = SColor());
	virtual ITexture* GetTexture(const string& specification);
	virtual ITexture* GetCubeTexture(const string& file);
	virtual SResult RemoveTexture(const string& specification);
	virtual SResult RemoveTexture(ITexture** pTex);



	virtual SResult ForEachTexture(IForEachHandler<ITexture*>* pForEachHandler);
	virtual void ListTextures(vector<string>& list) const;
};


SP_NMSPACE_END
