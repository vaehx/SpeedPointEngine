// ********************************************************************************************

//	SpeedPoint IndexBuffer Resource

//	This is a virtual class. You cannot instantiate it.
//	Please use Specific Implementation to instantiate.

// ********************************************************************************************

#pragma once

#include <SPrerequisites.h>


SP_NMSPACE_BEG

struct S_API IRenderer;
struct S_API IGameEngine;


// SpeedPoint IndexBuffer Format
enum S_API S_INDEXBUFFER_FORMAT
{
	S_INDEXBUFFER_16
	//S_INDEXBUFFER_32,	( not implemented yet )
};



enum S_API EIBLockType
{
	eIBLOCK_DISCARD,
	eIBLOCK_NOOVERWRITE
};


// Usage type of a indexbuffer to let the engine get the most optimization
enum EIBUsage
{
	eIBUSAGE_STATIC,		// the buffer has initial data and never changes until destruction
	eIBUSAGE_DYNAMIC_RARE,		// the buffer content changes less than once per frame
	eIBUSAGE_DYNAMIC_FREQUENT	// the buffer content changes more than once per frame
};


	
#ifdef _WIN32
typedef unsigned __int16 S_API SIndex;

// TODO: Add separation into SIndex16 and SIndex32

#else
typedef unsigned int S_API SIndex;
#endif


// SpeedPoint IndexBuffer Resource (abstract)
struct S_API IIndexBuffer
{
public:
	// Summary:
	//	Initialize the Index Buffer
	// Arguments:
	//	pInitialData - Specifies inital buffer data. If 0, then nSize and format is ignored and the buffers are created as soon as Fill() is called.
	virtual SResult Initialize(IGameEngine* pEngine, IRenderer* pRenderer, EIBUsage usage, int nSize, S_INDEXBUFFER_FORMAT format, SIndex* pInitialData = nullptr) = 0;
	virtual SResult Initialize(IGameEngine* pEngine, IRenderer* pRenderer, EIBUsage usage, int nSize, SIndex* pInitialData = nullptr) = 0;

	// Create the Hardware Index Buffer
	virtual SResult Create( int nIndices_, SIndex* pInitialData = nullptr, usint32 nInitialDataCount = 0) = 0;

	// Check if this Index Buffer is inited properly
	virtual BOOL IsInited( void ) = 0;

	// Change the size of the Hardware Index Buffer
	virtual SResult Resize( int nIndices_ ) = 0;

	// Lock the Hardware Index Buffer in order to be able to fill Hardware data
	virtual SResult Lock( UINT iBegin, UINT iLength, SIndex** buf, EIBLockType flags ) = 0;
	virtual SResult Lock( UINT iBegin, UINT iLength, SIndex** buf ) = 0;		

	// Fill the Hardware Index Buffer with an array of indices
	virtual SResult Fill( SIndex* indices, int nIndices_, bool append ) = 0;

	// Unlock the Hardware Index Buffer
	virtual SResult Unlock( void ) = 0;
	
	// Get the RAM Copy of the hardware Index Buffer
	virtual SIndex* GetShadowBuffer( void ) = 0;

	// Get a Pointer to an Index
	virtual SIndex* GetIndex( int iIndex ) = 0;

	// Get the total count of all indices
	virtual unsigned long GetIndexCount( void ) const = 0;

	// Get the Index Buffer format specified when creating the Index Buffer
//virtual S_INDEXBUFFER_FORMAT GetFormat( void ) = 0;

	// Clear everything and free memory
	virtual SResult Clear( void ) = 0;
};


SP_NMSPACE_END