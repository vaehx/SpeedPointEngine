// ********************************************************************************************

//	This file is part of the SpeedPoint Game Engine

//	(c) 2011-2014 Pascal Rosenkranz aka iSmokiieZz
//	All rights reserved.

// ********************************************************************************************

#include <OnionPhysics\SPhysicalSolidSystem.h>

namespace SpeedPoint
{
	S_API SResult SPhysicalSolidSystem::Initialize(SpeedPointEngine* pEngine)
	{
		return S_NOTIMPLEMENTED;
	}

	// ********************************************************************************************

	S_API SpeedPointEngine* SPhysicalSolidSystem::GetEngine()
	{
		return m_pEngine;
	}

	// ********************************************************************************************
	
	S_API SP_ID SPhysicalSolidSystem::AddSolid(void)
	{
		return SP_ID();
	}

	// ********************************************************************************************

	S_API SSolid* SPhysicalSolidSystem::GetSolid(SP_ID id)
	{
		return 0;
	}

	// ********************************************************************************************
	
	S_API unsigned int SPhysicalSolidSystem::GetSolidCount()
	{
		return 0;
	}
	
	// ********************************************************************************************

	S_API SResult SPhysicalSolidSystem::Clear(void)
	{
		return S_NOTIMPLEMENTED;
	}
}