// ******************************************************************************************

// This file is part of the SpeedPoint Engine

// ******************************************************************************************

#pragma once
#include <SPrerequisites.h>

namespace SpeedPoint
{

	// ******************************************************************************************

	// Type of SpeedPoint Event Handler Parameter Value
	enum S_API SEventParameterType
	{
		S_PARAMTYPE_INT
		, S_PARAMTYPE_FLOAT
		, S_PARAMTYPE_ZTSTR // = Zero-Terminated string
		, S_PARAMTYPE_BOOL
		, S_PARAMTYPE_PTR
	};

	// ******************************************************************************************

	// Index of the Parameter
	// At first, we wanted to offer most flexibility, but quickly it turned out to be the crappiest
	// idea we ever had! - Don't do this, use this enum instead!
	enum S_API SEventParameterIndex
	{
		ePARAM_NONE,
		ePARAM_SENDER,
		ePARAM_LIGHTSOURCES,
		ePARAM_DRAW_DESC,
		ePARAM_CUSTOM_DATA
	};

	// ******************************************************************************************

	// Structure of a single SpeedPoint Event Handler Parameter
	struct S_API SEventParameter
	{
		SEventParameterIndex	m_Index;
		void*			m_pValue;
		SEventParameterType	m_tType;

		// Default constructor
		SEventParameter();

		// Constructor with given values
		SEventParameter(SEventParameterIndex index, SEventParameterType tType, void* pValue);

		// Constructor with direct value (int)
		SEventParameter(SEventParameterIndex index, const int& iValue);

		// Constructor with direct value (float)
		SEventParameter(SEventParameterIndex index, const float& fValue);

		// Constructor with direct value (char*)
		SEventParameter(SEventParameterIndex index, char* cValue);

		// Constructor with direct value (bool)
		SEventParameter(SEventParameterIndex index, const bool& bValue);

		// Constructor with direct value (void*-Pointer)
		SEventParameter(SEventParameterIndex index, void* pPointerValue);		

		// Default destructor
		~SEventParameter();

		// Assign the index
		bool AssignIndex(SEventParameterIndex index);
	};

}