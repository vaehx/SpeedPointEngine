// ******************************************************************************************

// SpeedPoint Logging Stream

// ******************************************************************************************

#pragma once
#include <SAPI.h>
#include <SResult.h>
#include <SString.h>

#define SP_MAX_LOGSTREAM_HANDLERS 5

namespace SpeedPoint
{
	typedef void (*SLogHandler)( SResult, SString );

	// SpeedPoint Log Stream
	class S_API SLogStream
	{
	private:
		SLogHandler* pLogHandlers;

	public:		
		// Default constructor
		SLogStream()
			: pLogHandlers(0)
		{
		}

		// Initialize the stream
		void Initialize( void );

		// Add a new char buffer to the stream and notify listeners
		// will return the result given as a parameter
		SResult Report( SResult res, SString str );		

		// Register a listener function
		void RegisterHandler( SLogHandler pHandler );

		// Clear log listener functions
		void Clear( void );
	};
}