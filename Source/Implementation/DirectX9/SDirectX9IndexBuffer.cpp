// SpeedPoint Index Buffer

#include <Implementation\DirectX9\SDirectX9IndexBuffer.h>
#include <Implementation\DirectX9\SDirectX9Renderer.h>

namespace SpeedPoint
{

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

	S_API SResult SDirectX9IndexBuffer::Initialize( int nSize, bool bDyn, SpeedPointEngine* engine, SRenderer* renderer, S_INDEXBUFFER_FORMAT format )
	{
		if( IsInited() )
			Clear();

		pEngine = engine;
		pRenderer = renderer;

		if( Failure( Create( nSize, bDyn, format ) ) )
		{
			return S_ERROR;
		}

		return S_SUCCESS;
	}

	// *******************************************************************************************

	S_API SResult SDirectX9IndexBuffer::Create ( int nIndices_, bool bDynamic_, S_INDEXBUFFER_FORMAT format )
	{
		if( pEngine == NULL || pRenderer == NULL ) return S_ERROR;
	
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
			return S_ERROR;
			//return SUtil::error( "Failed to Create Indexbuffer!" );
		}
	
		pShadowBuffer = (DWORD*)( malloc( nIndices * sizeof( DWORD ) ) );
	
		return S_SUCCESS;
	}
	
	// *******************************************************************************************
	
	//	Resizes Index Buffer
	
	S_API SResult SDirectX9IndexBuffer::Resize ( int nIndices_ )
	{
		if( nIndices_ == nIndices )
			return S_SUCCESS;
	
		if( pEngine == NULL || pRenderer == NULL ) return S_ABORTED;
	
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
			return S_ERROR;
			//return SUtil::error( "Failed to Resize Indexbuffer: Buffer creation Failed!" );
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
			return S_ERROR;
			//return SUtil::error( "Failed to Resize Indexbuffer: Lock Failed!" );
	
		memcpy( pVertices, (void*)pShadowBuffer, nIndicesOld * sizeof(DWORD) );
	
		if( FAILED( pHWIndexBuffer->Unlock() ) )
			return S_ERROR;
			//return SUtil::error( "Failed to Resize IndexBuffer: Unlock Failed!" );
	
		return S_SUCCESS;
	}
	
	// *******************************************************************************************
	
	//	Locks Index Buffer
	
	S_API SResult SDirectX9IndexBuffer::Lock ( UINT iBegin, UINT iLength, void** buf )
	{
		if( IsInited() && pHWIndexBuffer != NULL && !bLocked )
		{		
			if( FAILED( pHWIndexBuffer->Lock( iBegin, iLength, buf, ((!bDynamic) ? D3DLOCK_NOSYSLOCK : D3DLOCK_NOOVERWRITE ) ) ) )
				return S_ERROR;
				//return SUtil::error( "Failed to Lock Indexbuffer!" );
			else
				bLocked = true;

			return S_SUCCESS;
		}

		return S_ERROR;
	}
	
	// *******************************************************************************************
	
	//	Locks Index Buffer using specified flags
	
	S_API SResult SDirectX9IndexBuffer::Lock ( UINT iBegin, UINT iLength, void** buf, DWORD flags )
	{
		if( IsInited() && pHWIndexBuffer != NULL && !bLocked )
		{		
			if( FAILED( pHWIndexBuffer->Lock( iBegin, iLength, buf, ((!bDynamic) ? D3DLOCK_NOSYSLOCK | flags : flags ) ) ) )
				return S_ERROR;
				//return SUtil::error( "Failed to Lock Indexbuffer!" );
			else
				bLocked = true;

			return S_SUCCESS;
		}

		return S_ERROR;
	}
	
	
	// *******************************************************************************************
	
	//	Fill vertex buffer
	
	S_API SResult SDirectX9IndexBuffer::Fill ( DWORD* vertices, int nIndices_, bool append )
	{
		if( IsInited() && pHWIndexBuffer != NULL )
		{
			if( !bDynamic && nIndices_ > nIndices )		
				return S_ERROR;	// too big		
	
			if( append && nIndicesWritten > 0 && !bDynamic )
				return S_ERROR;	// Buffer not expandable	
	
			if( nIndices_ + nIndicesWritten >  nIndices )
			{
				// Resize
				Resize( nIndices_ + nIndicesWritten );
			}		
	
			// Lock buffer if not happened
			void* pVert;
			if( !bLocked )
			{
				if( FAILED( Lock( (UINT)(nIndicesWritten * sizeof(DWORD)), (UINT)(nIndices_ * sizeof(DWORD)), &pVert ) ) )
					return S_ERROR;
			}
	
			// Now copy data		
			memcpy( pVert, (void*)vertices, nIndices_ * sizeof( DWORD ) );
	
			// backup data
			memcpy( (void*)&pShadowBuffer[nIndicesWritten], (void*)vertices, nIndices_ * sizeof( DWORD ) );
	
			nIndicesWritten += nIndices_;
	
			// ... and unlock
			if( FAILED( pHWIndexBuffer->Unlock() ) )
				return S_ERROR;
	
			bLocked = false;
	
			D3DINDEXBUFFER_DESC dsc;
			pHWIndexBuffer->GetDesc( &dsc );
			int vbs = dsc.Size;		
	
			return S_SUCCESS;
		}

		return S_ERROR;
	}
	
	// *******************************************************************************************
	
	//	Unlocks Index Buffer
	
	S_API SResult SDirectX9IndexBuffer::Unlock ( void )
	{
		if( IsInited() && pHWIndexBuffer != NULL && bLocked )
		{
			if( FAILED( pHWIndexBuffer->Unlock() ) )
				return S_ERROR;
			else
				bLocked = false;

			return S_SUCCESS;
		}

		return S_ERROR;
	}
	
	// *******************************************************************************************
	
	//	Clear and release Index Buffer
	
	S_API SResult SDirectX9IndexBuffer::Clear ( void )
	{
		if( IsInited() )
		{
			if( pHWIndexBuffer != NULL ) pHWIndexBuffer->Release(); pHWIndexBuffer = NULL;
			if( pShadowBuffer != NULL ) free( pShadowBuffer );	
			nIndices = 0;
			nIndicesWritten = 0;
		}
	
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