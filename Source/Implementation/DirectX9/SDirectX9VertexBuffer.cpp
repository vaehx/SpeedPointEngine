// SpeedPoint Vertex Buffer

#include <Implementation\DirectX9\SDirectX9VertexBuffer.h>
#include <Implementation\DirectX9\SDirectX9Renderer.h>
#include <SVertex.h>
#include <SpeedPoint.h>

namespace SpeedPoint
{

	// *******************************************************************************************

	S_API BOOL SDirectX9VertexBuffer::IsInited ( void )
	{
		return
			pEngine != NULL &&
			pRenderer != NULL &&
			pHWVertexBuffer != NULL &&
			pShadowBuffer != NULL;
	}
	
	// *******************************************************************************************

	S_API SResult SDirectX9VertexBuffer::Initialize( int nSize, bool bDyn, SpeedPointEngine* engine, SRenderer* renderer )
	{
		if( IsInited() )
			Clear();

		pEngine = engine;
		pRenderer = renderer;

		if( Failure( Create( nSize, bDyn ) ) )
		{
			return S_ERROR;
		}

		return S_SUCCESS;
	}

	// *******************************************************************************************

	S_API SResult SDirectX9VertexBuffer::Create ( int nVertices_, bool bDynamic_ )
	{
		if( pEngine == NULL ) return S_ABORTED;

		if( pRenderer == NULL )
			return pEngine->LogReport( S_ABORTED, "No Renderer given when initializing. While creating vertex buffer!" );
	
		nVertices = nVertices_;
		bDynamic = bDynamic_;
	
		SDirectX9Renderer* pDXRenderer = (SDirectX9Renderer*)pRenderer;

		if( FAILED( pDXRenderer->pd3dDevice->CreateVertexBuffer( nVertices * sizeof( SVertex ),
						      ((bDynamic) ? D3DUSAGE_DYNAMIC | D3DUSAGE_WRITEONLY : D3DUSAGE_WRITEONLY),
						      pDXRenderer->GetFVF(),
						      ((bDynamic) ? D3DPOOL_DEFAULT : D3DPOOL_MANAGED),
						      &pHWVertexBuffer,
						      NULL ) ) )
		{			
			return pEngine->LogReport( S_ERROR, "Could not create DX9 Vertex Buffer resource!" );
		}
	
		pShadowBuffer = (SVertex*)( malloc( nVertices * sizeof( SVertex ) ) );
	
		return S_SUCCESS;
	}
	
	// *******************************************************************************************
	
	//	Resizes Vertex Buffer
	
	S_API SResult SDirectX9VertexBuffer::Resize ( int nVertices_ )
	{
		if( nVertices_ == nVertices )
			return S_SUCCESS;
	
		if( pEngine == NULL ) return S_ABORTED;

		if( pRenderer == NULL )
			return pEngine->LogReport( S_ABORTED, "Renderer not set while trying to resize Vertex Buffer!" );
	
		int nVerticesOld = nVertices;
		nVertices = nVertices_;	
		
		SDirectX9Renderer* pDXRenderer = (SDirectX9Renderer*)pRenderer;

		PDIRECT3DVERTEXBUFFER9 pVBTemp;
		if( FAILED( pDXRenderer->pd3dDevice->CreateVertexBuffer( nVertices * sizeof( SVertex ),
						      (bDynamic) ? D3DUSAGE_DYNAMIC | D3DUSAGE_WRITEONLY : D3DUSAGE_WRITEONLY,
						      pRenderer->GetFVF(),
						      (bDynamic) ? D3DPOOL_DEFAULT : D3DPOOL_MANAGED,
						      &pVBTemp,
						      NULL ) ) )
		{
			return pEngine->LogReport( S_ERROR, "Failed create resized VertexBuffer while trying to resize vertex buffer!" );
		}
	
		if( pHWVertexBuffer != NULL ) pHWVertexBuffer->Release();
		pHWVertexBuffer = pVBTemp;
	
		// Resize memory
		void* vBackup = (void*)(malloc( nVerticesOld * sizeof(SVertex) ) );
		memcpy( vBackup, pShadowBuffer, sizeof(SVertex) * nVerticesOld );	
		pShadowBuffer = (SVertex*)(malloc( nVertices * sizeof(SVertex) ) );
		memcpy( pShadowBuffer, vBackup, sizeof(SVertex) * nVerticesOld );
	
		// Now update bufferset
		void* pVertices;
		
		if( FAILED( pHWVertexBuffer->Lock( 0, nVerticesOld * sizeof(SVertex), (void**)&pVertices, ((!bDynamic) ? D3DLOCK_NOSYSLOCK : 0 ) ) ) )			
			return pEngine->LogReport( S_ERROR, "Failed to lock resized Vertex Buffer!" );		
	
		memcpy( pVertices, (void*)pShadowBuffer, nVerticesOld * sizeof(SVertex) );
	
		if( FAILED( pHWVertexBuffer->Unlock() ) )
			return pEngine->LogReport( S_ERROR, "Failed to unlock resized Vertex Buffer!" );
	
		return S_SUCCESS;
	}
	
	// *******************************************************************************************
	
	//	Locks Vertex Buffer
	
