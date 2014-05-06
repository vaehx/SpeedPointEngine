// ********************************************************************************************

// This file is part of the SpeedPoint Engine

// ********************************************************************************************

#include <Observer\SEventParameters.h>

namespace SpeedPoint
{
	// ********************************************************************************************		

	S_API SResult SEventParameters::Initialize(const unsigned int nParameterCount)
	{
		// Check arguments
		SP_ASSERTR(nParameterCount, S_ABORTED);	

		// Make sure the buffers is cleared before 
		Clear();

		// Now initialize the new buffer
		m_pParameters = new SEventParameter[nParameterCount];
		m_nMaxParameters = nParameterCount;

		if (m_pParameters != 0)
			return S_SUCCESS;
		else
			return S_ERROR;
	}

	// ********************************************************************************************

	S_API SResult SEventParameters::Add(SEventParameterIndex index, SEventParameterType tType, void* pData)
	{
		unsigned int nIndexLength;

		if (m_pParameters == 0 || m_nMaxParameters == 0)
		{
			if (Failure(Initialize(1)))
				return S_ABORTED;
		}

		SP_ASSERTRD(index != ePARAM_NONE, S_ABORTED, "Invalid parameter index %d", index);
		SP_ASSERTR(pData, S_ABORTED, "Given data ptr is zero!");

		// Try to find an empty slot
		for (unsigned int i = 0; i < m_nMaxParameters; i++)
		{
			if (m_pParameters[i].m_Index == ePARAM_NONE)
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
				m_pParameters[i].m_Index = index;				
				return S_SUCCESS; // Added event parameter successfully!
			}
		}

		SP_ASSERTD(false, "Could not add event parameter: no free slot found!");

		return S_ERROR;
	}

	// ********************************************************************************************

	S_API SResult SEventParameters::AddArray(SEventParameter* pParameterArray, unsigned int nCount)
	{
		if (nCount == 0) return S_SUCCESS;

		if (m_pParameters == 0)
		{
			Initialize(nCount);
		}

		// check if there is anough space left for the parameters
		bool bEnoughSlotsLeft = (m_nMaxParameters >= nCount);
		unsigned int nFreeSlots = 0;		
		if (bEnoughSlotsLeft)
		{
			// if nMaxParameters seems to be enough, check if there are enough empty slots			
			for (unsigned int i = 0; i < m_nMaxParameters; ++i)
			{
				if (m_pParameters[i].m_Index == ePARAM_NONE)
				{
					// to be efficient, already copy first parameters of the array
					SEventParameter* pParam = &pParameterArray[nFreeSlots];
					if (pParam->m_tType == S_PARAMTYPE_PTR || pParam->m_tType == S_PARAMTYPE_ZTSTR)
						m_pParameters[i] = SEventParameter(pParam->m_Index, pParam->m_tType, &pParam->m_pValue);
					else
						m_pParameters[i] = SEventParameter(pParam->m_Index, pParam->m_tType, pParam->m_pValue);

					++nFreeSlots;
				}
			}

			if (nFreeSlots < nCount)
				bEnoughSlotsLeft = false;
		}

		if (!bEnoughSlotsLeft)
		{
			// add missing count of free slots
			SEventParameter* pTempParams = new SEventParameter[nCount - nFreeSlots];
			memcpy(pTempParams, m_pParameters, m_nMaxParameters);
			delete[] m_pParameters;
			m_pParameters = pTempParams;

			bEnoughSlotsLeft = true;
		}				

		// Copy remaining parameters
		for (unsigned int i = nFreeSlots; i < nCount; ++i)
		{
			SEventParameter* pParam = &pParameterArray[i];
			if (pParam->m_tType == S_PARAMTYPE_PTR || pParam->m_tType == S_PARAMTYPE_ZTSTR)
				m_pParameters[i] = SEventParameter(pParam->m_Index, pParam->m_tType, &pParam->m_pValue);
			else
				m_pParameters[i] = SEventParameter(pParam->m_Index, pParam->m_tType, pParam->m_pValue);
		}

		return S_SUCCESS;
	}

	// ********************************************************************************************

	S_API SEventParameter* SEventParameters::Get(SEventParameterIndex index)
	{
		SP_ASSERTRD(m_pParameters && m_nMaxParameters, 0, "Tried to get Event parameter of uninitialized buffer!");	
		
		SP_ASSERTRD(index != ePARAM_NONE, 0, "Invalid index %d", index);

		// now find the parameter
		for (unsigned int i = 0; i < m_nMaxParameters; i++)
		{
			if (index == m_pParameters[i].m_Index)
				return &m_pParameters[i];
		}

		// not found
		SP_ASSERTD(false, "Parameter with index %d not found!", index);
		return 0;
	}

	// ********************************************************************************************

	S_API SResult SEventParameters::Clear()
	{
		if (m_pParameters != 0)
			delete[] m_pParameters;

		m_pParameters = 0;
		m_nMaxParameters = 0;
		return S_SUCCESS;
	}
}