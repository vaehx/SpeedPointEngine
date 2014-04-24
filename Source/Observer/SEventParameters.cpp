// ********************************************************************************************

// This file is part of the SpeedPoint Engine

// ********************************************************************************************

#include <Observer\SEventParameters.h>

namespace SpeedPoint
{
	// ********************************************************************************************		

	SResult SEventParameters::Initialize(const unsigned int nParameterCount)
	{
		// Check arguments
		if (nParameterCount == 0)
		{
// ~~~~~~~~
// TODO: Implement logging
			// LogE("Tried to initialize EventParameters buffer with size 0. Call Clear() if you want to empty it.");
// ~~~~~~~~
			return S_ABORTED;
		}

		// Make sure the buffers is cleared before 
		Clear();		

		// Now initialize the new buffer
		m_pParameters = new SEventParameter[nParameterCount];
		m_nMaxParameters = nParameterCount;

		if (m_pParameters != 0) return S_SUCCESS; else return S_ERROR;
	}

	// ********************************************************************************************

	SResult SEventParameters::Add(char* pcIndex, SEventParameterType tType, void* pData)
	{
		unsigned int nIndexLength;

		if (m_pParameters == 0 || m_nMaxParameters == 0)
		{
//~~~~~~~~~
// TODO: Implement logging
			// LogW("Tried to add parameter to not-yet-initialized event parameters buffer. Initializing it now...");
//~~~~~~~~~
			if (Failure(Initialize(0))) return S_ABORTED;
		}

		if (pcIndex == 0 || (nIndexLength = strlen(pcIndex)) == 0)
		{
//~~~~~~~~
// TODO: Implement logging
			// LogE("Invalid index argument given when trying to add event parameter!");
//~~~~~~~~
			return S_ABORTED;
		}

		if (pData == 0)
		{
//~~~~~~~~
// TODO: Implement logging
			// LogE("Invalid index argument given when trying to add event parameter!");
//~~~~~~~~
			return S_ABORTED;
		}

		// Try to find an empty slot
		for (unsigned int i = 0; i < m_nMaxParameters; i++)
		{
			if (m_pParameters[i].m_pcIndex == 0)
			{
				// Found a free parameter slot, now initialize it
				unsigned int nBytes = 0;
				switch (tType)
				{
				case S_PARAMTYPE_INT:
					nBytes = sizeof(int);
					m_pParameters[i].m_pValue = new int();
					break;
				case S_PARAMTYPE_BOOL:
					nBytes = sizeof(bool);
					m_pParameters[i].m_pValue = new bool();
					break;
				case S_PARAMTYPE_FLOAT:
					nBytes = sizeof(float);
					m_pParameters[i].m_pValue = new float();
					break;
				case S_PARAMTYPE_PTR:
					nBytes = sizeof(void*);
					m_pParameters[i].m_pValue = new void*();
					break;
				case S_PARAMTYPE_ZTSTR:
					nBytes = strlen(*((char**)pData));
					m_pParameters[i].m_pValue = new char[nBytes + 1]; // +1 for terminating 0
					break;
				default:
					return S_INVALIDPARAM; // Variable type not known
				}

				if (tType == S_PARAMTYPE_ZTSTR)
					strcpy_s((char*)m_pParameters[i].m_pValue, nBytes, *(char**)pData);
				else
					memcpy(m_pParameters[i].m_pValue, pData, nBytes);

				m_pParameters[i].m_tType = tType;
				m_pParameters[i].m_pcIndex = new char[nIndexLength];
				strcpy_s(m_pParameters[i].m_pcIndex, nIndexLength, pcIndex);
				return S_SUCCESS; // Added event parameter successfully!
			}
		}

//~~~~~~
// TODO: Implement logging
		// return LogE("Could not add event parameter: no free slot found!");
//~~~~~~~
		return S_ERROR;
	}

	// ********************************************************************************************

	SEventParameter* SEventParameters::Get(char* pcIndex)
	{
		// check if the buffer is initialized at all
		if (m_pParameters == 0 || m_nMaxParameters == 0)
		{
//~~~~~~~
// TODO: Implement logging
			// LogE("Tried to get Event parameter of uninitialized buffer!");
//~~~~~~~
			return 0;
		}

		// check if given index parameter is valid
		if (pcIndex == 0 || strlen(pcIndex) == 0)
		{
//~~~~~~~
// TODO: Implement logging
			// LogE("Invalid index given when trying to get event parameter");
//~~~~~~~
			return 0;
		}

		// now find the parameter
		for (unsigned int i = 0; i < m_nMaxParameters; i++)
		{
			if (strcmp(m_pParameters[i].m_pcIndex, pcIndex) == 0)
				return &m_pParameters[i];
		}

		// not found
//~~~~~~~
// TODO: Implement logging
		// LogE("Could not find event parameter with given index");
//~~~~~~~
		return 0;
	}

	// ********************************************************************************************

	SResult SEventParameters::Clear()
	{
		if (m_pParameters != 0)
			delete[] m_pParameters;

		m_pParameters = 0;
		m_nMaxParameters = 0;
		return S_SUCCESS;
	}
}