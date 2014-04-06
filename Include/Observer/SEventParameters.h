// ******************************************************************************************

// This file is part of the SpeedPoint Engine

// ******************************************************************************************

#pragma once
#include <SPrerequisites.h>
#include "SEventParameter.h"

namespace SpeedPoint
{

	// ******************************************************************************************

	// SpeedPoint Event Handler Parameters class
	class SEventParameters
	{
	private:
		SEventParameter*	m_pParameters;		// Parameters buffer
		unsigned int		m_nMaxParameters;	// the maximum count of the parameter-slots allocated in the buffer		

	public:
		SEventParameters()
			: m_pParameters(0),
			m_nMaxParameters(0)
		{
		}

		~SEventParameters()
		{
			// Make sure the buffer is cleared out
			Clear();			
		}

		// Initialize the buffer with given parameter count.
		// If the buffer is already initialized, the data will be overwritten!
		SResult Initialize(const unsigned int nParameterCount);

		// Add a new parameter to the buffer with given attributes
		// The bytes pointed to by pData and the index are copied to the buffer then!
		SResult Add(char* pcIndex, SEventParameterType tType, void* pData);		

		// Get a parameter by given index string
		SEventParameter* Get(char* pcIndex);

		// Clear the buffer
		SResult Clear();
	};

}