#include "Scene.h"
#include "Entity.h"
#include "..\IEntityClass.h"

SP_NMSPACE_BEG

// -------------------------------------------------------------------------------------------------
Scene::Scene()
{
}

// -------------------------------------------------------------------------------------------------
S_API void Scene::Clear()
{
	m_Name = "Scene";

	m_ExternalEntities.clear();

	// Cleanup non-child entities first until there are no more non-child entities
	// In case there actually exist entities afterwards that still have a parent,
	// they are forcefully deleted.
	unsigned int numDeleted;
	bool forceCleanup = false;
	while (true)
	{
		numDeleted = 0;
		unsigned int iEntity;
		CEntity* pEntity = m_Entities.GetFirstUsedObject(iEntity);
		while (pEntity)
		{
			if (pEntity->GetParent() == 0 || forceCleanup)
			{
				++numDeleted;
				pEntity->Clear();
				m_Entities.Release(&pEntity);
			}

			pEntity = m_Entities.GetNextUsedObject(iEntity);
		}

		if (forceCleanup)
			break;

		if (numDeleted == 0)
			forceCleanup = true;
	}

	m_Entities.Clear();
}

// -------------------------------------------------------------------------------------------------
S_API SResult Scene::Initialize()
{
	Clear();
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
S_API void Scene::AddEntity(IEntity* pEntity)
{
	if (!pEntity)
	{
		CLog::Log(S_ERROR, "Tried to add nullptr entity to scene");
		return;
	}

	for (auto itExtEntity = m_ExternalEntities.begin(); itExtEntity != m_ExternalEntities.end(); ++itExtEntity)
	{
		IEntity* pExtEntity = *itExtEntity;
		if (pExtEntity == pEntity)
			return; // already added
	}

	CLog::Log(S_INFO, "Added external entity '%s' to scene!", pEntity->GetName());
}

// -------------------------------------------------------------------------------------------------
S_API IEntity* Scene::SpawnEntity(const string& name, IEntityClass* pClass /*=0*/)
{
	CEntity* pEntity = m_Entities.Get();
	pEntity->SetName(name.c_str());

	if (pClass)
		pClass->Apply(pEntity);

	return pEntity;
}

// -------------------------------------------------------------------------------------------------
S_API vector<IEntity*> Scene::GetEntities(const AABB& aabb /*= AABB(Vec3f(-FLT_MAX), Vec3f(FLT_MAX))*/) const
{
	vector<IEntity*> entities;

	unsigned int iEntity = 0;
	IEntity* pEntity = m_Entities.GetFirstUsedObject(iEntity);
	while (pEntity)
	{
		if (aabb.ContainsPoint(pEntity->GetPos()))
			entities.push_back(pEntity);

		pEntity = m_Entities.GetNextUsedObject(iEntity);
	}

	for (auto itExtEntity = m_ExternalEntities.begin(); itExtEntity != m_ExternalEntities.end(); ++itExtEntity)
	{
		pEntity = *itExtEntity;
		if (pEntity && aabb.ContainsPoint(pEntity->GetPos()))
			entities.push_back(pEntity);
	}

	return entities;
}

SP_NMSPACE_END
