// ******************************************************************************************

// This file is part of the SpeedPoint Engine

// ******************************************************************************************

#pragma once
#include <SPrerequisites.h>
#include "SEventHandler.h"

namespace SpeedPoint
{

	// ******************************************************************************************

	// Entry of a SpeedPoint Callback Table
	struct S_API SEventCallbackTableEntry
	{
		unsigned int 			m_iIndex;
		SEventHandler			m_Handler;
		SEventCallbackTableEntry* 	m_pPrevEntry;
		SEventCallbackTableEntry*	m_pNextEntry;

		SEventCallbackTableEntry()
			: m_iIndex(0),
			m_pPrevEntry(0),
			m_pNextEntry(0)
		{
		}
	};

}