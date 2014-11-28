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

S_API SResult DirectX11ResourcePool::Initialize(IGameEngine* eng, IRenderer* renderer)
{
	if (eng == NULL || renderer == NULL) return S_ABORTED;

	m_pEngine = eng;
	m_pDXRenderer = (DirectX11Renderer*)renderer;

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

S_API SResult DirectX11ResourcePool::LoadTexture(const SString& src, UINT w, UINT h, const SString& spec, ITexture** pTexture)
{	
	SP_ASSERTRD(IS_VALID_PTR(m_pDXRenderer) && IS_VALID_PTR(m_pEngine), S_NOTINIT,
		"Cannot load Texture (%s): Resource Pool not initialized.", (spec.IsValidString() ? (char*)spec : ""));

	if (w <= 64 || h <= 64)
		return m_pEngine->LogD("Cannot load textures with width or height < 64", S_ERROR);	

	if (!src.IsValidString())
		return m_pEngine->LogE("Invalid texture src specified: " + src);

	if (!spec.IsValidString())
		m_pEngine->LogW("Invalid or no specification for loaded texture (" + src + ")!");	

	DirectX11Texture* pdxTexture;

	if (Failure(m_plTextures.AddItem(&pdxTexture)) || !IS_VALID_PTR(pdxTexture))
		return m_pEngine->LogE("Failed add texture (" + src + ")");

	if (Failure(pdxTexture->Initialize(m_pEngine, spec, false)))
	{
		return S_ERROR;
	}

	if (Failure(pdxTexture->LoadFromFile(w, h, 5, src)))
	{
		return S_ERROR;
	}

	m_pEngine->LogD("Loaded Texture " + src + ", spec: '" + spec + "'");

	if (pTexture != NULL)
		*pTexture = (ITexture*)pdxTexture;

	return S_SUCCESS;
}

// **********************************************************************************

S_API SResult DirectX11ResourcePool::AddTexture(UINT w, UINT h, const SString& spec, S_TEXTURE_TYPE ty, ITexture** pTexture)
{
	SP_ASSERTRD(IS_VALID_PTR(m_pDXRenderer) && IS_VALID_PTR(m_pEngine), S_NOTINIT,
		"Cannot add Texture (%s): Resource Pool not initialized.", (spec.IsValidString() ? (char*)spec : ""));

	if (w <= 64 || h <= 64)
		return m_pEngine->LogD("Tried add texture with width or height < 64", S_ERROR);

	if (!spec.IsValidString())
		m_pEngine->LogW("Invalid or no specification for new texture!");

	DirectX11Texture* pdxTexture;

	if (Failure(m_plTextures.AddItem(&pdxTexture)) || !IS_VALID_PTR(pdxTexture))
		return m_pEngine->LogE("Failed add texture (" + spec + ")");

	if (Failure(pdxTexture->Initialize(m_pEngine, spec, false)))
	{
		return S_ERROR;
	}

	if (Failure(pdxTexture->CreateEmpty(w, h, 1, ty, SColor(0, 0, 0))))
	{
		return S_ERROR;
	}

	m_pEngine->LogD("Added Texture " + spec + "'");

	if (pTexture != NULL)
		*pTexture = (ITexture*)pdxTexture;

	return S_SUCCESS;
}

// **********************************************************************************

S_API ITexture* DirectX11ResourcePool::GetTexture(const SString& spec)
{
	if (!IS_VALID_PTR(m_pDXRenderer) || !m_plTextures.HasElements())
		return nullptr;

	DirectX11Texture* pDXTexture = m_plTextures.GetBySpecification(spec);
	return (ITexture*)pDXTexture;
}

// **********************************************************************************

S_API SString DirectX11ResourcePool::GetTextureSpecification(const ITexture* pTexture) const
{
	char* pRes = new char[1]; pRes[0] = 0;

	if (m_plTextures.GetUsedSlotCount() <= 0)
		return "???";

	DirectX11Texture* pDXTex = (DirectX11Texture*)pTexture;	// probably does reinterpret_cast ??
	auto pSlot = m_plTextures.GetSlot(pDXTex);
	if (!IS_VALID_PTR(pSlot))
		return "???";

	return SString(pSlot->specification);
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

	return res;
}


SP_NMSPACE_END