// SpeedPoint Index Buffer

#include <Implementation\DirectX9\SDirectX9IndexBuffer.h>
#include <Implementation\DirectX9\SDirectX9Renderer.h>
#include <SpeedPoint.h>

namespace SpeedPoint
{

	// *******************************************************************************************

	S_API SDirectX9IndexBuffer::~SDirectX9IndexBuffer()
	{
		// Make sure resources are freed
		Clear();
	}

	// *******************************************************************************************

	S_API BOOL SDirectX9IndexBuffer::IsInited ( void )
	{
		return
			pEngine != NULL &&
			pRenderer != NULL &&
			pHWIndexBuffer != NULL &&
			pShadowBuffer != NULL;
	}
	
	// *******************************************************************************************

	S_API SResult SDirectX9IndexBuffer::Initialize( int nSize, bool bDyn, SpeedPointEngine* engine, IRenderer* renderer, S_INDEXBUFFER_FORMAT format )
	{
		if( IsInited() )
			Clear();

		pEngine = engine;
		pRenderer = renderer;

		if( pEngine == NULL ) return S_ABORTED;

		if( Failure( Create( nSize, bDyn, format ) ) )
		{
			return pEngine->LogReport( S_ERROR, "Creation of the index buffer DX9 Resource failed!" );
		}

		return S_SUCCESS;
	}

	// *******************************************************************************************

	S_API SResult SDirectX9IndexBuffer::Create ( int nIndices_, bool bDynamic_, S_INDEXBUFFER_FORMAT format )
	{
		if( pEngine == NULL ) return S_ABORTED;
		
		if( pRenderer == NULL )
			return pEngine->LogReport( S_ABORTED, "Renderer not set while trying to create index buffer resource!" );
	
		nIndices = nIndices_;
		bDynamic = bDynamic_;
	
		fmtHWIndexBufferFormat = GetDXFormat( format );

		SDirectX9Renderer* pDXRenderer = (SDirectX9Renderer*)pRenderer;			
		if( FAILED( pDXRenderer->pd3dDevice->CreateIndexBuffer( nIndices * sizeof( DWORD ),
						      ((bDynamic) ? D3DUSAGE_DYNAMIC | D3DUSAGE_WRITEONLY : D3DUSAGE_WRITEONLY),
						      fmtHWIndexBufferFormat,
						      ((bDynamic) ? D3DPOOL_DEFAULT : D3DPOOL_MANAGED),
						      &pHWIndexBuffer,
						      NULL ) ) )
		{
			return pEngine->LogReport( S_ABORTED, "Could not create DX9 Hardware Index Buffer resource!" );
		}
	
		pShadowBuffer = (DWORD*)( malloc( nIndices * sizeof( DWORD ) ) );
	
		return S_SUCCESS;
	}
	
	// *******************************************************************************************
	
	//	Resizes Index Buffer
	
