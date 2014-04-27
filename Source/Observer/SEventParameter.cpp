// ******************************************************************************************

// This file is part of the SpeedPoint Engine

// ******************************************************************************************

#include <Observer\SEventParameter.h>

namespace SpeedPoint
{
	// ******************************************************************************************
	
	S_API SEventParameter::SEventParameter()
		: m_pcIndex(0),
		m_tType(S_PARAMTYPE_PTR),
		m_pValue(0)
	{
	}

	// ******************************************************************************************

	S_API bool SEventParameter::AssignIndex(char* pcIndex)
	{
		if (pcIndex)
		{
			unsigned int nStringLen = strlen(pcIndex);
			m_pcIndex = new char[nStringLen];
			if (nStringLen > 0)
			{
				strcpy_s(m_pcIndex, nStringLen, pcIndex);
			}
			else
			{
				m_pcIndex = 0;
				return false;
			}

			return true;
		}
		
		return false;		
	}

	// ******************************************************************************************
	
	S_API SEventParameter::SEventParameter(char* pcIndex, SEventParameterType tType, void* pValue)
	{		
		switch (m_tType = tType)
		{
		case S_PARAMTYPE_INT:
			SEventParameter(pcIndex, *(int*)pValue);
			break;

		case S_PARAMTYPE_BOOL:
			SEventParameter(pcIndex, *(bool*)pValue);
			break;

		case S_PARAMTYPE_FLOAT:
			SEventParameter(pcIndex, *(float*)pValue);
			break;

		case S_PARAMTYPE_PTR:
			SEventParameter(pcIndex, *(void**)pValue);
			break;

		case S_PARAMTYPE_ZTSTR:
			SEventParameter(pcIndex, *(char**)pValue);			
			break;

		default:
			return;
		}				
	}

	// ******************************************************************************************

	S_API SEventParameter::SEventParameter(char* pcIndex, const int& iValue)
	{
		if (!AssignIndex(pcIndex)) return;
		
		m_pValue = new int(iValue);
	}

	// ******************************************************************************************

	S_API SEventParameter::SEventParameter(char* pcIndex, const float& fValue)
	{
		if (!AssignIndex(pcIndex)) return;

		m_pValue = new float(fValue);
	}

	// ******************************************************************************************

	S_API SEventParameter::SEventParameter(char* pcIndex, char* cValue)
	{
		if (!AssignIndex(pcIndex)) return;

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

	S_API SEventParameter::SEventParameter(char* pcIndex, const bool& bValue)
	{
		if (!AssignIndex(pcIndex)) return;

		m_pValue = new bool(bValue);
	}

	// ******************************************************************************************

	S_API SEventParameter::SEventParameter(char* pcIndex, void* pPointerValue)
	{
		if (!AssignIndex(pcIndex)) return;

		m_pValue = new void*(pPointerValue);
	}

	// ******************************************************************************************

	S_API SEventParameter::~SEventParameter()
	{
		if (m_pValue) delete m_pValue;
		if (m_pcIndex) delete m_pcIndex;
		m_pValue = 0;
		m_pcIndex = 0;
	}
}