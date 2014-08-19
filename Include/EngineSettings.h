////////////////////////////////////////////////////////////////////////////////////////////////////////
//	SpeedPoint Engine Source File
//
//	(c) 2011-2014 Pascal Rosenkranz aka iSmokiieZz
//	All rights reserved.
////////////////////////////////////////////////////////////////////////////////////////////////////////

#pragma once

#include <SPrerequisites.h>
#include <Util\SColor.h>
#include <Abstract\IGameEngine.h>

SP_NMSPACE_BEG




// SpeedPoint Engine settings
class S_API EngineSettings : public IEngineSettings
{
private:
	IGameEngine* m_pGameEngine;
	SSettingsDesc m_Desc;


public:
	virtual ~EngineSettings();
	EngineSettings::EngineSettings();

	virtual SResult Initialize(IGameEngine* pGameEngine);

	virtual operator const SSettingsDesc&() const
	{
		return m_Desc;
	}
	virtual SSettingsDesc& Get() const
	{
		return (SSettingsDesc&)m_Desc;
	}


	virtual SResult Set(const SSettingsDesc& dsc);






	// Todo: Implement specific set functions from IEngineSettings








};


SP_NMSPACE_END