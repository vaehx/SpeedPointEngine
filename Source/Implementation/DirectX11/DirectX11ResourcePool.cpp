//////////////////////////////////////////////////////////////////////////////////////////////////
//
//	This file is part of the SpeedPoint Game Engine
//
//	written by Pascal R. aka iSmokiieZz
//	(c) 2011-2016, All rights reserved.
//
//////////////////////////////////////////////////////////////////////////////////////////////////

#include <Implementation\DirectX11\DirectX11ResourcePool.h>
#include <Implementation\DirectX11\DirectX11VertexBuffer.h>
#include <Implementation\DirectX11\DirectX11IndexBuffer.h>
#include <Implementation\DirectX11\DirectX11Texture.h>
#include <Implementation\DirectX11\DirectX11Shader.h>
//#include <SpeedPoint.h>
#include <Abstract\ISettings.h>
#include <SpeedPointEngine.h>


SP_NMSPACE_BEG


S_API string DirectX11ResourcePool::GetResourcePath(const string& file)
{
	IGameEngine* pEngine = SpeedPointEnv::GetEngine();
	if (!pEngine)
		return file;

	return pEngine->GetResourcePath(file);
}




// **************************************************************************
//				GENERAL
// **************************************************************************	

// **********************************************************************************

S_API SResult DirectX11ResourcePool::Initialize(IGameEngine* eng, IRenderer* renderer)
{
	if (eng == NULL || renderer == NULL) return S_ABORTED;

	m_pEngine = eng;
	m_pDXRenderer = (DirectX11Renderer*)renderer;

	CoInitialize(0);

	return S_SUCCESS;
}

// **************************************************************************
//				VertexBuffer
// **************************************************************************

// **********************************************************************************

S_API SResult DirectX11ResourcePool::AddVertexBuffer(IVertexBuffer** pVBuffer)
{
	if (m_pEngine == NULL) return S_ABORTED;

	if (m_pDXRenderer == NULL)
		return m_pEngine->LogReport(S_ABORTED, "Cannot add Vertex Buffer Resource: Renderer not initialized!");

	DirectX11VertexBuffer* pdxVertexBuffer;

	if (Failure(m_plVertexBuffers.AddItem(&pdxVertexBuffer)))
		return S_ERROR;

	SP_ASSERTR(IS_VALID_PTR(pdxVertexBuffer), S_ERROR);

	if (IS_VALID_PTR(pVBuffer))
		*pVBuffer = (IVertexBuffer*)pdxVertexBuffer;

	return S_SUCCESS;
}

// **********************************************************************************

S_API SResult DirectX11ResourcePool::RemoveVertexBuffer(IVertexBuffer** pVB)
{
	SP_ASSERTR(IS_VALID_PTR(pVB), S_INVALIDPARAM);

	if (IS_VALID_PTR(*pVB))
		(*pVB)->Clear();

	return m_plVertexBuffers.Delete((DirectX11VertexBuffer**)pVB);		
}

// **************************************************************************
//				IndexBuffer
// **************************************************************************

// **********************************************************************************

S_API SResult DirectX11ResourcePool::AddIndexBuffer(IIndexBuffer** pIBuffer)
{
	if (m_pEngine == NULL) return S_ABORTED;
	
	DirectX11IndexBuffer* pdxIndexBuffer;

	if (Failure(m_plIndexBuffers.AddItem(&pdxIndexBuffer)))
		return S_ERROR;

	SP_ASSERTR(IS_VALID_PTR(pdxIndexBuffer), S_ERROR);

	if (IS_VALID_PTR(pIBuffer))
		*pIBuffer = (IIndexBuffer*)pdxIndexBuffer;

	return S_SUCCESS;
}

// **********************************************************************************

S_API SResult DirectX11ResourcePool::RemoveIndexBuffer(IIndexBuffer** pIB)
{
	SP_ASSERTR(IS_VALID_PTR(pIB), S_INVALIDPARAM);

	if (IS_VALID_PTR(*pIB))
		(*pIB)->Clear();

	m_plIndexBuffers.Delete((DirectX11IndexBuffer**)pIB);

	return S_SUCCESS;
}

// **************************************************************************
//				Texturing
// **************************************************************************


