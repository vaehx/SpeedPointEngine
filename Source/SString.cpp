// SpeedPoint String

#include <SString.h>
#include <Windows.h>

namespace SpeedPoint
{
	// **********************************************************************************

	S_API SString::SString()
	{
		cString = 0;
	}

	// **********************************************************************************

	S_API SString::SString( const SString& str )
	{
		if( str.cString && strlen( str.cString ) > 0 )
		{
			cString = new char[strlen( str.cString )];
			strcpy_s( cString, strlen( str.cString ), str.cString );
		}
		else
		{
			cString = new char[1];
			cString[0] = '\0';
		}
	}

	// **********************************************************************************

	S_API SString::SString( char* src )
	{
		if( src && strlen( src ) > 0 )
		{
			cString = new char[strlen( src )];
			strcpy_s( cString, strlen( src ), src );
		}
		else
		{
			cString = new char[1];
			cString[0] = '\0';
		}
	}

	// **********************************************************************************
		
	S_API int SString::GetLength( void )
	{
		if( cString )
			return (int)strlen( cString );

		return 0;
	}

	// **********************************************************************************

	S_API void SString::Clear( void )
	{
		if( cString )
			delete[] cString;
		
		cString = 0;
	}

	// **********************************************************************************

	S_API SString::~SString()
	{
		if( cString )
			delete[] cString;

		cString = 0;
	}
	
}