// ******************************************************************************************

// This file is part of the SpeedPoint Engine

// ******************************************************************************************

#pragma once
#include "SEventCallbackTable.h"

namespace SpeedPoint
{

	// ******************************************************************************************

	// Basic Destruction Event used for every Pipeline
#define S_E_DESTRUCT 99999

	// ******************************************************************************************

	// SpeedPoint Observed Pipeline Base class
	class SObservedObject
	{
	private:
		SEventCallbackTable m_EventCallbackTable;

	public:
		// Default constructor
		SObservedObject()
		{
		}

		// constructor with destruction handler pointer
		SObservedObject(SEventCallback pDestructionCallbackFunction);

		// Default destructor
		~SObservedObject();

		// Set the destruction handler manually		
		void SetDestructionCallback(SEventCallback pDestructionCallbackFunc);

		// Function to call an event by given index and parameters
		SResult CallEvent(unsigned int iIndex, SEventParameters* pParams);
	};
}