	S_API SResult SDirectX9VertexBuffer::Lock ( UINT iBegin, UINT iLength, void** buf )
	{
		return Lock( iBegin, iLength, buf, D3DLOCK_NOOVERWRITE );	
	}
	
	// *******************************************************************************************
	
	//	Locks Vertex Buffer using specified flags
	
	S_API SResult SDirectX9VertexBuffer::Lock ( UINT iBegin, UINT iLength, void** buf, DWORD flags )
	{
		if( pEngine == NULL ) return S_ABORTED;

		if( IsInited() && pHWVertexBuffer != NULL && !bLocked )
		{		
			if( FAILED( pHWVertexBuffer->Lock( iBegin, iLength, buf, ((!bDynamic) ? D3DLOCK_NOSYSLOCK | flags : flags ) ) ) )
				return pEngine->LogReport( S_ERROR, "Failed lock DX9 Hardware Vertex Buffer Resource!" );
			else
				bLocked = true;

			return S_SUCCESS;
		}

		return pEngine->LogReport( S_ABORTED, "Could not lock vertex buffer: Not initialized or already locked!" );
	}
	
	
	// *******************************************************************************************
	
	//	Fill vertex buffer
	
	S_API SResult SDirectX9VertexBuffer::Fill ( SVertex* vertices, int nVertices_, bool append )
	{
		if( pEngine == NULL ) return S_ABORTED;

		if( IsInited() && pHWVertexBuffer != NULL )
		{
			if( !bDynamic && nVertices_ > nVertices )		
				return pEngine->LogReport( S_ABORTED, "Cannot fill non-dynamic vertex buffer: Buffer overflow!" );
	
			if( append && nVerticesWritten > 0 && !bDynamic )
				return pEngine->LogReport( S_ABORTED, "Cannot append Vertex data to non-dynamic Vertex Buffer" );				
	
			if( nVertices_ + nVerticesWritten >  nVertices )
			{
				// Resize
				if( Failure( Resize( nVertices_ + nVerticesWritten ) ) )
					return pEngine->LogReport( S_ERROR, "Resize for fill vertex buffer failed!" );
			}		
	
			// Lock buffer if not happened
			void* pVert;
			if( !bLocked )
			{
				if( Failure( Lock( (UINT)(nVerticesWritten * sizeof(SVertex)), (UINT)(nVertices_ * sizeof(SVertex)), &pVert ) ) )
					return pEngine->LogReport( S_ERROR, "Could not lock vertex buffer to fill with vertex data!" );
			}
	
			// Now copy data		
			memcpy( pVert, (void*)vertices, nVertices_ * sizeof( SVertex ) );
	
			// backup data
			memcpy( (void*)&pShadowBuffer[nVerticesWritten], (void*)vertices, nVertices_ * sizeof( SVertex ) );
	
			nVerticesWritten += nVertices_;
	
			// ... and unlock
			if( FAILED( pHWVertexBuffer->Unlock() ) )
				return pEngine->LogReport( S_ERROR, "Could not unlock DX9 Vertex Buffer resource while filling with vertex data!" );
	
			bLocked = false;
	
			D3DVERTEXBUFFER_DESC dsc;
			pHWVertexBuffer->GetDesc( &dsc );
			int vbs = dsc.Size;		
	
			return S_SUCCESS;
		}

		return pEngine->LogReport( S_ABORTED, "Cannot Fill vertex buffer that is not initialized!" );
	}
	
	// *******************************************************************************************
	
	//	Unlocks Vertex Buffer
	
	S_API SResult SDirectX9VertexBuffer::Unlock ( void )
	{
		if( pEngine ) return S_ABORTED;

		if( IsInited() && pHWVertexBuffer != NULL && bLocked )
		{
			if( FAILED( pHWVertexBuffer->Unlock() ) )
				return pEngine->LogReport( S_ERROR, "Could not unlock DX9 vertex buffer resource!" );
			else
				bLocked = false;

			return S_SUCCESS;
		}

		return pEngine->LogReport( S_ABORTED, "Cannot unlock vertex buffer that is not initialized!" );
	}
	
	// *******************************************************************************************
	
	//	Clear and release Vertex Buffer
	
	S_API SResult SDirectX9VertexBuffer::Clear ( void )
	{
		if( IsInited() )
		{
			if( pHWVertexBuffer != NULL ) pHWVertexBuffer->Release(); pHWVertexBuffer = NULL;
			if( pShadowBuffer != NULL ) free( pShadowBuffer );	
			nVertices = 0;
			nVerticesWritten = 0;
		}
	
		return S_SUCCESS;
	}
	
	// *******************************************************************************************
	
	//	Get Pointer to Shadow Buffer
	
	S_API SVertex* SDirectX9VertexBuffer::GetShadowBuffer ( void )
	{
		if( IsInited() )
		{
			return pShadowBuffer;
		}

		return NULL;
	}
	
	// *******************************************************************************************
	
	//	Get specific Vertex by Index
	
	S_API SVertex* SDirectX9VertexBuffer::GetVertex ( int iIndex )
	{
		if( IsInited() && nVertices > iIndex && iIndex >= 0 )
		{
			return &pShadowBuffer[iIndex];
		}

		return NULL;
	}

	// *******************************************************************************************

	//	Get Vertex Count

	S_API INT SDirectX9VertexBuffer::GetVertexCount ( void )
	{
		return nVertices;
	}

}