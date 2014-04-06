// ******************************************************************************************

// This file is part of the SpeedPoint Engine

// ******************************************************************************************

#pragma once
#include "SEventHandler.h"

namespace SpeedPoint
{

	// ******************************************************************************************

	// SpeedPoint Observed Pipeline Base class
	class SObservedObject
	{
	private:
		SEventHandler m_DestructionHandler;

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
	};
}