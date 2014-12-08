
#include <Implementation\Geometry\Scene.h>
#include <Implementation\Geometry\Terrain.h>

SP_NMSPACE_BEG

// -------------------------------------------------------------------------------------------------
S_API void Scene::Clear()
{
	if (IS_VALID_PTR(m_pTerrain))
	{
		m_pTerrain->Clear();
		delete m_pTerrain;
	}

	m_pTerrain = nullptr;
}

// -------------------------------------------------------------------------------------------------
S_API SResult Scene::Initialize(IGameEngine* pGameEngine)
{
	SP_ASSERTR(IS_VALID_PTR(pGameEngine), S_INVALIDPARAM);
	m_pEngine = pGameEngine;
	return S_SUCCESS;
}

// -------------------------------------------------------------------------------------------------
S_API ITerrain* Scene::CreateTerrain(float width, float depth, unsigned int nX, unsigned int nZ,
	float baseHeight, ITexture* pColorMap, ITexture* pDetailMap)
{
	SP_ASSERTR(IS_VALID_PTR(m_pEngine), nullptr);
	if (IS_VALID_PTR(m_pTerrain))
		m_pTerrain->Clear();

	m_pTerrain = new Terrain();
	if (Failure(m_pTerrain->Initialize(m_pEngine, nX, nZ)))
		EngLog(S_ERROR, m_pEngine, "Failed initialize terrain in Scene::CreateTerrain");

	if (Failure(m_pTerrain->CreatePlanar(width, depth, baseHeight)))
		EngLog(S_ERROR, m_pEngine, "Failed create planar terrain in Scene::CreateTerrain");

	m_pTerrain->SetColorMap(pColorMap);
	m_pTerrain->SetDetailMap(pDetailMap);

	return m_pTerrain;
}

SP_NMSPACE_END