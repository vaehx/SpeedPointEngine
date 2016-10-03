// ******************************************************************************************

// This file is part of the SpeedPoint Engine

// ******************************************************************************************

#include <Observer\SEventParameter.h>

namespace SpeedPoint
{
	// ******************************************************************************************
	
	S_API SEventParameter::SEventParameter()
		: m_Index(ePARAM_NONE),
		m_tType(S_PARAMTYPE_PTR),
		m_pValue(0)
	{
	}

	// ******************************************************************************************

	S_API bool SEventParameter::AssignIndex(SEventParameterIndex index)
	{
		m_Index = index;
		return true;
	}

	// ******************************************************************************************
	
	S_API SEventParameter::SEventParameter(SEventParameterIndex index, SEventParameterType tType, void* pValue)
	{		
		switch (m_tType = tType)
		{
		case S_PARAMTYPE_INT:
			SEventParameter(index, *(int*)pValue);
			break;

		case S_PARAMTYPE_BOOL:
			SEventParameter(index, *(bool*)pValue);
			break;

		case S_PARAMTYPE_FLOAT:
			SEventParameter(index, *(float*)pValue);
			break;

		case S_PARAMTYPE_PTR:
			SEventParameter(index, *(void**)pValue);
			break;

		case S_PARAMTYPE_ZTSTR:
			SEventParameter(index, *(char**)pValue);			
			break;

		default:
			return;
		}				
	}

	// ******************************************************************************************

	S_API SEventParameter::SEventParameter(SEventParameterIndex index, const int& iValue)
	{
		if (!AssignIndex(index)) return;
		
		m_pValue = new int(iValue);
	}

	// ******************************************************************************************

	S_API SEventParameter::SEventParameter(SEventParameterIndex index, const float& fValue)
	{
		if (!AssignIndex(index)) return;

		m_pValue = new float(fValue);
	}

	// ******************************************************************************************

	S_API SEventParameter::SEventParameter(SEventParameterIndex index, char* cValue)
	{
		if (!AssignIndex(index)) return;

		if (cValue)
		{
			unsigned int nStringLen = strlen(cValue);
			m_pValue = new char[nStringLen];
			if (nStringLen > 0)
				strcpy_s((char*)m_pValue, nStringLen, cValue);
		}
		else
		{
			m_pValue = 0;
		}
	}

	// ******************************************************************************************

	S_API SEventParameter::SEventParameter(SEventParameterIndex index, const bool& bValue)
	{
		if (!AssignIndex(index)) return;

		m_pValue = new bool(bValue);
	}

	// ******************************************************************************************

	S_API SEventParameter::SEventParameter(SEventParameterIndex index, void* pPointerValue)
	{
		if (!AssignIndex(index)) return;

		m_pValue = new void*(pPointerValue);
	}

	// ******************************************************************************************

	S_API SEventParameter::~SEventParameter()
	{
		if (m_pValue) delete m_pValue;
		m_pValue = 0;

		m_Index = ePARAM_NONE;
	}
}