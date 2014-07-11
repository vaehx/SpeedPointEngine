// ********************************************************************************************

//	SpeedPoint IndexBuffer Resource

//	This is a virtual class. You cannot instantiate it.
//	Please use Specific Implementation to instantiate.

// ********************************************************************************************

#pragma once

#include <SPrerequisites.h>


SP_NMSPACE_BEG

class S_API SpeedPointEngine;
struct S_API IRenderer;


// SpeedPoint IndexBuffer Format
enum S_API S_INDEXBUFFER_FORMAT
{
	S_INDEXBUFFER_16,
	S_INDEXBUFFER_32,
};



enum S_API EIBLockType
{
	eIBLOCK_DISCARD,
	eIBLOCK_NOOVERWRITE
};

	
#ifdef _WIN32
typedef unsigned __int16 S_API SIndex;
#else
typedef unsigned int S_API SIndex;
#endif


// SpeedPoint IndexBuffer Resource (abstract)
struct S_API IIndexBuffer
{
public:
	// Initialize the Index Buffer
	virtual SResult Initialize( int nSize, bool bDynamic, SpeedPointEngine* pEngine, IRenderer* pRenderer, S_INDEXBUFFER_FORMAT format ) = 0;

	// Create the Hardware Index Buffer
	virtual SResult Create( int nIndices_, bool bDynamic_ ) = 0;

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
	virtual INT GetIndexCount( void ) = 0;

	// Get the Index Buffer format specified when creating the Index Buffer
	virtual S_INDEXBUFFER_FORMAT GetFormat( void ) = 0;

	// Clear everything and free memory
	virtual SResult Clear( void ) = 0;
};


SP_NMSPACE_END