	S_API SResult SDirectX9IndexBuffer::Resize ( int nIndices_ )	
	{
		if( pEngine == NULL ) return S_ABORTED;

		if( nIndices_ == nIndices )
			return S_SUCCESS;
			
		if( pRenderer == NULL )
			return pEngine->LogReport( S_ABORTED, "Renderer not set while trying to resize Index Buffer!" );
	
		int nIndicesOld = nIndices;
		nIndices = nIndices_;	
		
		SDirectX9Renderer* pDXRenderer = (SDirectX9Renderer*)pRenderer;

		LPDIRECT3DINDEXBUFFER9 pIBTemp;
		if( FAILED( pDXRenderer->pd3dDevice->CreateIndexBuffer( nIndices * sizeof( DWORD ),
						      (bDynamic) ? D3DUSAGE_DYNAMIC | D3DUSAGE_WRITEONLY : D3DUSAGE_WRITEONLY,
						      fmtHWIndexBufferFormat,
						      (bDynamic) ? D3DPOOL_DEFAULT : D3DPOOL_MANAGED,
						      &pIBTemp,
						      NULL ) ) )
		{
			return pEngine->LogReport( S_ERROR, "Failed to create DX9 Hardware Index Buffer resource while trying to resize!" );
		}
	
		if( pHWIndexBuffer != NULL ) pHWIndexBuffer->Release();
		pHWIndexBuffer = pIBTemp;
	
		// Resize memory
		void* vBackup = (void*)(malloc( nIndicesOld * sizeof(DWORD) ) );
		memcpy( vBackup, pShadowBuffer, sizeof(DWORD) * nIndicesOld );	
		pShadowBuffer = (DWORD*)(malloc( nIndices * sizeof(DWORD) ) );
		memcpy( pShadowBuffer, vBackup, sizeof(DWORD) * nIndicesOld );
	
		// Now update bufferset
		void* pVertices;
		if( FAILED( pHWIndexBuffer->Lock( 0, nIndicesOld * sizeof(DWORD), (void**)&pVertices, ((!bDynamic) ? D3DLOCK_NOSYSLOCK : 0 ) ) ) )
			return pEngine->LogReport( S_ERROR, "Could not Lock resized Index Buffer resource" );
	
		memcpy( pVertices, (void*)pShadowBuffer, nIndicesOld * sizeof(DWORD) );
	
		if( FAILED( pHWIndexBuffer->Unlock() ) )
			return pEngine->LogReport( S_ERROR, "Could not unlock resized Index Buffer resource!" );
	
		return S_SUCCESS;
	}
	
	// *******************************************************************************************
	
	//	Locks Index Buffer
	
	S_API SResult SDirectX9IndexBuffer::Lock ( UINT iBegin, UINT iLength, void** buf )
	{
		return Lock( iBegin, iLength, buf, D3DLOCK_NOOVERWRITE );
	}
	
	// *******************************************************************************************
	
	//	Locks Index Buffer using specified flags
	
	S_API SResult SDirectX9IndexBuffer::Lock ( UINT iBegin, UINT iLength, void** buf, DWORD flags )
	{
		if( pEngine == NULL ) return S_ABORTED;

		if( IsInited() && pHWIndexBuffer != NULL && !bLocked )
		{		
			// check flags for HW Buffer Lock
			if (!bDynamic)
			{
				if (flags & D3DLOCK_DISCARD) flags &= ~D3DLOCK_DISCARD;
				if (flags & D3DLOCK_NOOVERWRITE) flags &= ~D3DLOCK_NOOVERWRITE;
				if (!(flags & D3DLOCK_NOSYSLOCK)) flags |= D3DLOCK_NOSYSLOCK;
			}

			// Now Fill the HW Index Buffer
			if( FAILED( pHWIndexBuffer->Lock( iBegin, iLength, buf, flags ) ) )
				return pEngine->LogReport( S_ERROR, "Could not lock DX9 Hardware Index Buffer resource" );
			else
				bLocked = true;

			// everything went well
			return S_SUCCESS;
		}

		return pEngine->LogReport( S_ABORTED, "Could not lock index buffer: Not initialized or already locked!" );
	}
	
	
	// *******************************************************************************************
	
	//	Fill vertex buffer
	
