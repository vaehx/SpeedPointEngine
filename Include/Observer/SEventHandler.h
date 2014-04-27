// ******************************************************************************************

// This file is part of the SpeedPoint Engine

// ******************************************************************************************

#pragma once
#include <SPrerequisites.h>
#include "SEventParameters.h"

namespace SpeedPoint
{
	// ******************************************************************************************

	// SpeedPoint Event Handler callback function
	typedef S_API SResult(*SEventCallback)(SEventParameters*);

	// SpeedPoint Event Handler class (part of the observer pattern)
	class S_API SEventHandler
	{
	private:
		SEventCallback m_pFunction;

	public:
		// Default constructor
		SEventHandler();

		// Constructor with pointer to the callback function
		SEventHandler(SEventCallback pCallbackFunction);

		// Default destructor
		~SEventHandler();

		// Sets the callback function
		SResult Initialize(SEventCallback pCallbackFunction);

		// Check if the event handler is initialized
		bool IsInitialized();

		// Call the callback function
		SResult Call(SEventParameters* pParams);
	};
}