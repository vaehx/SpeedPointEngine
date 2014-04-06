// ********************************************************************************************

// This file is part of the SpeedPoint Engine

// ********************************************************************************************

#include <Observer\SObservedObject.h>

namespace SpeedPoint
{

	// ********************************************************************************************
	
	SObservedObject::SObservedObject()		
	{
	}

	// ********************************************************************************************

	SObservedObject::SObservedObject(SEventCallback pDestructionCallbackFunction)
	{
		SetDestructionCallback(pDestructionCallbackFunction);
	}

	// ********************************************************************************************

	SObservedObject::~SObservedObject()
	{
		if (m_DestructionHandler.IsInitialized())
		{
			bool bRes = true;
			SEventParameters params;
			if (bRes && Failure(params.Initialize(1))) bRes = false;
			if (bRes && Failure(params.Add("sender", S_PARAMTYPE_PTR, this))) bRes = false;
			if (bRes && Failure(m_DestructionHandler.Call(0))) bRes = false;
		}
	}

	// ********************************************************************************************
	
	void SObservedObject::SetDestructionCallback(SEventCallback pDestructionCallbackFunc)
	{
		m_DestructionHandler.Initialize(pDestructionCallbackFunc);
	}
}