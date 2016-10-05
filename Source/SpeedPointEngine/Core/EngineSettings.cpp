// SpeedPoint Engine Settings

#include "EngineSettings.h"
#include <Abstract\IScene.h>
#include <Abstract\ITerrain.h>
#include <Abstract\IRenderer.h>
#include <Abstract\I3DEngine.h>

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
	if (IS_VALID_PTR(m_pGameEngine))
	{
		ITerrain* pTerrain = m_pGameEngine->Get3DEngine()->GetTerrain();
		if (IS_VALID_PTR(pTerrain))
			pTerrain->RequireCBUpdate();
	}
}

// ------------------------------------------------------------------------------------------
S_API bool EngineSettings::SetFrontFaceType(EFrontFace ffType)
{
	ENGSETTING_SET_GETEQ(m_Desc.render.frontFaceType, ffType);
}

// ------------------------------------------------------------------------------------------
S_API void EngineSettings::EnableWireframe(bool state)
{
	m_Desc.render.bRenderWireframe = state;
	if (IS_VALID_PTR(m_pGameEngine))
		m_pGameEngine->GetRenderer()->EnableWireframe(state);
}

// ------------------------------------------------------------------------------------------
S_API void EngineSettings::EnableDepthTest(bool state)
{
	m_Desc.render.bEnableDepthTest = state;
	if (IS_VALID_PTR(m_pGameEngine))
		m_pGameEngine->GetRenderer()->EnableDepthTest(state);
}



SP_NMSPACE_END