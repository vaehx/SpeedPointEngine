//////////////////////////////////////////////////////////////////////////////////////////////////
//
//	This file is part of the SpeedPoint Game Engine
//
//	written by Pascal R. aka iSmokiieZz
//	(c) 2011-2014, All rights reserved.
//
//////////////////////////////////////////////////////////////////////////////////////////////////

#include <Implementation\DirectX11\DirectX11ResourcePool.h>
#include <Implementation\DirectX11\DirectX11VertexBuffer.h>
#include <Implementation\DirectX11\DirectX11IndexBuffer.h>
#include <Implementation\DirectX11\DirectX11Texture.h>
#include <Implementation\DirectX11\DirectX11Effect.h>
//#include <SpeedPoint.h>
#include <SpeedPointEngine.h>


SP_NMSPACE_BEG




// **************************************************************************
//				GENERAL
// **************************************************************************	

// **********************************************************************************

S_API SResult DirectX11ResourcePool::Initialize(IGameEngine* eng, IRenderer* renderer, const string& resourceBasePath)
{
	if (eng == NULL || renderer == NULL) return S_ABORTED;

	m_pEngine = eng;
	m_pDXRenderer = (DirectX11Renderer*)renderer;

	m_BasePath = resourceBasePath;

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

// **********************************************************************************

S_API SResult DirectX11ResourcePool::LoadTexture(const string& specification, ITexture** ppTexture, const string& file /*=""*/, UINT w /*=0*/, UINT h /*=0*/, bool bDynamic /*=false*/, bool bStaged /*=false*/)
{	
	SP_ASSERTRD(IS_VALID_PTR(m_pDXRenderer) && IS_VALID_PTR(m_pEngine), S_NOTINIT,
		"Cannot load Texture (file=%s spec=%s): Resource Pool not initialized.", file.c_str(), specification.c_str());

	if (specification.empty())
		return CLog::Log(S_ERROR, "Cannot load texture! Empty specification given!");

	string filePath = m_BasePath + (m_BasePath.empty() ? "" : "\\");
	if (!file.empty())
		filePath += file;
	else
		filePath += specification;


	ITexture* pTex = GetTexture(specification);

	// The texture might exist already
	pTex->Clear();

	if (Failure(pTex->Initialize(m_pEngine, specification, bDynamic, bStaged)))
		return S_ERROR;
	
	if (Failure(pTex->LoadFromFile(w, h, 5, filePath.c_str())))
		return EngLog(S_ERROR, m_pEngine, "Failed load texture file='%s' spec='%s'!", filePath.c_str(), specification.c_str());

	CLog::Log(S_DEBUG, "Loaded Texture file='%s' spec='%s', dyn=%d, staged=%d", filePath.c_str(), specification.c_str(), bDynamic, bStaged);

	if (IS_VALID_PTR(ppTexture))
		*ppTexture = pTex;

	return S_SUCCESS;
}

// **********************************************************************************

S_API SResult DirectX11ResourcePool::LoadCubeTexture(const string& specification, ITexture** ppTexture, const string& src /*=""*/, UINT w /*=0*/, UINT h /*=0*/)
{
	SP_ASSERTRD(IS_VALID_PTR(m_pDXRenderer) && IS_VALID_PTR(m_pEngine), S_NOTINIT,
		"Cannot load Cubetexture (%s): Resource Pool not initialized.", src.c_str());

	if (specification.empty())
		return CLog::Log(S_ERROR, "Empty cubemap specification specified src='%s'!", src.c_str());

	string filePath = m_BasePath + (m_BasePath.empty() ? "" : "\\");
	if (!src.empty())
		filePath += src;
	else
		filePath += specification;


	ITexture* pTex = GetTexture(specification);

	// Texture may be loaded already
	pTex->Clear();

	if (Failure(pTex->Initialize(m_pEngine, specification, false, false)))
		return S_ERROR;

	if (Failure(pTex->LoadCubemapFromFile(w, h, filePath.c_str())))
		return EngLog(S_ERROR, m_pEngine, "Failed load cubemap file='%s' spec='%s'!", filePath.c_str(), specification.c_str());

	CLog::Log(S_DEBUG, "Loaded Cubemap '%s'", specification.c_str());

	if (ppTexture != NULL)
		*ppTexture = pTex;

	return S_SUCCESS;
}

// **********************************************************************************

S_API SResult DirectX11ResourcePool::AddTexture(const string& spec, ITexture** ppTexture, UINT w, UINT h, const ETextureType& ty, const SColor& clearcolor, bool bDynamic, bool bStaged)
{
	SP_ASSERTRD(IS_VALID_PTR(m_pDXRenderer) && IS_VALID_PTR(m_pEngine), S_NOTINIT,
		"Cannot add Texture (%s): Resource Pool not initialized.", spec.c_str());

	if (w == 0 || h == 0)
		return CLog::Log(S_ERROR, "Tried add texture with width or height == 0", S_ERROR);

	if (spec.empty())
		return CLog::Log(S_ERROR, "Empty specification given for new texture!");


	ITexture* pTex = GetTexture(spec);

	pTex->Clear();

	if (Failure(pTex->Initialize(m_pEngine, spec, bDynamic, bStaged)))
		return S_ERROR;

	if (Failure(pTex->CreateEmpty(w, h, 1, ty, clearcolor)))
		return S_ERROR;

	CLog::Log(S_DEBUG, "Added Texture spec=%s, dyn=%d, staged=%d", spec.c_str(), bDynamic, bStaged);

	if (ppTexture != NULL)
		*ppTexture = pTex;

	return S_SUCCESS;
}

// **********************************************************************************

S_API ITexture* DirectX11ResourcePool::GetTexture(const string& spec)
{
	DirectX11Texture* pDXTexture = m_plTextures.GetBySpecification(spec);
	if (!IS_VALID_PTR(pDXTexture))
	{
		m_plTextures.AddItem(&pDXTexture, spec);
	}

	return (ITexture*)pDXTexture;
}

// **********************************************************************************

S_API SResult DirectX11ResourcePool::RemoveTexture(ITexture** pTexture)
{
	if (!IS_VALID_PTR(pTexture))
		return S_INVALIDPARAM;

	DirectX11Texture** pDXTexture = (DirectX11Texture**)pTexture;
	return m_plTextures.Delete(pDXTexture);
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