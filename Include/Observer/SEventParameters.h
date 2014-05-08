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
	class S_API SEventParameters
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

		// Check if the buffer is initialized
		bool IsInitialized()
		{
			return m_pParameters && m_nMaxParameters;
		}

		// Add a new parameter to the buffer with given attributes
		// The bytes pointed to by pData and the index are copied to the buffer then!
		// ZSTRs are excepted to be given as Pointer to a Pointer of the char buffer (char**)
		// PTRs are expected to be given as Pointer to the Pointer. Then the adress is copied. (void**)
		SResult Add(SEventParameterIndex index, SEventParameterType tType, void* pData);	

		// Add a new parameter with the ptr to the sender of the event
		// This function does NOT require a ptr to the ptr of the sender!
		// Instead you can just put 'this' as argument
		SResult AddSenderParam(void* pSender)
		{
			if (!pSender) 
				return S_INVALIDPARAM;
			else
				return Add(ePARAM_SENDER, S_PARAMTYPE_PTR, &pSender);
		}

		// Add a bunch of parameters to the buffer given by an array of SEventParameter
		// The bytes pointed to by pData and the index are copied to the buffer then!
		// ZSTRs are excepted to be given as Pointer to a Pointer of the char buffer (char**)
		// PTRs are expected to be given as Pointer to the Pointer. Then the adress is copied. (void**)
		SResult AddArray(SEventParameter* pParameterArray, unsigned int nCount);

		// Get a parameter by given index string
		SEventParameter* Get(SEventParameterIndex index);

		// Clear the buffer
		SResult Clear();
	};

}