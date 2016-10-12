#include "Scene.h"
#include "Entity.h"
#include "RenderableComponent.h"
#include "PhysicalComponent.h"
#include <Abstract\IGeometry.h>
#include <Abstract\IResourcePool.h>
#include <Abstract\IMaterial.h>
#include <Abstract\I3DEngine.h>
#include <Abstract\IPhysics.h>

SP_NMSPACE_BEG

// -------------------------------------------------------------------------------------------------
S_API Scene::Scene()	
	: m_pEngine(0)
{
}

// -------------------------------------------------------------------------------------------------
S_API void Scene::Clear()
{
	for (auto itEntity = m_Entities.begin(); itEntity != m_Entities.end(); ++itEntity)
	{
		IEntity* pEntity = *itEntity;
		if (pEntity)
		{
			pEntity->Clear();
			delete pEntity;
		}
	}

	m_Entities.clear();

	m_pEngine = 0;
}

// -------------------------------------------------------------------------------------------------
S_API SResult Scene::Initialize(IGameEngine* pGameEngine)
{
	SP_ASSERTR(IS_VALID_PTR(pGameEngine), S_INVALIDPARAM);
	
	m_pEngine = pGameEngine;


	I3DEngine* p3DEngine = m_pEngine->Get3DEngine();
	if (p3DEngine)
	{
		p3DEngine->SetRenderMeshImplementation<CRenderMeshComponent>();
		p3DEngine->SetRenderLightImplementation<CRenderLightComponent>();
	}
	else
	{
		CLog::Log(S_ERROR, "Cannot create render object pools: 3DEngine not initialized");
	}


	IPhysics* pPhysics = m_pEngine->GetPhysics();
	if (pPhysics)
		m_pEngine->GetPhysics()->CreatePhysObjectPool<CPhysicalComponent>();
	else
		CLog::Log(S_ERROR, "Cannot create phys object pool: Physics not initialized");

	return S_SUCCESS;
}

// -------------------------------------------------------------------------------------------------
S_API void Scene::SetName(const string& name)
{
	m_Name = name;
}

S_API const string& Scene::GetName() const
{
	return m_Name;
}

// -------------------------------------------------------------------------------------------------
S_API void Scene::AddObject(IEntity* pEntity)
{
	if (!pEntity)
	{
		CLog::Log(S_ERROR, "Tried to add nullptr entity to scene");
		return;
	}

	m_Entities.push_back(pEntity);
	CLog::Log(S_INFO, "Added '%s' to scene!", pEntity->GetName());
}

// -------------------------------------------------------------------------------------------------
S_API IEntity* Scene::SpawnEntity()
{
	return new CEntity();
}

// -------------------------------------------------------------------------------------------------
S_API IEntity* Scene::LoadObjectFromFile(const char* filename, const char* objName)
{
	SP_ASSERTR(IS_VALID_PTR(m_pEngine), nullptr, "Engine not initialized!");

	I3DEngine* p3DEngine = SpeedPointEnv::GetEngine()->Get3DEngine();
	SP_ASSERTR(p3DEngine, nullptr, "3DEngine not initialized");

	// Create the entity
	IEntity* pEntity = SpawnEntity();
	pEntity->SetName(objName);

	SRenderMeshParams meshParams;
	string file = SpeedPointEnv::GetEngine()->GetResourcePath(filename);
	meshParams.pGeomDesc = p3DEngine->GetGeometryManager()->LoadGeometry(file.c_str());
#ifdef _DEBUG
	meshParams._name = objName;
#endif
	pEntity->AddComponent(p3DEngine->CreateMesh(meshParams));

	return pEntity;
}

SP_NMSPACE_END
