// ******************************************************************************************

// This file is part of the SpeedPoint Engine

// ******************************************************************************************

#pragma once

namespace SpeedPoint
{

	// ******************************************************************************************

	// Type of SpeedPoint Event Handler Parameter Value
	enum SEventParameterType
	{
		S_PARAMTYPE_INT
		, S_PARAMTYPE_FLOAT
		, S_PARAMTYPE_ZTSTR // = Zero-Terminated string
		, S_PARAMTYPE_BOOL
		,S_PARAMTYPE_PTR
	};

	// ******************************************************************************************

	// Structure of a single SpeedPoint Event Handler Parameter
	struct SEventParameter
	{
		char* m_pcIndex;
		void* m_pValue;
		SEventParameterType m_tType;

		SEventParameter()
			: m_pcIndex(0),
			m_pValue(0),
			m_tType(S_PARAMTYPE_INT)
		{
		}

		~SEventParameter()
		{
			if (m_pValue) delete m_pValue;
			if (m_pcIndex) delete m_pcIndex;
			m_pValue = 0;
			m_pcIndex = 0;
		}
	};

}