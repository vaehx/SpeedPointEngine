///////////////////////////////////////////////////////////////////////////////////////////////////////////////////////
//
//	SpeedPoint Game Engine
//	Copyright (c) 2011-2017 Pascal Rosenkranz, All rights reserved.
//
///////////////////////////////////////////////////////////////////////////////////////////////////////////////////////

#include "DX11ResourcePool.h"
#include "DX11VertexBuffer.h"
#include "DX11IndexBuffer.h"
#include "DX11InstanceBuffer.h"
#include "DX11Texture.h"
#include "DX11Shader.h"
#include "DX11Renderer.h"
#include <Common\FileUtils.h>

SP_NMSPACE_BEG

///////////////////////////////////////////////////////////////////////////////////////////////////////////////////////
///////////////////////////////////////////////////////////////////////////////////////////////////////////////////////

S_API SResult DX11ResourcePool::Initialize(IRenderer* renderer)
{
	if (renderer == NULL)
		return S_INVALIDPARAM;

	m_pDXRenderer = dynamic_cast<DX11Renderer*>(renderer);

	CoInitialize(0);

	return S_SUCCESS;
}

S_API SResult DX11ResourcePool::ClearAll()
{
	SResult res = S_SUCCESS;

	// Destructors of instances are called by the pool when the slot buffer is destroyed
	m_InstanceBuffers.clear();
	m_plVertexBuffers.Clear();
	m_plIndexBuffers.Clear();
	m_plTextures.Clear();

	m_pDXRenderer = NULL;

	CoUninitialize();

	return res;
}

///////////////////////////////////////////////////////////////////////////////////////////////////////////////////////

template<typename T, const unsigned int chunk_size>
void RunGarbageCollectionOnResourcePool(ChunkedObjectPool<T, chunk_size>& pool)
{
	unsigned int i;
	T* resource = pool.GetFirstUsedObject(i);
	while (resource)
	{
		if (resource->GetRefCount() == 0)
			pool.Release(&resource);

		resource = pool.GetNextUsedObject(i);
	}
}

S_API void DX11ResourcePool::RunGarbageCollection()
{
	for (auto itInstanceBuffer = m_InstanceBuffers.begin(); itInstanceBuffer != m_InstanceBuffers.end();)
	{
		IResource* instanceBuffer = static_cast<IResource*>(*itInstanceBuffer);
		if (!instanceBuffer || instanceBuffer->GetRefCount() == 0)
			itInstanceBuffer = m_InstanceBuffers.erase(itInstanceBuffer);
		else
			++itInstanceBuffer;
	}
	
	RunGarbageCollectionOnResourcePool(m_plVertexBuffers);
	RunGarbageCollectionOnResourcePool(m_plIndexBuffers);
	RunGarbageCollectionOnResourcePool(m_plTextures);
}

///////////////////////////////////////////////////////////////////////////////////////////////////////////////////////

S_API const string& DX11ResourcePool::GetResourceRootPath() const
{
	return m_RootPath;
}

S_API void DX11ResourcePool::SetResourceRootPath(const string& rootSystemPath)
{
	m_RootPath = rootSystemPath;
}

S_API string DX11ResourcePool::GetResourceSystemPath(const string& resourcePath) const
{
	if (resourcePath.empty())
		return m_RootPath;

	if (m_RootPath.empty())
		return resourcePath;

	string sysPath = strreplace(resourcePath, '/', '\\');
	if (!strstartswith(sysPath, '\\'))
		sysPath = "\\" + sysPath;

	return m_RootPath + sysPath;
}

S_API string DX11ResourcePool::GetAbsoluteResourcePath(const string& relPath, const string& absReference) const
{
	if (relPath.empty())
		return absReference;

	if (relPath[0] == '/')
		return relPath; // already absolute

	string absPath = absReference;
	if (!strendswith(absPath, '/'))
		absPath += "/";

	return absPath + relPath;
}





///////////////////////////////////////////////////////////////////////////////////////////////////////////////////////
//
//	VertexBuffer
//
///////////////////////////////////////////////////////////////////////////////////////////////////////////////////////

S_API SResult DX11ResourcePool::AddVertexBuffer(IVertexBuffer** pVBuffer)
{
	if (m_pDXRenderer == NULL)
		return CLog::Log(S_ERROR, "Cannot add Vertex Buffer Resource: Renderer not initialized!");

	if (!pVBuffer)
		return S_INVALIDPARAM;

	DX11VertexBuffer* pdxVertexBuffer = m_plVertexBuffers.Get();
	if (!pdxVertexBuffer)
		return S_ERROR;

	pdxVertexBuffer->AddRef();
	*pVBuffer = pdxVertexBuffer;
	return S_SUCCESS;
}






///////////////////////////////////////////////////////////////////////////////////////////////////////////////////////
//
//	IndexBuffer
//
///////////////////////////////////////////////////////////////////////////////////////////////////////////////////////

