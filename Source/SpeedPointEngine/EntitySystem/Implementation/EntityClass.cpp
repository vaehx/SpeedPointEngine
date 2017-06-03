//////////////////////////////////////////////////////////////////////////////////////////////////
//
//	SpeedPoint Game Engine
//	Copyright (c) 2011-2017 Pascal Rosenkranz, All rights reserved.
//
//////////////////////////////////////////////////////////////////////////////////////////////////

#include "EntityClass.h"

SP_NMSPACE_BEG

S_API void EntityClassManager::AddClass(IEntityClass* pClass)
{
	if (!pClass)
		return;

	m_Classes[pClass->GetName()] = pClass;
}

S_API IEntityClass* EntityClassManager::GetClass(const string& name) const
{
	auto found = m_Classes.find(name);
	if (found == m_Classes.end())
		return 0;
	else
		return found->second;
}

S_API bool EntityClassManager::ApplyClass(IEntity* entity, const string& name) const
{
	if (!entity)
		return false;

	IEntityClass* pClass = GetClass(name);
	if (!pClass)
	{
		CLog::Log(S_ERROR, "Failed EntityClassManager::ApplyClass('%s'): Class not found", name.c_str());
		return false;
	}

	return pClass->Apply(entity);
}

S_API void EntityClassManager::RemoveClass(const string& name)
{
	auto found = m_Classes.find(name);
	if (found == m_Classes.end())
		return;

	delete found->second;
	found->second = 0;

	m_Classes.erase(name);
}

S_API void EntityClassManager::Clear()
{
	for (auto itClass = m_Classes.begin(); itClass != m_Classes.end(); ++itClass)
	{
		delete itClass->second;
		itClass->second = 0;
	}

	m_Classes.clear();
}

SP_NMSPACE_END
