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

	pEngine = eng;
	pDXRenderer = (DirectX11Renderer*)renderer;

	return S_SUCCESS;
}

// **************************************************************************
//				VertexBuffer
// **************************************************************************

// **********************************************************************************

S_API SResult DirectX11ResourcePool::AddVertexBuffer(IVertexBuffer** pVBuffer, SP_ID* pUID)
{
	if (pEngine == NULL) return S_ABORTED;

	if (pDXRenderer == NULL)
		return pEngine->LogReport(S_ABORTED, "Cannot add Vertex Buffer Resource: Renderer not initialized!");

	DirectX11VertexBuffer dxVertexBuffer;
	DirectX11VertexBuffer* pdxVertexBuffer;

	if (NULL == (pdxVertexBuffer = plVertexBuffers.AddItem(dxVertexBuffer, pUID)))
		return S_ERROR;

	if (pVBuffer != NULL)
		*pVBuffer = (IVertexBuffer*)pdxVertexBuffer;

	return S_SUCCESS;
}

// **********************************************************************************

S_API IVertexBuffer* DirectX11ResourcePool::GetVertexBuffer(SP_ID iUID)
{
	if (pEngine == NULL) return NULL;

	DirectX11VertexBuffer* pdxVertexBuffer = plVertexBuffers.GetItemByUID(iUID);

	return (IVertexBuffer*)pdxVertexBuffer;
}

// **********************************************************************************

S_API SResult DirectX11ResourcePool::RemoveVertexBuffer(SP_ID iUID)
{
	plVertexBuffers.DeleteItem(iUID);

	return S_SUCCESS;
}

// **************************************************************************
//				IndexBuffer
// **************************************************************************

// **********************************************************************************

S_API SResult DirectX11ResourcePool::AddIndexBuffer(IIndexBuffer** pIBuffer, SP_ID* pUID)
{
	if (pEngine == NULL) return S_ABORTED;

	DirectX11IndexBuffer dxIndexBuffer;
	DirectX11IndexBuffer* pdxIndexBuffer;

	if (NULL == (pdxIndexBuffer = plIndexBuffers.AddItem(dxIndexBuffer, pUID)))
	{
		return S_ERROR;
	}

	if (pIBuffer != NULL)
		*pIBuffer = (IIndexBuffer*)pdxIndexBuffer;

	return S_SUCCESS;
}

// **********************************************************************************

S_API IIndexBuffer* DirectX11ResourcePool::GetIndexBuffer(SP_ID iUID)
{
	if (pDXRenderer == NULL || plIndexBuffers.GetSize() == 0) return NULL;

	DirectX11IndexBuffer* pdxIndexBuffer = plIndexBuffers.GetItemByUID(iUID);

	return (IIndexBuffer*)pdxIndexBuffer;
}

// **********************************************************************************

S_API SResult DirectX11ResourcePool::RemoveIndexBuffer(SP_ID iUID)
{
	if (pDXRenderer == NULL || plIndexBuffers.GetSize() == 0) return S_ABORTED;

	plIndexBuffers.DeleteItem(iUID);

	return S_SUCCESS;
}

// **************************************************************************
//				Texturing
// **************************************************************************

// **********************************************************************************

S_API SResult DirectX11ResourcePool::AddTexture(SString src, UINT w, UINT h, SString spec, ITexture** pTexture, SP_ID* pUID)
{
	if (pDXRenderer == NULL || (char*)src == NULL || w <= 64 || h <= 64 || (char*)spec == NULL)
	{
		return S_ABORTED;
	}

	DirectX11Texture dxTexture;
	DirectX11Texture* pdxTexture;

	if (NULL == (pdxTexture = plTextures.AddItem(dxTexture, pUID)))
	{
		return S_ERROR;
	}

	if (Failure(pdxTexture->Initialize(pEngine, spec, false)))
	{
		return S_ERROR;
	}

	if (Failure(pdxTexture->LoadFromFile(w, h, 5, src)))
	{
		return S_ERROR;
	}

	if (pTexture != NULL) *pTexture = (ITexture*)pdxTexture;

	return S_SUCCESS;
}

// **********************************************************************************

S_API SResult DirectX11ResourcePool::AddTexture(UINT w, UINT h, SString spec, S_TEXTURE_TYPE ty, ITexture** pTexture, SP_ID* pUID)
{
	if (pDXRenderer == NULL || w <= 0 || h <= 0 || (char*)spec == 0)
	{
		return S_ABORTED;
	}

	DirectX11Texture* pdxTexture;

	if (NULL == (pdxTexture = plTextures.AddItem(DirectX11Texture(), pUID)))
	{
		return S_ERROR;
	}

	if (Failure(pdxTexture->Initialize(pEngine, spec, true)))
	{
		return S_ERROR;
	}

	if (Failure(pdxTexture->CreateEmpty(w, h, 1, ty, SColor(0, 0, 0))))
	{
		return S_ERROR;
	}

	if (pTexture != NULL) *pTexture = (ITexture*)pdxTexture;

	return S_SUCCESS;
}

// **********************************************************************************

S_API ITexture* DirectX11ResourcePool::GetTexture(SP_ID iUID)
{
	if (pDXRenderer == NULL || plTextures.GetSize() <= 0) return NULL;

	DirectX11Texture* pdxTexture = plTextures.GetItemByUID(iUID);

	return (ITexture*)pdxTexture;
}

