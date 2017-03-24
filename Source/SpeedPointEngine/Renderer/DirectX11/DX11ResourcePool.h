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
#include <Renderer\IResourcePool.h>
#include <Common\ChunkPool.h>

SP_NMSPACE_BEG

// SpeedPoint DirectX11 Resource Pool
class S_API DX11ResourcePool : public IResourcePool
{
private:
	DX11Renderer* m_pDXRenderer;
	string m_RootPath;

	ChunkPool<DX11IndexBuffer>	m_plIndexBuffers;
	ChunkPool<DX11VertexBuffer> m_plVertexBuffers;
	ChunkPool<DX11Shader> m_plShaders;
	ChunkPool<DX11Texture> m_plTextures;

public:		
	virtual SResult Initialize(IRenderer* pRenderer);
	virtual SResult ClearAll();

	virtual const string& GetResourceRootPath() const;
	virtual void SetResourceRootPath(const string& path);
	virtual string GetResourcePath(const string& file) const;


	virtual SResult AddVertexBuffer(IVertexBuffer** pVBuffer);	
	virtual SResult RemoveVertexBuffer(IVertexBuffer** pVB);



	virtual SResult AddIndexBuffer(IIndexBuffer** pIBuffer);
	virtual SResult RemoveIndexBuffer(IIndexBuffer** pIB);



	virtual SResult AddTexture(const string& specification, ITexture** pTex, UINT w, UINT h, UINT miplevels = 1, const ETextureType& ty = eTEXTURE_R8G8B8A8_UNORM, const SColor& clearcolor = SColor());
	virtual ITexture* GetTexture(const string& specification);
	virtual ITexture* GetCubeTexture(const string& file);
	virtual SResult ForEachTexture(IForEachHandler<ITexture*>* pForEachHandler);
	virtual void ListTextures(vector<string>& list) const;
};


SP_NMSPACE_END