	S_API SResult SDirectX9IndexBuffer::Fill ( DWORD* vertices, int nIndices_, bool append )
	{
		if( pEngine == NULL ) return S_ABORTED;

		if( IsInited() && pHWIndexBuffer != NULL )
		{
			if( !bDynamic && nIndices_ > nIndices )		
				return pEngine->LogReport( S_ABORTED, "Cannot fill non-dynamic index buffer: Buffer overflow!" );
	
			if( append && nIndicesWritten > 0 && !bDynamic )
				return pEngine->LogReport( S_ABORTED, "Cannot append data to non-dynamic index buffer!" );
	
			if( nIndices_ + nIndicesWritten >  nIndices )
			{
				// Resize
				if( Failure( Resize( nIndices_ + nIndicesWritten ) ) )
					return pEngine->LogReport( S_ERROR, "Resize of index buffer failed while trying to fill!" );
			}		
	
			// Lock buffer if not happened
			void* pVert = 0;
			if( !bLocked )
			{
				SResult lockResult = Lock((UINT)(nIndicesWritten * sizeof(DWORD)), (UINT)(nIndices_ * sizeof(DWORD)), &pVert);
				if (Failure(lockResult) || pVert == 0)
					return pEngine->LogReport( S_ERROR, "Failed to lock index buffer while trying to fill!" );
			}
	
			// Now copy data		
			memcpy( pVert, (void*)vertices, nIndices_ * sizeof( DWORD ) );
	
			// backup data
			memcpy( (void*)&pShadowBuffer[nIndicesWritten], (void*)vertices, nIndices_ * sizeof( DWORD ) );
	
			nIndicesWritten += nIndices_;
	
			// ... and unlock
			if( FAILED( pHWIndexBuffer->Unlock() ) )
				return pEngine->LogReport( S_ERROR, "failed to unlock index buffer while trying to fill!" );
	
			bLocked = false;
	
			D3DINDEXBUFFER_DESC dsc;
			pHWIndexBuffer->GetDesc( &dsc );
			int vbs = dsc.Size;		
	
			return S_SUCCESS;
		}

		return pEngine->LogReport( S_ABORTED, "Cannot fill not-initialized index buffer!" );
	}
	
	// *******************************************************************************************
	
	//	Unlocks Index Buffer
	
	S_API SResult SDirectX9IndexBuffer::Unlock ( void )
	{
		if( pEngine == NULL ) return S_ABORTED;

		if( IsInited() && pHWIndexBuffer != NULL && bLocked )
		{
			if( FAILED( pHWIndexBuffer->Unlock() ) )
				return pEngine->LogReport( S_ERROR, "Failed to unlock DX9 Hardware index buffer resource!" );
			else
				bLocked = false;

			return S_SUCCESS;
		}

		return pEngine->LogReport( S_ABORTED, "Cannot unlock DX9 Hardware index buffer resource: Not initialized or not locked!" );
	}
	
	// *******************************************************************************************
	
	//	Clear and release Index Buffer
	
	S_API SResult SDirectX9IndexBuffer::Clear ( void )
	{
		if( pHWIndexBuffer != NULL ) pHWIndexBuffer->Release(); pHWIndexBuffer = NULL;
		if( pShadowBuffer != NULL ) free(pShadowBuffer); pShadowBuffer = 0;
		nIndices = 0;
		nIndicesWritten = 0;	
	
		return S_SUCCESS;
	}
	
	// *******************************************************************************************
	
	//	Get Pointer to Shadow Buffer
	
	S_API DWORD* SDirectX9IndexBuffer::GetShadowBuffer ( void )
	{
		if( IsInited() )
		{
			return pShadowBuffer;
		}

		return NULL;
	}
	
	// *******************************************************************************************
	
	//	Get specific Index by Index
	
	S_API DWORD* SDirectX9IndexBuffer::GetIndex ( int iIndex )
	{
		if( IsInited() && nIndices > iIndex && iIndex >= 0 )
		{
			return &pShadowBuffer[iIndex];
		}

		return NULL;
	}

	// *******************************************************************************************

	//	Get Index Count

	S_API INT SDirectX9IndexBuffer::GetIndexCount ( void )
	{
		return nIndices;
	}

	// *******************************************************************************************

	S_API D3DFORMAT SDirectX9IndexBuffer::GetDXFormat( S_INDEXBUFFER_FORMAT format )
	{
		D3DFORMAT fmtIndexBufferFormat;
		switch( format )
		{
		case S_INDEXBUFFER_16:
			fmtIndexBufferFormat = D3DFMT_INDEX16;
			break;

		case S_INDEXBUFFER_32:
			fmtIndexBufferFormat = D3DFMT_INDEX32;
			break;
		}

		return fmtIndexBufferFormat;
	}

	// *******************************************************************************************

	S_API S_INDEXBUFFER_FORMAT SDirectX9IndexBuffer::GetFormat( void )
	{
		switch( fmtHWIndexBufferFormat )
		{
		case D3DFMT_INDEX16:
			return S_INDEXBUFFER_16;
		default:
			return S_INDEXBUFFER_32;
		}
	}
}