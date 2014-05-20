// ********************************************************************************************

//	SpeedPoint VertexBuffer Resource

//	This is a virtual class. You cannot instantiate it.
//	Please use Specific Implementation to instantiate.

// ********************************************************************************************

#pragma once

#include <SPrerequisites.h>

SP_NMSPACE_BEG



class S_API SVertex;
class S_API SpeedPointEngine;
class S_API IRenderer;


enum EVBLockType
{
	eVBLOCK_DISCARD,
	eVBLOCK_NOOVERWRITE
};



// SpeedPoint VertexBuffer Resource (abstract)
class S_API IVertexBuffer
{
public:
	// Initialize the vertex buffer
	virtual SResult Initialize( int nSize, bool bDynamic, SpeedPointEngine* pEng, IRenderer* renderer ) = 0;

	// Create the Hardware Vertex Buffer
	virtual SResult Create( int nSize, bool bDynamic_ ) = 0;

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