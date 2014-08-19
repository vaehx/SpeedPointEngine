// ******************************************************************************************

// This file is part of the SpeedPoint Engine

// ******************************************************************************************

#pragma once
#include <SPrerequisites.h>
#include "SEventCallbackTable.h"

namespace SpeedPoint
{		

	// ******************************************************************************************

	// SpeedPoint Observed Pipeline Base class
	class S_API SObservedObject
	{
	private:
		SEventCallbackTable m_EventCallbackTable;

	protected:
		SResult InitEventCallbackTable(SpeedPointEngine* pEngine);

	public:
		// Default constructor
		SObservedObject();

		// constructor with destruction handler pointer
		SObservedObject(SEventCallback pDestructionCallbackFunction);

		// Default destructor
		~SObservedObject();		

		// Set the destruction handler manually		
		void SetDestructionCallback(SEventCallback pDestructionCallbackFunc);

		// Function to call an event by given index and parameters
		SResult CallEvent(unsigned int iIndex, SEventParameters* pParams);

		// Function to call an event by given index and parameter array
		SResult CallEvent(unsigned int iIndex, SEventParameter* pParamArray, unsigned int nParamCount);
	};
}