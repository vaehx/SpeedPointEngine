// SpeedPoint Logging Stream

#include "SLogStream.h"
#include <cstring>
#include <iostream>

namespace SpeedPoint
{

	// ******************************************************************************************

	S_API void SLogStream::Initialize( void )
	{
		Clear();

		pLogHandlers = new PLogHandler[ SP_MAX_LOGSTREAM_HANDLERS ];
		
		memset( pLogHandlers, 0, sizeof( PLogHandler ) * SP_MAX_LOGSTREAM_HANDLERS );
	}

	// ******************************************************************************************

	S_API SResult SLogStream::Report( SResult res, const string& msg )
	{
		// Notify all listeners
		bool bFoundLogHandler = false;
		if( pLogHandlers )
		{
			for( int i = 0; i < SP_MAX_LOGSTREAM_HANDLERS; i++ )
			{
				if( pLogHandlers[i] == NULL ) continue;

				// Call the log handler listener function
				(*pLogHandlers[i])( res, msg );
				bFoundLogHandler = true;
			}
		}

		if (!bFoundLogHandler)
		{
			// simply throw it into standard output stream
			std::cout << msg << std::endl;
		}

		return res;
	}

	// ******************************************************************************************

	S_API void SLogStream::RegisterHandler( PLogHandler pHandler )
	{
		if( !pLogHandlers ) Initialize();

		if( pLogHandlers )
		{
			for( int i = 0; i < SP_MAX_LOGSTREAM_HANDLERS; i++ )
			{
				if( pLogHandlers[i] == NULL )
				{
					pLogHandlers[i] = pHandler;
					break;
				}
			}
		}
	}

	// ******************************************************************************************

	S_API void SLogStream::Clear( void )
	{
		if( pLogHandlers )
		{			
			delete[] pLogHandlers;
			pLogHandlers = NULL;
		}
	}


}