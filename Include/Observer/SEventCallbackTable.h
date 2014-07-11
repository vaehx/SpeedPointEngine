// ******************************************************************************************

// This file is part of the SpeedPoint Engine

// ******************************************************************************************

#pragma once
#include <SPrerequisites.h>
#include "SEventCallbackTableEntry.h"

SP_NMSPACE_BEG

class S_API SpeedPointEngine;


// ******************************************************************************************	

// SpeedPoint Event Callback Table
class S_API SEventCallbackTable
{
private:
	SpeedPointEngine*		m_pEngine;
	SEventCallbackTableEntry*	m_pFirstEntry;
	SEventCallbackTableEntry*	m_pLastEntry;	// The last entry used to avoid searching for the end in Add()

public:
	SEventCallbackTable()
		: m_pFirstEntry(0)
	{
	}

	~SEventCallbackTable()
	{
	}

	// will create first table item
	SResult Initialize(SpeedPointEngine* pEngine);

	// Check if the callback table is initialized
	bool IsInitialized();

	// Add an entry to this table with given event handler
	SResult Add(unsigned int iIndex, const SEventHandler& handler);

	// Add an entry to this table by given handler function
	SResult Add(unsigned int iIndex, SEventCallback pCB);

	// Call all entries of this table with given index
	SResult Call(unsigned int iIndex, SEventParameters* parameters);

	// Clear out all table entries
	SResult Clear();

	// Clear out all table entries with given index
	SResult Clear(unsigned int iIndex);
};


SP_NMSPACE_END