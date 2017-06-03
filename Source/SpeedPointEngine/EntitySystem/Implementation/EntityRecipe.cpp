//////////////////////////////////////////////////////////////////////////////////////////////////
//
//	SpeedPoint Game Engine
//	Copyright (c) 2011-2017 Pascal Rosenkranz, All rights reserved.
//
//////////////////////////////////////////////////////////////////////////////////////////////////

#include "EntityRecipe.h"

SP_NMSPACE_BEG

S_API void EntityRecipeManager::AddRecipe(IEntityRecipe* recipe)
{
	if (!recipe)
		return;

	m_Recipes[recipe->GetName()] = recipe;
}

S_API IEntityRecipe* EntityRecipeManager::GetRecipe(const string& name) const
{
	auto found = m_Recipes.find(name);
	if (found == m_Recipes.end())
		return 0;
	else
		return found->second;
}

S_API bool EntityRecipeManager::ApplyRecipe(IEntity* entity, const string& name) const
{
	if (!entity)
		return false;

	IEntityRecipe* recipe = GetRecipe(name);
	if (!recipe)
	{
		CLog::Log(S_ERROR, "Failed EntityRecipeManager::ApplyRecipe('%s'): Recipe not found", name.c_str());
		return false;
	}

	return recipe->Apply(entity);
}

S_API void EntityRecipeManager::RemoveRecipe(const string& name)
{
	auto found = m_Recipes.find(name);
	if (found == m_Recipes.end())
		return;

	delete found->second;
	found->second = 0;

	m_Recipes.erase(name);
}

S_API void EntityRecipeManager::Clear()
{
	for (auto itRecipe = m_Recipes.begin(); itRecipe != m_Recipes.end(); ++itRecipe)
	{
		delete itRecipe->second;
		itRecipe->second = 0;
	}

	m_Recipes.clear();
}

SP_NMSPACE_END