S_API SResult DX11ResourcePool::AddIndexBuffer(IIndexBuffer** pIBuffer)
{
	if (!pIBuffer)
		return S_INVALIDPARAM;
	
	DX11IndexBuffer* pdxIndexBuffer = m_plIndexBuffers.Get();
	if (!pdxIndexBuffer)
		return S_ERROR;

	pdxIndexBuffer->AddRef();
	*pIBuffer = pdxIndexBuffer;
	return S_SUCCESS;
}





///////////////////////////////////////////////////////////////////////////////////////////////////////////////////////
//
//	InstanceBuffer
//
///////////////////////////////////////////////////////////////////////////////////////////////////////////////////////

S_API ITypelessInstanceBuffer* DX11ResourcePool::CreateTypelessInstanceBuffer()
{
	return new DX11InstanceBuffer(m_pDXRenderer);
}

S_API void DX11ResourcePool::OnInstanceBufferResourceCreated(IInstanceBufferResource* pInstanceBuffer)
{
	if (!pInstanceBuffer)
		return;

	m_InstanceBuffers.push_back(pInstanceBuffer);
}





///////////////////////////////////////////////////////////////////////////////////////////////////////////////////////
//
//	Textures
//
///////////////////////////////////////////////////////////////////////////////////////////////////////////////////////

S_API DX11Texture* DX11ResourcePool::FindDX11TextureBySpecification(const string& specification)
{
	unsigned int iTexture;
	DX11Texture* pTexture = m_plTextures.GetFirstUsedObject(iTexture);
	while (pTexture)
	{
		if (pTexture->GetSpecification() == specification)
			return pTexture;

		pTexture = m_plTextures.GetNextUsedObject(iTexture);
	}

	return 0;
}

S_API ITexture* DX11ResourcePool::GetTexture(const string& specification, EAbsentTextureStrategy absentTextureStrategy)
{
	if (specification.empty())
		return 0;

	DX11Texture* pTexture = FindDX11TextureBySpecification(specification);
	if (pTexture)
	{
		pTexture->AddRef();
	}
	else if (absentTextureStrategy != eTEX_ABSENT_NOOP)
	{
		pTexture = m_plTextures.Get();
		pTexture->AddRef();
		pTexture->SetSpecification(specification);
		pTexture->D3D11_SetRenderer(m_pDXRenderer);

		// Try to load the texture.
		// This attempt may fail if the specification is not actually a filename.
		if (absentTextureStrategy == eTEX_ABSENT_CREATE_AND_ATTEMPT_LOAD && IsAbsoluteResourcePath(specification))
		{
			pTexture->LoadFromFile(GetResourceSystemPath(specification));
		}
		else
		{
			CLog::Log(S_DEBUG, "Added texture '%s'", specification.c_str());
		}
	}

	return pTexture;
}

S_API ITexture* DX11ResourcePool::FindTexture(const string& specification)
{
	unsigned int iTexture;
	DX11Texture* pTexture = m_plTextures.GetFirstUsedObject(iTexture);
	while (pTexture)
	{
		if (pTexture->GetSpecification() == specification)
			return pTexture;

		pTexture = m_plTextures.GetNextUsedObject(iTexture);
	}

	return 0;
}

S_API ITexture* DX11ResourcePool::GetCubeTexture(const string& file)
{
	if (file.empty())
		return 0;

	DX11Texture* pTexture = FindDX11TextureBySpecification(file);
	if (pTexture)
	{
		pTexture->AddRef();
	}
	else
	{
		pTexture = m_plTextures.Get();
		pTexture->AddRef();
		pTexture->SetSpecification(file);
		pTexture->D3D11_SetRenderer(m_pDXRenderer);
	}

	if (!pTexture->IsCubemap())
	{
		if (Failure(pTexture->LoadCubemapFromFile(GetResourceSystemPath(file))))
			CLog::Log(S_ERROR, "DX11ResourcePool::GetCubeTexture(): Failed DX11Texture::LoadCubemapFromFile()");
	}

	return pTexture;
}

S_API void DX11ResourcePool::ForEachTexture(const std::function<void(ITexture*)>& fn)
{
	if (!fn)
		return;

	unsigned int i;
	DX11Texture* pTexture = m_plTextures.GetFirstUsedObject(i);
	while (pTexture)
	{
		fn(pTexture);
		pTexture = m_plTextures.GetNextUsedObject(i);
	}
}

S_API void DX11ResourcePool::ListTextures(vector<string>& list) const
{
	unsigned int i;
	DX11Texture* pTexture = m_plTextures.GetFirstUsedObject(i);
	while (pTexture)
	{
		list.push_back(pTexture->GetSpecification());
		pTexture = m_plTextures.GetNextUsedObject(i);
	}
}


SP_NMSPACE_END
