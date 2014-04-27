// ********************************************************************************************

// This file is part of the SpeedPoint Engine

// ********************************************************************************************

#include <Observer\SEventCallbackTable.h>

//~~~~~~~~~
// TODO: Try to eliminate this include and use interface instead!
#include <SSpeedPointEngine.h>
//~~~~~~~~~

namespace SpeedPoint
{
	// ********************************************************************************************

	S_API bool SEventCallbackTable::IsInitialized()
	{		
		return (m_pFirstEntry != 0 && m_pEngine != 0);
	}

	// ********************************************************************************************

	S_API SResult SEventCallbackTable::Initialize(SpeedPointEngine* pEngine)
	{
		if (IsInitialized())
		{					
			m_pEngine->LogW("Tried to initialize already initialized EventCallbackTable! Clearing it now...");

			if (Failure(Clear()))
			{				
				m_pEngine->LogE("Clearing of initialized EventCallbackTable failed!");
				return S_ABORTED;
			}
		}

		m_pFirstEntry = new SEventCallbackTableEntry();
		m_pLastEntry = m_pFirstEntry;
		return S_SUCCESS;
	}

	// ********************************************************************************************

	S_API SResult SEventCallbackTable::Add(unsigned int iIndex, const SEventHandler& handler)
	{		
		if (!IsInitialized())		
			// cannot throw error here due to the engine ptr not being initialized
			return S_NOTINIT;		

		// Check against NPE
		if (m_pLastEntry == 0)	
			return m_pEngine->LogE("Could not add Callback Table entry: Ptr to last entry points into nirvana (wrongly initialized).");		

		// Add the table entry		
		SEventCallbackTableEntry* pEntry = new SEventCallbackTableEntry();
		pEntry->m_Handler = handler;
		pEntry->m_iIndex = iIndex;
		pEntry->m_pNextEntry = 0;
		pEntry->m_pPrevEntry = m_pLastEntry;
		m_pLastEntry->m_pNextEntry = pEntry;
		m_pLastEntry = pEntry;

		return S_SUCCESS;
	}

	// ********************************************************************************************

	S_API SResult SEventCallbackTable::Add(unsigned int iIndex, SEventCallback pCB)
	{
		// we do not have to go further if the table is not initialized ;)
		if (!IsInitialized())
			return S_NOTINIT;

		SEventHandler handler;
		handler.Initialize(pCB);

		return Add(iIndex, handler);
	}

	// ********************************************************************************************
	
	S_API SResult SEventCallbackTable::Call(unsigned int iIndex, SEventParameters* parameters)
	{
		// We do not have to go further if the table isnt initialized
		if (!IsInitialized())
			return S_NOTINIT;

		if (m_pFirstEntry == 0 || m_pLastEntry == 0)
			return m_pEngine->LogE("Could not call event of EventTable: Table Stack damaged!");

		// now seek for all table entries with given index
		SResult res = S_SUCCESS;
		SEventCallbackTableEntry* pCurrentTableEntry = m_pFirstEntry;		
		while (true)
		{
			if (pCurrentTableEntry->m_iIndex == iIndex && pCurrentTableEntry->m_Handler.IsInitialized())
			{
				if (Failure(pCurrentTableEntry->m_Handler.Call(parameters)))
					res = S_ERROR;
			}

			if (pCurrentTableEntry->m_pNextEntry != 0 && pCurrentTableEntry != m_pLastEntry)
				pCurrentTableEntry = pCurrentTableEntry->m_pNextEntry;
			else
				break;
		}

		return res;
	}

	// ********************************************************************************************

	S_API SResult SEventCallbackTable::Clear()
	{
		if (IsInitialized())
		{
			SEventCallbackTableEntry* pCurrentEntry = m_pFirstEntry;
			while (true)
			{
				// warning: this might produce an endless loop if m_pNextEntry is not inited to 0 properly
				if (pCurrentEntry->m_pNextEntry != 0)
				{
					pCurrentEntry = pCurrentEntry->m_pNextEntry;
				}
				else
				{
					SEventCallbackTableEntry* pCurrentParentEntry = pCurrentEntry->m_pPrevEntry;
					delete pCurrentEntry;

					if (pCurrentParentEntry == 0)
						break; // reached beginning of the buffer, which means that everything is cleared well

					pCurrentEntry = pCurrentParentEntry;
					pCurrentEntry->m_pNextEntry = 0;
				}
			}			

			return S_SUCCESS;
		}

		return S_NOTINIT;
	}

	// ********************************************************************************************

	S_API SResult SEventCallbackTable::Clear(unsigned int iIndex)
	{
		if (IsInitialized())
		{
			SEventCallbackTableEntry* pCurrentEntry = m_pLastEntry;
			while (true)
			{
				if (pCurrentEntry == 0)
					break; // reached the end of the buffer -> all entries with given index are cleared out properly

				SEventCallbackTableEntry* pTmpEntry = pCurrentEntry;
				pCurrentEntry = pTmpEntry->m_pPrevEntry;

				if (pTmpEntry->m_iIndex == iIndex)
				{					
					delete pTmpEntry;
					pTmpEntry = 0;
					pCurrentEntry->m_pNextEntry = 0;
				}				
			}

			return S_SUCCESS;
		}

		return S_NOTINIT;
	}
}