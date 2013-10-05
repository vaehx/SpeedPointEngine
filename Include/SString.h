// ********************************************************************************************

//	SpeedPoint String

// ********************************************************************************************

#pragma once
#include "SAPI.h"

namespace SpeedPoint
{
	// SpeedPoint String
	class S_API SString
	{
	public:
		char* cString;

		// Default constructor
		SString();

		// Copy constructor
		SString( const SString& str );

		// Constructor for char* copy
		SString( char* src );

		// Destructor
		~SString();

		// Get the length
		int GetLength( void );	

		// Clear
		void Clear( void );		
	};

	inline bool operator == ( const SString& sa, const SString& sb )
	{
		if( sa.cString == sb.cString ) return true;
		
		if( sa.cString == 0 || sb.cString == 0 ) return false;

		// loop through all characters	
		int i = 0;
		while( true )
		{
			if( sa.cString[i] != sb.cString[i] ) return false;
			if( sa.cString[i] == 0 ) return true;

			++i;
		}

		return true;
	}
}