// ********************************************************************************************

//	SpeedPoint indirection based Pool

// ********************************************************************************************

#pragma once
#include <Abstract\SAPI.h>

namespace SpeedPoint
{
	// Index struct for SpeedPoint Array
	struct S_API SPoolIndex
	{
		int iIndex;
		int iVersion;

		// Default constructor
		SPoolIndex() : iIndex(-1), iVersion(-1) {};

		// Copy constructor
		SPoolIndex( const SPoolIndex& o ) : iIndex( o.iIndex ), iVersion( o.iVersion ) {};

		// Comparison operator
		bool operator == ( const SPoolIndex& ia )
		{
			return( ia.iIndex == iIndex && ia.iVersion == iVersion );
		}

		// Not-Comparison operator
		bool operator != ( const SPoolIndex& ia )
		{
			return( ia.iIndex != iIndex || ia.iVersion != iVersion );
		}
	};

	typedef struct S_API SPoolIndex SP_ID;
}