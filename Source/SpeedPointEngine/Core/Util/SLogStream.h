// ******************************************************************************************

// SpeedPoint Logging Stream

//	This file is part of the SpeedPoint Game Engine

//	(c) 2011 - 2014 Pascal Rosenkranz aka iSmokiieZz
//	All rights reserved.

// ******************************************************************************************

#pragma once
#include <Abstract\SPrerequisites.h>
//#include "SResult.h"
//#include "SString.h"

#define SP_MAX_LOGSTREAM_HANDLERS 5

namespace SpeedPoint
{
	// ******************************************************************************************

	// Use this to point to a function that shall be called when log entry was added to the stream

	typedef S_API void(*PLogHandler)(SResult, const string&);

	// ******************************************************************************************	


	// SpeedPoint Log Stream	
	class S_API SLogStream
	{
	private:
		PLogHandler* pLogHandlers;

	public:		
		// Default constructor
		SLogStream()
			: pLogHandlers(0)
		{
		}

		// Initialize the stream
		void Initialize(void);

		// Add a new char buffer to the stream and notify listeners
		// will return the result given as a parameter
		SResult Report(SResult res, const string& str);

		// Register a listener function
		void RegisterHandler(PLogHandler pHandler);

		// Clear log listener functions
		void Clear(void);
	};
}