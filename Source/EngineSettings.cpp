// SpeedPoint Engine Settings

#include <EngineSettings.h>

SP_NMSPACE_BEG


// ------------------------------------------------------------------------------------------
S_API EngineSettings::EngineSettings()
: m_pGameEngine(nullptr)
{
}

// ------------------------------------------------------------------------------------------
S_API EngineSettings::~EngineSettings()
{
}

// ------------------------------------------------------------------------------------------
S_API SResult EngineSettings::Initialize(IGameEngine* pGameEngine)
{
	m_pGameEngine = pGameEngine;
	return (m_pGameEngine) ? S_SUCCESS : S_INVALIDPARAM;
}

// ------------------------------------------------------------------------------------------
S_API void EngineSettings::SetTerrainDetailMapFadeRadius(float radius)
{
	m_Desc.render.fTerrainDMFadeRange = radius;
}

// ------------------------------------------------------------------------------------------
S_API bool EngineSettings::SetFrontFaceType(EFrontFace ffType)
{
	ENGSETTING_SET_GETEQ(m_Desc.render.frontFaceType, ffType);
}



SP_NMSPACE_END