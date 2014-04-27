// SpeedPoint DirectX9 Resource Pool

#include <Implementation\DirectX9\SDirectX9ResourcePool.h>
#include <Implementation\DirectX9\SDirectX9VertexBuffer.h>
#include <Implementation\DirectX9\SDirectX9IndexBuffer.h>
#include <Implementation\DirectX9\SDirectX9Texture.h>
#include <Implementation\DirectX9\SDirectX9Shader.h>
#include <SpeedPoint.h>

namespace SpeedPoint
{

	// **************************************************************************
	//				GENERAL
	// **************************************************************************	

	// **********************************************************************************
	
	S_API SResult SDirectX9ResourcePool::Initialize( SpeedPointEngine* eng, SRenderer* renderer )
	{
		if( eng == NULL || renderer == NULL ) return S_ABORTED;

		pEngine = eng;
		pDXRenderer = (SDirectX9Renderer*)renderer;

		return S_SUCCESS;
	}

	// **************************************************************************
	//				VertexBuffer
	// **************************************************************************

	// **********************************************************************************
	
	S_API SResult SDirectX9ResourcePool::AddVertexBuffer( SVertexBuffer** pVBuffer, SP_ID* pUID )
	{
		if( pEngine == NULL ) return S_ABORTED;

		if( pDXRenderer == NULL )
			return pEngine->LogReport( S_ABORTED, "Cannot add Vertex Buffer Resource: Renderer not initialized!" );

		SDirectX9VertexBuffer dxVertexBuffer;
		SDirectX9VertexBuffer* pdxVertexBuffer;

		if( NULL == ( pdxVertexBuffer = plVertexBuffers.AddItem( dxVertexBuffer, pUID ) ) )
			return S_ERROR;

		if( pVBuffer != NULL )
			*pVBuffer = (SVertexBuffer*)pdxVertexBuffer;

		return S_SUCCESS;
	}

	// **********************************************************************************

	S_API SVertexBuffer* SDirectX9ResourcePool::GetVertexBuffer( SP_ID iUID )
	{
		if( pEngine == NULL ) return NULL;

		SDirectX9VertexBuffer* pdxVertexBuffer = plVertexBuffers.GetItemByUID( iUID );

		return (SVertexBuffer*)pdxVertexBuffer;
	}

	// **********************************************************************************
	
	S_API SResult SDirectX9ResourcePool::RemoveVertexBuffer( SP_ID iUID )
	{
		plVertexBuffers.DeleteItem( iUID );

		return S_SUCCESS;
	}

	// **************************************************************************
	//				IndexBuffer
	// **************************************************************************

	// **********************************************************************************

	S_API SResult SDirectX9ResourcePool::AddIndexBuffer( SIndexBuffer** pIBuffer, SP_ID* pUID )
	{
		if( pEngine == NULL ) return S_ABORTED;

		SDirectX9IndexBuffer dxIndexBuffer;
		SDirectX9IndexBuffer* pdxIndexBuffer;

		if( NULL == ( pdxIndexBuffer = plIndexBuffers.AddItem( dxIndexBuffer, pUID ) ) )
		{
			return S_ERROR;
		}

		if( pIBuffer != NULL )
			*pIBuffer = (SIndexBuffer*)pdxIndexBuffer;

		return S_SUCCESS;
	}

	// **********************************************************************************

	S_API SIndexBuffer* SDirectX9ResourcePool::GetIndexBuffer( SP_ID iUID )	
	{
		if( pDXRenderer == NULL || plIndexBuffers.GetSize() == 0 ) return NULL;

		SDirectX9IndexBuffer* pdxIndexBuffer = plIndexBuffers.GetItemByUID( iUID );
		
		return (SIndexBuffer*)pdxIndexBuffer;
	}

	// **********************************************************************************

	S_API SResult SDirectX9ResourcePool::RemoveIndexBuffer( SP_ID iUID )
	{
		if( pDXRenderer == NULL || plIndexBuffers.GetSize() == 0 ) return S_ABORTED;

		plIndexBuffers.DeleteItem( iUID );

		return S_SUCCESS;
	}

