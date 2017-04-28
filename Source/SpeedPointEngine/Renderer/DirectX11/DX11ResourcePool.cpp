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
	//m_plShaders.Clear();
	m_plTextures.Clear();

	m_pDXRenderer = NULL;

	CoUninitialize();

	return res;
}

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

	DX11VertexBuffer* pdxVertexBuffer;

	if (Failure(m_plVertexBuffers.AddItem(&pdxVertexBuffer)))
		return S_ERROR;

	SP_ASSERTR(IS_VALID_PTR(pdxVertexBuffer), S_ERROR);

	if (IS_VALID_PTR(pVBuffer))
		*pVBuffer = (IVertexBuffer*)pdxVertexBuffer;

	return S_SUCCESS;
}

S_API SResult DX11ResourcePool::RemoveVertexBuffer(IVertexBuffer** pVB)
{
	SP_ASSERTR(IS_VALID_PTR(pVB), S_INVALIDPARAM);

	if (IS_VALID_PTR(*pVB))
	{
		(*pVB)->Clear();
		DX11VertexBuffer* pDXVB = dynamic_cast<DX11VertexBuffer*>(*pVB);
		if (pDXVB && !m_plVertexBuffers.Delete(&pDXVB))
			return S_ERROR;

		*pVB = 0;
	}

	return S_SUCCESS;
}






///////////////////////////////////////////////////////////////////////////////////////////////////////////////////////
//
//	IndexBuffer
//
///////////////////////////////////////////////////////////////////////////////////////////////////////////////////////

S_API SResult DX11ResourcePool::AddIndexBuffer(IIndexBuffer** pIBuffer)
{
	DX11IndexBuffer* pdxIndexBuffer;

	if (Failure(m_plIndexBuffers.AddItem(&pdxIndexBuffer)))
		return S_ERROR;

	SP_ASSERTR(IS_VALID_PTR(pdxIndexBuffer), S_ERROR);

	if (IS_VALID_PTR(pIBuffer))
		*pIBuffer = (IIndexBuffer*)pdxIndexBuffer;

	return S_SUCCESS;
}

