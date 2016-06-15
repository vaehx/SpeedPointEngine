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
	S_INDEXBUFFER_16,
	S_INDEXBUFFER_32
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
typedef unsigned long S_API SLargeIndex;

#else
typedef unsigned int S_API SIndex;
#endif


// SpeedPoint IndexBuffer Resource (interface)
struct S_API IIndexBuffer
{
public:
	// Summary:
	//	Initialize the Index Buffer
	// Arguments:
	//	pInitialData - Specifies inital buffer data. If 0, then nSize and format is ignored and the buffers are created as soon as Fill() is called.
	virtual SResult Initialize(IRenderer* pRenderer, EIBUsage usage, unsigned long nSize, S_INDEXBUFFER_FORMAT format, void* pInitialData = nullptr) = 0;
	virtual SResult Initialize(IRenderer* pRenderer, EIBUsage usage, unsigned long nSize, SIndex* pInitialData = nullptr) = 0;

	// Create the Hardware Index Buffer
	virtual SResult Create( unsigned long nIndices_, void* pInitialData = nullptr, usint32 nInitialDataCount = 0) = 0;

	// Check if this Index Buffer is inited properly
	virtual BOOL IsInited( void ) = 0;

	// Change the size of the Hardware Index Buffer
	virtual SResult Resize( unsigned long nIndices_ ) = 0;

	// Lock the Hardware Index Buffer in order to be able to fill Hardware data
	virtual SResult Lock( UINT iBegin, UINT iLength, void** buf, EIBLockType flags ) = 0;
	virtual SResult Lock( UINT iBegin, UINT iLength, void** buf ) = 0;		

	// Fill the Hardware Index Buffer with an array of indices
	virtual SResult Fill( void* indices, unsigned long nIndices_, bool append ) = 0;

	// Unlock the Hardware Index Buffer
	virtual SResult Unlock( void ) = 0;
	
	// Get the RAM Copy of the hardware Index Buffer
	virtual void* GetShadowBuffer( void ) = 0;


	// This function returns the value at the correct index, but casts it to SIndex.
	// !!In case you're using a 32bit Index Buffer, you should be using GetLargeIndex()!!
	virtual SIndex* GetIndex(unsigned long iIndex) = 0;

	virtual SLargeIndex* GetLargeIndex(unsigned long iIndex) = 0;

	// Get the total count of all indices
	virtual unsigned long GetIndexCount( void ) const = 0;

	// Get the Index Buffer format specified when creating the Index Buffer
	virtual S_INDEXBUFFER_FORMAT GetFormat() const = 0;

	// Clear everything and free memory
	virtual SResult Clear( void ) = 0;
};


SP_NMSPACE_END