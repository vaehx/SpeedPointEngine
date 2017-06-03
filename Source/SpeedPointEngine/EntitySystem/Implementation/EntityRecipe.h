//////////////////////////////////////////////////////////////////////////////////////////////////
//
//	SpeedPoint Game Engine
//	Copyright (c) 2011-2017 Pascal Rosenkranz, All rights reserved.
//
//////////////////////////////////////////////////////////////////////////////////////////////////

#pragma once

#include "..\IEntityRecipe.h"

SP_NMSPACE_BEG

class S_API CRenderMesh;

class S_API EntityRecipeManager : public IEntityRecipeManager
{
private:
	map<string, IEntityRecipe*> m_Recipes;

protected:
	// Recipe will be OWNED by the manager, thus will be deleted on removal
	virtual void AddRecipe(IEntityRecipe* receipt);

public:
	virtual ~EntityRecipeManager()
	{
	}

	virtual IEntityRecipe* GetRecipe(const string& name) const;
	virtual bool ApplyRecipe(IEntity* entity, const string& name) const;
	virtual void RemoveRecipe(const string& name);
	virtual void Clear();
};

SP_NMSPACE_END