// **********************************************************************************

S_API ITexture* DirectX11ResourcePool::GetTexture(SString spec)
{
	if (pDXRenderer == NULL || plTextures.GetSize() <= 0) return NULL;

	DirectX11Texture* pdxTexture = NULL;

	UINT iItems = (UINT)plTextures.GetSize();
	for (UINT iItem = 0; iItem < iItems; ++iItem)
	{
		DirectX11Texture* pdxTempTexture = plTextures.GetItemByIndirectionIndex(iItem);
		if (pdxTempTexture->GetSpecification() == spec)
		{
			pdxTexture = pdxTempTexture;
			break;
		}
	}

	return (ITexture*)pdxTexture;
}

// **********************************************************************************

S_API char* DirectX11ResourcePool::GetTextureSpecification(SP_ID iUID)
{
	char* pRes = new char[1]; pRes[0] = 0;

	if (pDXRenderer == NULL || plTextures.GetSize() <= 0)
	{
		return pRes;
	}

	// Get the texture
	DirectX11Texture* pdxTexture = plTextures.GetItemByUID(iUID);

	if (pdxTexture == NULL)
	{
		return pRes;
	}

	SString sSpec = pdxTexture->GetSpecification();
	if (sSpec == 0 || sSpec.GetLength() == 0) return pRes;

	delete[] pRes;
	pRes = new char[sSpec.GetLength()];
	strcpy_s(pRes, sSpec.GetLength(), sSpec);
	return pRes;
}

// **********************************************************************************

S_API SP_ID DirectX11ResourcePool::GetTextureUID(SString spec)
{
	if (pDXRenderer == NULL || plTextures.GetSize() <= 0) return SP_ID();

	SPool<DirectX11Texture>::SPoolObject* pDXPoolObject = NULL;

	UINT iItems = (UINT)plTextures.GetSize();
	for (UINT iItem = 0; iItem < iItems; ++iItem)
	{
		DirectX11Texture* pdxTempTexture = plTextures.GetItemByIndirectionIndex(iItem);
		if (pdxTempTexture->GetSpecification() == spec)
		{
			pDXPoolObject = plTextures.GetPoolObjectByIndirectionIndex(iItem);
			break;
		}
	}

	if (pDXPoolObject == NULL) return SP_ID();

	SP_ID iRes;
	iRes.iIndex = pDXPoolObject->iIndirectionIndex;
	iRes.iVersion = pDXPoolObject->iVersion;

	return iRes;
}

// **********************************************************************************

S_API SResult DirectX11ResourcePool::RemoveTexture(SP_ID iUID)
{
	if (pDXRenderer == NULL || plTextures.GetSize() <= 0) return S_ABORTED;

	plTextures.DeleteItem(iUID);

	return S_SUCCESS;
}

// **********************************************************************************

S_API SResult DirectX11ResourcePool::ForEachTexture(void(*pIterationFunc)(ITexture*, const SP_ID&))
{
	if (pDXRenderer == NULL || plTextures.GetSize() <= 0) return S_ABORTED;

	UINT iItems = (UINT)plTextures.GetSize();
	for (UINT iItem = 0; iItem < iItems; ++iItem)
	{
		SPool<DirectX11Texture>::SPoolObject* pDXPoolObject
			= plTextures.GetPoolObjectByIndirectionIndex(iItem);

		SP_ID iCurrent;
		iCurrent.iIndex = pDXPoolObject->iIndirectionIndex;
		iCurrent.iVersion = pDXPoolObject->iVersion;

		pIterationFunc((ITexture*)&pDXPoolObject->tInstance, iCurrent);
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

	// Clear Index Buffers
	for (UINT iIndexBuffer = 0; iIndexBuffer < (UINT)plIndexBuffers.GetSize(); iIndexBuffer++)
	{
		DirectX11IndexBuffer* pDXIndexBuffer = plIndexBuffers.GetItemByIndirectionIndex(iIndexBuffer);
		if (pDXIndexBuffer == NULL)
		{
			res = S_ERROR;
			continue;
		}

		if (Failure(pDXIndexBuffer->Clear()))
			res = S_ERROR;
	}

	plIndexBuffers.Clear();

	// Clear Vertex Buffers
	for (UINT iVertexBuffer = 0; iVertexBuffer < (UINT)plVertexBuffers.GetSize(); iVertexBuffer++)
	{
		DirectX11VertexBuffer* pDXVertexBuffer = plVertexBuffers.GetItemByIndirectionIndex(iVertexBuffer);
		if (pDXVertexBuffer == NULL)
		{
			res = S_ERROR;
			continue;
		}

		if (Failure(pDXVertexBuffer->Clear()))
			res = S_ERROR;
	}

	plVertexBuffers.Clear();

	// Clear textures
	for (UINT iTexture = 0; iTexture < (UINT)plTextures.GetSize(); iTexture++)
	{
		DirectX11Texture* pDXTexture = plTextures.GetItemByIndirectionIndex(iTexture);
		if (pDXTexture == NULL)
		{
			res = S_ERROR;
			continue;
		}

		if (Failure(pDXTexture->Clear()))
			res = S_ERROR;
	}

	plTextures.Clear();

	pEngine = NULL;
	pDXRenderer = NULL;

	return res;
}


SP_NMSPACE_END