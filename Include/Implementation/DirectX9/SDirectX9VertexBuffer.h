// ********************************************************************************************

//	SpeedPoint VertexBuffer Resource

// ********************************************************************************************


#pragma once

#include <Abstract\IVertexBuffer.h>
#include <d3d9.h>

namespace SpeedPoint
{
	// SpeedPoint VertexBuffer Resource
	class S_API SDirectX9VertexBuffer : public IVertexBuffer
	{
	public:
		SpeedPointEngine*		pEngine;
		IRenderer*			pRenderer;
		LPDIRECT3DVERTEXBUFFER9		pHWVertexBuffer;
		SVertex*			pShadowBuffer;
		int				nVertices;	
		int				nVerticesWritten;
		bool				bDynamic;	
		bool				bLocked;		

		// Default constructor
		SDirectX9VertexBuffer()
			: pEngine( NULL ),
			pRenderer( NULL ),
			pHWVertexBuffer( NULL ),
			pShadowBuffer( NULL ),
			nVertices( 0 ),
			nVerticesWritten( 0 ),
			bDynamic( false ),
			bLocked( false ) {};

		// Copy constructor
		SDirectX9VertexBuffer( const SDirectX9VertexBuffer& o )
			: pEngine( o.pEngine ),
			pRenderer( o.pRenderer ),
			pHWVertexBuffer( o.pHWVertexBuffer ),
			pShadowBuffer( o.pShadowBuffer ),
			nVertices( o.nVertices ),
			nVerticesWritten( o.nVerticesWritten ),
			bDynamic( o.bDynamic ),
			bLocked( o.bLocked ) {};

		// Default destructor
		~SDirectX9VertexBuffer();

		// Initialize the vertex buffer
		SResult Initialize( int nSize, bool bDynamic, SpeedPointEngine* pEng, IRenderer* renderer );

		// Create the Hardware Vertex Buffer
		SResult Create( int nSize, bool bDynamic_ );

		// Check if this Vertex Buffer is inited properly
		BOOL IsInited( void );

		// Change the size of the Hardware Vertex Buffer
		SResult Resize( int nNewSize );

		// Lock the Hardware Vertex Buffer in order to be able to fill Hardware data
		SResult Lock( UINT iBegin, UINT iLength, void** buf, DWORD flags );
		SResult Lock( UINT iBegin, UINT iLength, void** buf );		

		// Fill the Hardware Vertex Buffer with an array of vertices
		SResult Fill( SVertex* pVertices, int nVertices, bool append );

		// Unlock the Hardware Vertex Buffer
		SResult Unlock( void );
	
		// Get the RAM Copy of the hardware Vertex Buffer
		SVertex* GetShadowBuffer( void );

		// Get a Pointer to an Vertex
		SVertex* GetVertex( int iVertex );

		// Get the total count of all vertices
		INT GetVertexCount( void );

		// Clear everything and free memory
		SResult Clear( void );
	};
}