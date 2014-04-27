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
		,S_PARAMTYPE_PTR
	};

	// ******************************************************************************************

	// Structure of a single SpeedPoint Event Handler Parameter
	struct S_API SEventParameter
	{
		char* m_pcIndex;
		void* m_pValue;
		SEventParameterType m_tType;

		// Default constructor
		SEventParameter();

		// Constructor with given values
		SEventParameter(char* pcIndex, SEventParameterType tType, void* pValue);

		// Constructor with direct value (int)
		SEventParameter(char* pcIndex, const int& iValue);

		// Constructor with direct value (float)
		SEventParameter(char* pcIndex, const float& fValue);

		// Constructor with direct value (char*)
		SEventParameter(char* pcIndex, char* cValue);

		// Constructor with direct value (bool)
		SEventParameter(char* pcIndex, const bool& bValue);

		// Constructor with direct value (void*-Pointer)
		SEventParameter(char* pcIndex, void* pPointerValue);		

		// Default destructor
		~SEventParameter();

		// Assign the index
		bool AssignIndex(char* pcIndex);
	};

}