	// **************************************************************************
	//				Texturing
	// **************************************************************************

	// **********************************************************************************

	S_API SResult SDirectX9ResourcePool::AddTexture( SString src, UINT w, UINT h, SString spec, STexture** pTexture, SP_ID* pUID )
	{
		if( pDXRenderer == NULL || (char*)src == NULL || w <= 64 || h <= 64 || (char*)spec == NULL )
		{
			return S_ABORTED;
		}

		SDirectX9Texture dxTexture;
		SDirectX9Texture* pdxTexture;		

		if( NULL == ( pdxTexture = plTextures.AddItem( dxTexture, pUID ) ) )
		{
			return S_ERROR;
		}

		if( Failure( pdxTexture->Initialize( pEngine, spec, false ) ) )
		{
			return S_ERROR;
		}

		if( Failure( pdxTexture->LoadFromFile( w, h, 5, src ) ) )
		{
			return S_ERROR;
		}

		if( pTexture != NULL ) *pTexture = (STexture*)pdxTexture;

		return S_SUCCESS;
	}

	// **********************************************************************************

	S_API SResult SDirectX9ResourcePool::AddTexture( UINT w, UINT h, SString spec, S_TEXTURE_TYPE ty, STexture** pTexture, SP_ID* pUID )
	{
		if( pDXRenderer == NULL || w <= 0 || h <= 0 || (char*)spec == 0 )
		{
			return S_ABORTED;
		}

		SDirectX9Texture* pdxTexture;

		if( NULL == ( pdxTexture = plTextures.AddItem( SDirectX9Texture(), pUID ) ) )
		{
			return S_ERROR;
		}

		if( Failure( pdxTexture->Initialize( pEngine, spec, true ) ) )
		{
			return S_ERROR;
		}

		if( Failure( pdxTexture->CreateEmpty( w, h, 1, ty, SColor( 0, 0, 0 ) ) ) )
		{
			return S_ERROR;
		}

		if( pTexture != NULL ) *pTexture = (STexture*)pdxTexture;

		return S_SUCCESS;
	}

	// **********************************************************************************

	S_API STexture* SDirectX9ResourcePool::GetTexture( SP_ID iUID )
	{
		if( pDXRenderer == NULL || plTextures.GetSize() <= 0 ) return NULL;

		SDirectX9Texture* pdxTexture = plTextures.GetItemByUID( iUID );

		return (STexture*)pdxTexture;
	}

	// **********************************************************************************

	S_API STexture* SDirectX9ResourcePool::GetTexture( SString spec )
	{
		if( pDXRenderer == NULL || plTextures.GetSize() <= 0 ) return NULL;

		SDirectX9Texture* pdxTexture = NULL;

		UINT iItems = (UINT)plTextures.GetSize();
		for( UINT iItem = 0; iItem < iItems; ++iItem )
		{
			SDirectX9Texture* pdxTempTexture = plTextures.GetItemByIndirectionIndex( iItem );
			if( pdxTempTexture->GetSpecification() == spec )
			{
				pdxTexture = pdxTempTexture;
				break;
			}
		}

		return (STexture*)pdxTexture;
	}

	// **********************************************************************************

	S_API char* SDirectX9ResourcePool::GetTextureSpecification( SP_ID iUID )
	{
		char* pRes = new char[1]; pRes[0] = 0;		

		if( pDXRenderer == NULL || plTextures.GetSize() <= 0 )
		{			
			return pRes;
		}

		// Get the texture
		SDirectX9Texture* pdxTexture = plTextures.GetItemByUID( iUID );				

		if( pdxTexture == NULL )
		{
			return pRes;
		}

		SString sSpec = pdxTexture->GetSpecification();
		if( sSpec == 0 || sSpec.GetLength() == 0 ) return pRes;

		delete[] pRes;
		pRes = new char[sSpec.GetLength()];
		strcpy_s( pRes, sSpec.GetLength(), sSpec );
		return pRes;
	}

	// **********************************************************************************