S_API SResult DirectX11ResourcePool::AddTexture(const string& specification, ITexture** pTex, UINT w, UINT h, UINT mipLevels, const ETextureType& ty, const SColor& clearcolor)
{
	if (specification.empty())
		return CLog::Log(S_INVALIDPARAM, "DX11ResourcePool::AddTexture(): Empty specification");

	if (w == 0 || h == 0)
		return CLog::Log(S_INVALIDPARAM, "DX11ResourcePool::AddTexture(): width or height is 0");

	DirectX11Texture* pTexture = m_plTextures.GetBySpecification(specification);
	if (!pTexture)
		m_plTextures.AddItem(&pTexture, specification);

	if (Failure(pTexture->CreateEmpty(specification, w, h, mipLevels, ty, clearcolor)))
		return CLog::Log(S_ERROR, "DX11ResourcePool::AddTexture(): Failed DX11Texture::CreateEmpty()");

	if (pTex)
		*pTex = pTexture;

	return S_SUCCESS;
}

// **********************************************************************************

S_API ITexture* DirectX11ResourcePool::GetTexture(const string& specification)
{
	if (specification.empty())
		return 0;

	DirectX11Texture* pDXTexture = m_plTextures.GetBySpecification(specification);
	if (!pDXTexture)
		m_plTextures.AddItem(&pDXTexture, specification);

	ITexture* pTexture = pDXTexture;
	if (!pTexture->IsInitialized())
	{
		// This attempt may fail if the specification is not actually a filename
		pTexture->LoadFromFile(specification, GetResourcePath(specification));
	}

	return pTexture;
}

// **********************************************************************************

S_API ITexture* DirectX11ResourcePool::GetCubeTexture(const string& file)
{
	if (file.empty())
		return 0;

	DirectX11Texture* pDXTexture = m_plTextures.GetBySpecification(file);
	if (!pDXTexture)
		m_plTextures.AddItem(&pDXTexture, file);

	ITexture* pTexture = pDXTexture;
	if (!pTexture->IsCubemap())
	{
		if (Failure(pTexture->LoadCubemapFromFile(file, GetResourcePath(file))))
			CLog::Log(S_ERROR, "DX11ResourcePool::GetCubeTexture(): Failed DX11Texture::LoadCubemapFromFile()");
	}

	return pTexture;
}

// **********************************************************************************

S_API SResult DirectX11ResourcePool::RemoveTexture(const string& specification)
{
	if (specification.empty())
		return CLog::Log(S_INVALIDPARAM, "DX11ResourcePool::RemoveTexture(): Given specification empty");

	DirectX11Texture* pDXTexture = m_plTextures.GetBySpecification(specification);
	if (pDXTexture)
	{
		pDXTexture->Clear();
		return S_SUCCESS;
	}
	else
	{
		return S_NOTFOUND;
	}
}

// **********************************************************************************

S_API SResult DirectX11ResourcePool::RemoveTexture(ITexture** pTex)
{
	if (!pTex)
		return S_INVALIDPARAM;

	DirectX11Texture* pDXTexture = dynamic_cast<DirectX11Texture*>(*pTex);
	if (pDXTexture)
	{
		// Make sure the texture is in the pool
		ChunkSlot<DirectX11Texture>* pSlot = m_plTextures.GetSlot(pDXTexture);
		if (pSlot)
			pDXTexture->Clear();
	}

	*pTex = 0;

	return S_SUCCESS;
}

// **********************************************************************************

S_API SResult DirectX11ResourcePool::ForEachTexture(IForEachHandler<ITexture*>* pForEachHandler)
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
			ChunkSlot<DirectX11Texture>* pSlot = &pChnk->pSlots[iSlot];
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

// **********************************************************************************

S_API void DirectX11ResourcePool::ListTextures(vector<string>& list) const
{
	auto pChunks = m_plTextures.GetChunks();
	unsigned int nChunks = m_plTextures.GetChunkCount();
	for (unsigned int iChnk = 0; iChnk < nChunks; ++iChnk)
	{
		auto pChnk = pChunks[iChnk];
		if (pChnk->nUsedSlots == 0) continue;

		for (unsigned int iSlot = 0; iSlot < pChnk->nSlots; ++iSlot)
		{
			ChunkSlot<DirectX11Texture>* pSlot = &pChnk->pSlots[iSlot];
			if (!pSlot->bUsed) continue;

			ITexture* pTex = (ITexture*)&pSlot->instance;
			list.push_back(pTex->GetSpecification());
		}
	}
}



// **************************************************************************
//				All
// **************************************************************************

// **********************************************************************************

S_API SResult DirectX11ResourcePool::ClearAll(VOID)
{
	SResult res = S_SUCCESS;

	// Destructors of instances are called by the pool when the slot buffer is destroyed
	m_plVertexBuffers.Clear();
	m_plIndexBuffers.Clear();
	//m_plShaders.Clear();
	m_plTextures.Clear();	

	m_pEngine = NULL;
	m_pDXRenderer = NULL;

	CoUninitialize();

	return res;
}


SP_NMSPACE_END