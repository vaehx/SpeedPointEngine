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
		UINT nSrcSize = strlen( str.cString );
		if( str.cString && nSrcSize > 0 )
		{
			cString = new char[nSrcSize + 1];
			strcpy_s( cString, nSrcSize + 1, str.cString );
			cString[nSrcSize] = 0;
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
		UINT nSrcSize = strlen( src );
		if( src && nSrcSize > 0 )
		{
			cString = new char[nSrcSize + 1];
			ZeroMemory( cString, nSrcSize + 1 );
			strcpy_s( cString, nSrcSize + 1, src );
			cString[nSrcSize] = 0;
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