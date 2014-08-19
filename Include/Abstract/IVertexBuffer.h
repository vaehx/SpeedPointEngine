// ********************************************************************************************

//	SpeedPoint VertexBuffer Resource

//	This is a virtual class. You cannot instantiate it.
//	Please use Specific Implementation to instantiate.

// ********************************************************************************************

#pragma once

#include <SPrerequisites.h>

SP_NMSPACE_BEG



struct S_API SVertex;
struct S_API IRenderer;
struct S_API IGameEngine;


enum S_API EVBLockType
{
	eVBLOCK_DISCARD,
	eVBLOCK_NOOVERWRITE
};


// Usage type of a vertexbuffer to let the engine get the most optimization
enum S_API EVBUsage
{
	eVBUSAGE_STATIC,		// the buffer has initial data and never changes until destruction
	eVBUSAGE_DYNAMIC_RARE,		// the buffer content changes less than once per frame
	eVBUSAGE_DYNAMIC_FREQUENT	// the buffer content changes more than once per frame
};



// SpeedPoint VertexBuffer Resource (abstract)
struct S_API IVertexBuffer
{
public:
	// Summary:
	//	Initialize the vertex buffer
	// Arguments:
	//	nSize - spezifies the count of pInitialData if set. If pInitialData is 0 then nSize is ignored and the buffer is created
	//		as soon as Fill() is called.
	virtual SResult Initialize(IGameEngine* pEngine, IRenderer* renderer, EVBUsage usage, int nSize, SVertex* pInitialData = nullptr) = 0;

	// Summary:
	//	 Create the Hardware Vertex Buffer
	// Arguments:
	//	nInitialDataCount - if 0, pInitialData is not copied into buffer. Function fails if nInitialDatacount is greater than nSize
	virtual SResult Create( int nSize, SVertex* pInitalData = nullptr, usint32 nInitialDataCount = 0 ) = 0;

	// Check if this Vertex Buffer is inited properly
	virtual BOOL IsInited( void ) = 0;

	// Change the size of the Hardware Vertex Buffer
	virtual SResult Resize( int nNewSize ) = 0;

	// Lock the Hardware Vertex Buffer in order to be able to fill Hardware data
	virtual SResult Lock( UINT iBegin, UINT iLength, SVertex** buf, EVBLockType lockType ) = 0;
	virtual SResult Lock( UINT iBegin, UINT iLength, SVertex** buf ) = 0;		

	// Fill the Hardware Vertex Buffer with an array of vertices
	virtual SResult Fill( SVertex* pVertices, int nVertices, bool append ) = 0;

	// Unlock the Hardware Vertex Buffer
	virtual SResult Unlock( void ) = 0;
	
	// Get the RAM Copy of the hardware Vertex Buffer
	virtual SVertex* GetShadowBuffer( void ) = 0;

	// Get a Pointer to an Vertex
	virtual SVertex* GetVertex( int iVertex ) = 0;

	// Get the total count of all vertices
	virtual INT GetVertexCount( void ) = 0;

	// Clear everything and free memory
	virtual SResult Clear( void ) = 0;
};



SP_NMSPACE_END