// ********************************************************************************************

//	SpeedPoint IndexBuffer Resource

//	This is a virtual class. You cannot instantiate it.
//	Please use Specific Implementation to instantiate.

// ********************************************************************************************

#pragma once

#include <SPrerequisites.h>

namespace SpeedPoint
{
	// SpeedPoint IndexBuffer Resource (abstract)
	class S_API IIndexBuffer
	{
	public:
		// Initialize the Index Buffer
		virtual SResult Initialize( int nSize, bool bDynamic, SpeedPointEngine* pEngine, IRenderer* pRenderer, S_INDEXBUFFER_FORMAT format ) = 0;

		// Create the Hardware Index Buffer
		virtual SResult Create( int nIndices_, bool bDynamic_, S_INDEXBUFFER_FORMAT ) = 0;

		// Check if this Index Buffer is inited properly
		virtual BOOL IsInited( void ) = 0;

		// Change the size of the Hardware Index Buffer
		virtual SResult Resize( int nIndices_ ) = 0;

		// Lock the Hardware Index Buffer in order to be able to fill Hardware data
		virtual SResult Lock( UINT iBegin, UINT iLength, void** buf, DWORD flags ) = 0;
		virtual SResult Lock( UINT iBegin, UINT iLength, void** buf ) = 0;		

		// Fill the Hardware Index Buffer with an array of indices
		virtual SResult Fill( DWORD* indices, int nIndices_, bool append ) = 0;

		// Unlock the Hardware Index Buffer
		virtual SResult Unlock( void ) = 0;
	
		// Get the RAM Copy of the hardware Index Buffer
		virtual DWORD* GetShadowBuffer( void ) = 0;

		// Get a Pointer to an Index
		virtual DWORD* GetIndex( int iIndex ) = 0;

		// Get the total count of all indices
		virtual INT GetIndexCount( void ) = 0;

		// Get the Index Buffer format specified when creating the Index Buffer
		virtual S_INDEXBUFFER_FORMAT GetFormat( void ) = 0;

		// Clear everything and free memory
		virtual SResult Clear( void ) = 0;
	};
}