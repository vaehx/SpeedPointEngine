// ********************************************************************************************

// This file is part of the SpeedPoint Engine

// ********************************************************************************************

#include <Observer\SObservedObject.h>

namespace SpeedPoint
{

	// ********************************************************************************************
	
	S_API SObservedObject::SObservedObject()		
	{
	}

	// ********************************************************************************************

	S_API SObservedObject::SObservedObject(SEventCallback pDestructionCallbackFunction)
	{
		SetDestructionCallback(pDestructionCallbackFunction);
	}

	// ********************************************************************************************

	S_API SObservedObject::~SObservedObject()
	{
		if (m_EventCallbackTable.IsInitialized())
		{
			bool bRes = true;
			SEventParameters params;
			if (bRes && Failure(params.Initialize(1))) bRes = false;
			if (bRes && Failure(params.Add(ePARAM_SENDER, S_PARAMTYPE_PTR, this))) bRes = false;
			if (bRes && Failure(m_EventCallbackTable.Call(S_E_DESTRUCT, &params))) bRes = false;
		}
	}

	// ********************************************************************************************
	
	S_API void SObservedObject::SetDestructionCallback(SEventCallback pDestructionCallbackFunc)
	{
		if (pDestructionCallbackFunc == 0) return;

		if (Failure(m_EventCallbackTable.Clear(S_E_DESTRUCT)))
			return;

		if (Failure(m_EventCallbackTable.Add(S_E_DESTRUCT, pDestructionCallbackFunc)))
			return;
	}

	// ********************************************************************************************

	S_API SResult SObservedObject::CallEvent(unsigned int iIndex, SEventParameters* pParams)
	{
		if (iIndex > 99999 || pParams == 0)
			return S_INVALIDPARAM;

		return m_EventCallbackTable.Call(iIndex, pParams);
	}

	// ********************************************************************************************

	S_API SResult SObservedObject::CallEvent(unsigned int iIndex, SEventParameter* pParamArray, unsigned int nParamCount)
	{
		if (iIndex > 99999 || pParamArray == 0)
			return S_INVALIDPARAM;

		// Prepare Params
		SEventParameters params;
		if (Failure(params.AddArray(pParamArray, nParamCount)))
			return S_ERROR;

		return m_EventCallbackTable.Call(iIndex, &params);
	}

}