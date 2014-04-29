
#pragma once
#include <Abstract\IIndexBuffer.h>
#include <d3d9.h>

namespace SpeedPoint
{
	// SpeedPoint DirectX9 IndexBuffer
	class S_API SDirectX9IndexBuffer : public IIndexBuffer
	{
	public:
		SpeedPointEngine*		pEngine;
		IRenderer*			pRenderer;
		LPDIRECT3DINDEXBUFFER9		pHWIndexBuffer;
		DWORD*				pShadowBuffer;
		D3DFORMAT			fmtHWIndexBufferFormat;	// we need to save this due to the resize function creating a new one
		int				nIndices;
		int				nIndicesWritten;
		bool				bDynamic;
		bool				bLocked;

		// Default constructor
		SDirectX9IndexBuffer()
			: pEngine( NULL ),
			pRenderer( NULL ),
			pHWIndexBuffer( NULL ),
			pShadowBuffer( NULL ),
			fmtHWIndexBufferFormat( D3DFMT_INDEX32 ),
			nIndices( 0 ),
			nIndicesWritten( 0 ),
			bDynamic( false ),
			bLocked( false ) {};

		// Copy constructor
		SDirectX9IndexBuffer( const SDirectX9IndexBuffer& o )
			: pEngine( o.pEngine ),
			pRenderer( o.pRenderer ),
			pHWIndexBuffer( o.pHWIndexBuffer ),
			pShadowBuffer( o.pShadowBuffer ),
			fmtHWIndexBufferFormat( o.fmtHWIndexBufferFormat ),
			nIndices( o.nIndices ),
			nIndicesWritten( o.nIndicesWritten ),
			bDynamic( o.bDynamic ),
			bLocked( o.bLocked ) {};

		// Default destructor
		~SDirectX9IndexBuffer();

		// Initialize the Index Buffer
		SResult Initialize( int nSize, bool bDyn, SpeedPointEngine* pEng, IRenderer* pRenderer, S_INDEXBUFFER_FORMAT format );

		// Create the Hardware Index Buffer
		SResult Create( int nIndices_, bool bDynamic_, S_INDEXBUFFER_FORMAT );

		// Check if this Index Buffer is inited properly
		BOOL IsInited( void );

		// Change the size of the Hardware Index Buffer
		SResult Resize( int nIndices_ );

		// Lock the Hardware Index Buffer in order to be able to fill Hardware data
		SResult Lock( UINT iBegin, UINT iLength, void** buf, DWORD flags );
		SResult Lock( UINT iBegin, UINT iLength, void** buf );		

		// Fill the Hardware Index Buffer with an array of indices
		SResult Fill( DWORD* indices, int nIndices_, bool append );

		// Unlock the Hardware Index Buffer
		SResult Unlock( void );
	
		// Get the RAM Copy of the hardware Index Buffer
		DWORD* GetShadowBuffer( void );

		// Get a Pointer to an Index
		DWORD* GetIndex( int iIndex );

		// Get the total count of all indices
		INT GetIndexCount( void );

		// Get the Index Buffer format specified when creating the Index Buffer
		S_INDEXBUFFER_FORMAT GetFormat( void );

		// Clear everything and free memory
		SResult Clear( void );

		// -- Functions that are not derived --

		// Convert S_INDEXBUFFER_FORMAT to D3DFORMAT
		D3DFORMAT GetDXFormat( S_INDEXBUFFER_FORMAT format );
	};
}