	S_API SP_ID SDirectX9ResourcePool::GetTextureUID( SString spec )
	{
		if( pDXRenderer == NULL || plTextures.GetSize() <= 0 ) return SP_ID();

		SPool<SDirectX9Texture>::SPoolObject* pDXPoolObject = NULL;

		UINT iItems = (UINT)plTextures.GetSize();
		for( UINT iItem = 0; iItem < iItems; ++iItem )
		{
			SDirectX9Texture* pdxTempTexture = plTextures.GetItemByIndirectionIndex( iItem );
			if( pdxTempTexture->GetSpecification() == spec )
			{
				pDXPoolObject = plTextures.GetPoolObjectByIndirectionIndex( iItem );
				break;
			}
		}

		if( pDXPoolObject == NULL ) return SP_ID();

		SP_ID iRes;
		iRes.iIndex = pDXPoolObject->iIndirectionIndex;
		iRes.iVersion = pDXPoolObject->iVersion;

		return iRes;
	}

	// **********************************************************************************

	S_API SResult SDirectX9ResourcePool::RemoveTexture( SP_ID iUID )
	{
		if( pDXRenderer == NULL || plTextures.GetSize() <= 0 ) return S_ABORTED;

		plTextures.DeleteItem( iUID );

		return S_SUCCESS;
	}

	// **********************************************************************************

	S_API SResult SDirectX9ResourcePool::ForEachTexture( void (*pIterationFunc)(STexture*, const SP_ID&) )
	{
		if( pDXRenderer == NULL || plTextures.GetSize() <= 0 ) return S_ABORTED;

		UINT iItems = (UINT)plTextures.GetSize();
		for( UINT iItem = 0; iItem < iItems; ++iItem )
		{
			SPool<SDirectX9Texture>::SPoolObject* pDXPoolObject
				= plTextures.GetPoolObjectByIndirectionIndex( iItem );

			SP_ID iCurrent;
			iCurrent.iIndex = pDXPoolObject->iIndirectionIndex;
			iCurrent.iVersion = pDXPoolObject->iVersion;

			pIterationFunc( (STexture*)&pDXPoolObject->tInstance, iCurrent );
		}

		return S_SUCCESS;
	}
	

	// **************************************************************************
	//				All
	// **************************************************************************

	// **********************************************************************************

	S_API SResult SDirectX9ResourcePool::ClearAll ( VOID )
	{
		SResult res = S_SUCCESS;

		// Clear Index Buffers
		for( UINT iIndexBuffer = 0; iIndexBuffer < (UINT)plIndexBuffers.GetSize(); iIndexBuffer++ )
		{
			SDirectX9IndexBuffer* pDXIndexBuffer = plIndexBuffers.GetItemByIndirectionIndex( iIndexBuffer );
			if( pDXIndexBuffer == NULL )
			{
				res = S_ERROR;
				continue;
			}
			
			if( Failure( pDXIndexBuffer->Clear() ) )			
				res = S_ERROR;
		}

		plIndexBuffers.Clear();

		// Clear Vertex Buffers
		for( UINT iVertexBuffer = 0; iVertexBuffer < (UINT)plVertexBuffers.GetSize(); iVertexBuffer++ )
		{
			SDirectX9VertexBuffer* pDXVertexBuffer = plVertexBuffers.GetItemByIndirectionIndex( iVertexBuffer );
			if( pDXVertexBuffer == NULL )
			{
				res = S_ERROR;
				continue;
			}

			if( Failure( pDXVertexBuffer->Clear() ) )
				res = S_ERROR;
		}

		plVertexBuffers.Clear();

		// Clear textures
		for( UINT iTexture = 0; iTexture < (UINT)plTextures.GetSize(); iTexture++ )
		{
			SDirectX9Texture* pDXTexture = plTextures.GetItemByIndirectionIndex( iTexture );
			if( pDXTexture == NULL )
			{
				res = S_ERROR;
				continue;
			}

			if( Failure( pDXTexture->Clear() ) )
				res = S_ERROR;
		}

		plTextures.Clear();

		pEngine = NULL;
		pDXRenderer = NULL;

		return res;
	}

}