// SpeedPoint Vertex Buffer

#include <Implementation\DirectX9\SDirectX9VertexBuffer.h>
#include <Implementation\DirectX9\SDirectX9Renderer.h>
#include <SVertex.h>

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
		if( pEngine == NULL || pRenderer == NULL ) return S_ERROR;
	
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
			return S_ERROR;
			//return SUtil::error( "Failed to Create Vertexbuffer!" );
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
	
		if( pEngine == NULL || pRenderer == NULL ) return S_ABORTED;
	
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
			return S_ERROR;
			//return SUtil::error( "Failed to Resize Vertexbuffer: Buffer creation Failed!" );
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
			return S_ERROR;
			//return SUtil::error( "Failed to Resize Vertexbuffer: Lock Failed!" );
	
		memcpy( pVertices, (void*)pShadowBuffer, nVerticesOld * sizeof(SVertex) );
	
		if( FAILED( pHWVertexBuffer->Unlock() ) )
			return S_ERROR;
			//return SUtil::error( "Failed to Resize VertexBuffer: Unlock Failed!" );
	
		return S_SUCCESS;
	}
	
	// *******************************************************************************************
	
	//	Locks Vertex Buffer
	
	S_API SResult SDirectX9VertexBuffer::Lock ( UINT iBegin, UINT iLength, void** buf )
	{
		if( IsInited() && pHWVertexBuffer != NULL && !bLocked )
		{		
			if( FAILED( pHWVertexBuffer->Lock( iBegin, iLength, buf, ((!bDynamic) ? D3DLOCK_NOSYSLOCK : D3DLOCK_NOOVERWRITE ) ) ) )
				return S_ERROR;
				//return SUtil::error( "Failed to Lock Vertexbuffer!" );
			else
				bLocked = true;

			return S_SUCCESS;
		}

		return S_ERROR;
	}
	
	// *******************************************************************************************
	
	//	Locks Vertex Buffer using specified flags
	
	S_API SResult SDirectX9VertexBuffer::Lock ( UINT iBegin, UINT iLength, void** buf, DWORD flags )
	{
		if( IsInited() && pHWVertexBuffer != NULL && !bLocked )
		{		
			if( FAILED( pHWVertexBuffer->Lock( iBegin, iLength, buf, ((!bDynamic) ? D3DLOCK_NOSYSLOCK | flags : flags ) ) ) )
				return S_ERROR;
				//return SUtil::error( "Failed to Lock Vertexbuffer!" );
			else
				bLocked = true;

			return S_SUCCESS;
		}

		return S_ERROR;
	}
	
	
	// *******************************************************************************************
	
	//	Fill vertex buffer
	
	S_API SResult SDirectX9VertexBuffer::Fill ( SVertex* vertices, int nVertices_, bool append )
	{
		if( IsInited() && pHWVertexBuffer != NULL )
		{
			if( !bDynamic && nVertices_ > nVertices )		
				return S_ERROR;	// too big		
	
			if( append && nVerticesWritten > 0 && !bDynamic )
				return S_ERROR;	// Buffer not expandable	
	
			if( nVertices_ + nVerticesWritten >  nVertices )
			{
				// Resize
				Resize( nVertices_ + nVerticesWritten );
			}		
	
			// Lock buffer if not happened
			void* pVert;
			if( !bLocked )
			{
				if( FAILED( Lock( (UINT)(nVerticesWritten * sizeof(SVertex)), (UINT)(nVertices_ * sizeof(SVertex)), &pVert ) ) )
					return S_ERROR;
			}
	
			// Now copy data		
			memcpy( pVert, (void*)vertices, nVertices_ * sizeof( SVertex ) );
	
			// backup data
			memcpy( (void*)&pShadowBuffer[nVerticesWritten], (void*)vertices, nVertices_ * sizeof( SVertex ) );
	
			nVerticesWritten += nVertices_;
	
			// ... and unlock
			if( FAILED( pHWVertexBuffer->Unlock() ) )
				return S_ERROR;
	
			bLocked = false;
	
			D3DVERTEXBUFFER_DESC dsc;
			pHWVertexBuffer->GetDesc( &dsc );
			int vbs = dsc.Size;		
	
			return S_SUCCESS;
		}

		return S_ERROR;
	}
	
	// *******************************************************************************************
	
	//	Unlocks Vertex Buffer
	
	S_API SResult SDirectX9VertexBuffer::Unlock ( void )
	{
		if( IsInited() && pHWVertexBuffer != NULL && bLocked )
		{
			if( FAILED( pHWVertexBuffer->Unlock() ) )
				return S_ERROR;
			else
				bLocked = false;

			return S_SUCCESS;
		}

		return S_ERROR;
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