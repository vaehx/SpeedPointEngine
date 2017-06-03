//////////////////////////////////////////////////////////////////////////////////////////////////
//
//	SpeedPoint Game Engine
//	Copyright (c) 2011-2017 Pascal Rosenkranz, All rights reserved.
//
//////////////////////////////////////////////////////////////////////////////////////////////////

#pragma once

#include <Common\SerializationTools.h>

SP_NMSPACE_BEG

struct S_API IEntity;

/*

	An entity recipe defines the properties and the components of the entity.

	Entity recipes may inherit one or more other recipes. This allows recipes
	to extend each other in a very flexible way.

	Entity recipes are designed to be used with scripts and scene serialization.

	The recipe of an entity can only be specified when spawning it. There is no
	altering after spawning possible.

*/

struct S_API IEntityRecipe
{
protected:
	vector<IEntityRecipe*> m_InheritedRecipes;

	template<class EntityRecipeImpl>
	void Inherit()
	{
		IEntityRecipe* recipe = static_cast<IEntityRecipe*>(new EntityRecipeImpl());

		for (auto itRecipe = m_InheritedRecipes.begin(); itRecipe != m_InheritedRecipes.end(); ++itRecipe)
		{
			if (*itRecipe && (*itRecipe)->GetName() == recipe->GetName())
			{
				// Already inherited, do not add a new one
				delete recipe;
				return;
			}
		}

		m_InheritedRecipes.push_back(recipe);
	}

public:
	virtual ~IEntityRecipe()
	{
		for (auto itRecipe = m_InheritedRecipes.begin(); itRecipe != m_InheritedRecipes.end(); ++itRecipe)
			delete *itRecipe;
	}

	// Registers properties and creates the necessary components.
	// This call is propagated recursively to the inherited recipes.
	// Returns true on success, false on failure.
	virtual bool Apply(IEntity* entity)
	{
		bool success = true;
		for (auto itRecipe = m_InheritedRecipes.begin(); itRecipe != m_InheritedRecipes.end(); ++itRecipe)
		{
			IEntityRecipe* inheritedRecipe = *itRecipe;
			if (!inheritedRecipe)
				continue;

			if (!inheritedRecipe->Apply(entity))
				success = false;
		}

		return success;
	}

	virtual const char* GetName() const = 0;

	virtual const vector<IEntityRecipe*>& GetInheritedRecipes() const
	{
		return m_InheritedRecipes;
	}
};

struct S_API IEntityRecipeManager
{
protected:
	virtual void AddRecipe(IEntityRecipe* recipe) = 0;

public:
	virtual ~IEntityRecipeManager() {}

	// Overrides existing recipes with the same name
	template<class EntityRecipeImpl>
	void RegisterRecipe()
	{
		AddRecipe(static_cast<IEntityRecipe*>(new EntityRecipeImpl()));
	}

	virtual IEntityRecipe* GetRecipe(const string& name) const = 0;
	virtual void RemoveRecipe(const string& name) = 0;
	virtual void Clear() = 0;

	// Returns true on success, false if something failed
	virtual bool ApplyRecipe(IEntity* entity, const string& name) const = 0;
};

SP_NMSPACE_END
