// ********************************************************************************************

// This file is part of the SpeedPoint Engine

// ********************************************************************************************

#include <Observer\SEventHandler.h>

namespace SpeedPoint
{
	// ******************************************************************************************

	SEventHandler::SEventHandler()
		: m_pFunction(0)
	{		
	}

	// ******************************************************************************************

	SEventHandler::SEventHandler(SEventCallback pFunction)
		: m_pFunction(pFunction)
	{
	}
	
	
	// ******************************************************************************************

	SEventHandler::~SEventHandler()		
	{
		m_pFunction(0);
	}

	// ******************************************************************************************

	SResult SEventHandler::Initialize(SEventCallback pFunction)
	{
		if (pFunction == 0)
		{
//~~~~~~
// TODO: Implement logging
			// LogE("Invalid callback function pointer given on initialization of Event Handler!");
//~~~~~~
			return S_ABORTED; // invalid callback function pointer
		}

		m_pFunction = pFunction;
		return S_SUCCESS;
	}

	// ******************************************************************************************

	bool SEventHandler::IsInitialized()
	{
		return m_pFunction;
	}

	// ******************************************************************************************

	SResult SEventHandler::Call(SEventParameters* pParameters)
	{
		if (IsInitialized())
		{
			return m_pFunction(pParameters);
		}
		else
		{
// ~~~~~~~~~~~
// TODO: Implement logging here
			// LogE( "Tried to call unset event handler xyz!" );
// ~~~~~~~~~~~
			return S_ABORTED;
		}
	}
}