S_API SResult DX11ResourcePool::RemoveIndexBuffer(IIndexBuffer** pIB)
{
	SP_ASSERTR(IS_VALID_PTR(pIB), S_INVALIDPARAM);

	if (IS_VALID_PTR(*pIB))
	{
		(*pIB)->Clear();
		DX11IndexBuffer* pDXIB = dynamic_cast<DX11IndexBuffer*>(*pIB);
		if (pDXIB && !m_plIndexBuffers.Delete(&pDXIB))
			return S_ERROR;

		*pIB = 0;
	}

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

S_API SResult DX11ResourcePool::RemoveInstanceBuffer(IInstanceBufferResource** pInstanceBuffer)
{
	if (!pInstanceBuffer || !*pInstanceBuffer)
		return CLog::Log(S_ERROR, "Failed remove instance buffer: nullptr given");

	for (auto itInstanceBuffer = m_InstanceBuffers.begin(); itInstanceBuffer != m_InstanceBuffers.end(); ++itInstanceBuffer)
	{
		if (*itInstanceBuffer == *pInstanceBuffer)
		{
			delete *pInstanceBuffer;
			*pInstanceBuffer = 0;
			m_InstanceBuffers.erase(itInstanceBuffer);
			break;
		}
	}

	return S_SUCCESS;
}





///////////////////////////////////////////////////////////////////////////////////////////////////////////////////////
//
//	Textures
//
///////////////////////////////////////////////////////////////////////////////////////////////////////////////////////

S_API SResult DX11ResourcePool::AddTexture(const string& specification, ITexture** pTex, UINT w, UINT h, UINT mipLevels, const ETextureType& ty, const SColor& clearcolor)
{
	if (specification.empty())
		return CLog::Log(S_INVALIDPARAM, "DX11ResourcePool::AddTexture(): Empty specification");

	if (w == 0 || h == 0)
		return CLog::Log(S_INVALIDPARAM, "DX11ResourcePool::AddTexture(): width or height is 0");

	DX11Texture* pTexture = m_plTextures.GetBySpecification(specification);
	if (!pTexture)
	{
		m_plTextures.AddItem(&pTexture, specification);
		pTexture->D3D11_SetRenderer(m_pDXRenderer);
	}
	else
	{
		pTexture->AddRef();
	}

	if (Failure(pTexture->CreateEmpty(specification, w, h, mipLevels, ty, clearcolor)))
		return CLog::Log(S_ERROR, "DX11ResourcePool::AddTexture(): Failed DX11Texture::CreateEmpty()");

	if (pTex)
		*pTex = pTexture;

	return S_SUCCESS;
}

S_API ITexture* DX11ResourcePool::GetTexture(const string& specification)
{
	if (specification.empty())
		return 0;

	DX11Texture* pDXTexture = m_plTextures.GetBySpecification(specification);
	if (!pDXTexture)
	{
		m_plTextures.AddItem(&pDXTexture, specification);
		pDXTexture->D3D11_SetRenderer(m_pDXRenderer);
	}
	else
	{
		pDXTexture->AddRef();
	}

	ITexture* pTexture = pDXTexture;
	if (!pTexture->IsInitialized())
	{
		// This attempt may fail if the specification is not actually a filename
		pTexture->LoadFromFile(specification, GetResourceSystemPath(specification));
	}

	return pTexture;
}

S_API ITexture* DX11ResourcePool::GetCubeTexture(const string& file)
{
	if (file.empty())
		return 0;

	DX11Texture* pDXTexture = m_plTextures.GetBySpecification(file);
	if (!pDXTexture)
	{
		m_plTextures.AddItem(&pDXTexture, file);
		pDXTexture->D3D11_SetRenderer(m_pDXRenderer);
	}
	else
	{
		pDXTexture->AddRef();
	}

	ITexture* pTexture = pDXTexture;
	if (!pTexture->IsCubemap())
	{
		if (Failure(pTexture->LoadCubemapFromFile(file, GetResourceSystemPath(file))))
			CLog::Log(S_ERROR, "DX11ResourcePool::GetCubeTexture(): Failed DX11Texture::LoadCubemapFromFile()");
	}

	return pTexture;
}

S_API SResult DX11ResourcePool::ForEachTexture(IForEachHandler<ITexture*>* pForEachHandler)
{
	if (!IS_VALID_PTR(pForEachHandler))
		return S_INVALIDPARAM;

	// need to wrap ForEach() function of ChunkPool here, because we cannot implicitly convert
	// from DirectX11Texture* to ITexture*
	if (m_plTextures.GetUsedSlotCount() == 0)
		return S_SUCCESS;

	auto pChunks = m_plTextures.GetChunks();
	unsigned int nChunks = m_plTextures.GetChunkCount();
	for (unsigned int iChnk = 0; iChnk < nChunks; ++iChnk)
	{
		auto pChnk = pChunks[iChnk];
		if (pChnk->nUsedSlots == 0) continue;

		for (unsigned int iSlot = 0; iSlot < pChnk->nSlots; ++iSlot)
		{
			ChunkSlot<DX11Texture>* pSlot = &pChnk->pSlots[iSlot];
			if (!pSlot->bUsed) continue;
			
			ITexture* pTex = (ITexture*)&pSlot->instance;
			EForEachHandlerState state = pForEachHandler->Handle(&pTex);
			if (state == FOREACH_ABORT_ERR)
				return S_ERROR;
			else if (state == FOREACH_BREAK)
				return S_SUCCESS;
		}
	}

	return S_SUCCESS;
}

S_API void DX11ResourcePool::ListTextures(vector<string>& list) const
{
	auto pChunks = m_plTextures.GetChunks();
	unsigned int nChunks = m_plTextures.GetChunkCount();
	for (unsigned int iChnk = 0; iChnk < nChunks; ++iChnk)
	{
		auto pChnk = pChunks[iChnk];
		if (pChnk->nUsedSlots == 0) continue;

		for (unsigned int iSlot = 0; iSlot < pChnk->nSlots; ++iSlot)
		{
			ChunkSlot<DX11Texture>* pSlot = &pChnk->pSlots[iSlot];
			if (!pSlot->bUsed) continue;

			ITexture* pTex = (ITexture*)&pSlot->instance;
			list.push_back(pTex->GetSpecification());
		}
	}
}


SP_NMSPACE_END
