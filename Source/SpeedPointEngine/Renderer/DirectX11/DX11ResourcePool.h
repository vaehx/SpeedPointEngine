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
#include <Common\ChunkedObjectPool.h>

SP_NMSPACE_BEG

// SpeedPoint DirectX11 Resource Pool
class S_API DX11ResourcePool : public IResourcePool
{
private:
	DX11Renderer* m_pDXRenderer;
	string m_RootPath; // System path to resource root directory

	vector<IInstanceBufferResource*> m_InstanceBuffers;
	ChunkedObjectPool<DX11IndexBuffer> m_plIndexBuffers;
	ChunkedObjectPool<DX11VertexBuffer> m_plVertexBuffers;
	ChunkedObjectPool<DX11Texture> m_plTextures;

protected:
	virtual ITypelessInstanceBuffer* CreateTypelessInstanceBuffer();
	virtual void OnInstanceBufferResourceCreated(IInstanceBufferResource* instanceBuffer);

public:		
	virtual SResult Initialize(IRenderer* pRenderer);
	virtual SResult ClearAll();
	virtual void RunGarbageCollection();

	virtual const string& GetResourceRootPath() const;
	virtual void SetResourceRootPath(const string& rootSystemPath);
	virtual string GetResourceSystemPath(const string& file) const;
	virtual string GetAbsoluteResourcePath(const string& relPath, const string& absReferenceDir) const;

	virtual SResult AddVertexBuffer(IVertexBuffer** pVBuffer);
	virtual SResult AddIndexBuffer(IIndexBuffer** pIBuffer);

	// Returns 0 if not found. Returns first one in the pool if there are multiple
	DX11Texture* FindDX11TextureBySpecification(const string& specification);

	virtual ITexture* GetTexture(const string& specification, bool createIfNotFound = true);
	virtual ITexture* FindTexture(const string& specification);
	virtual ITexture* GetCubeTexture(const string& file);
	virtual void ForEachTexture(const std::function<void(ITexture*)>& fn);
	virtual void ListTextures(vector<string>& list) const;
};


SP_NMSPACE_END
