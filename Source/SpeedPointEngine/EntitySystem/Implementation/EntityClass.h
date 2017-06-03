//////////////////////////////////////////////////////////////////////////////////////////////////
//
//	SpeedPoint Game Engine
//	Copyright (c) 2011-2017 Pascal Rosenkranz, All rights reserved.
//
//////////////////////////////////////////////////////////////////////////////////////////////////

#pragma once

#include "..\IEntityClass.h"

SP_NMSPACE_BEG

class S_API CRenderMesh;

class S_API EntityClassManager : public IEntityClassManager
{
private:
	map<string, IEntityClass*> m_Classes;

protected:
	// Class will be OWNED by the manager, thus will be deleted on removal
	virtual void AddClass(IEntityClass* receipt);

public:
	virtual ~EntityClassManager()
	{
	}

	virtual IEntityClass* GetClass(const string& name) const;
	virtual bool ApplyClass(IEntity* entity, const string& name) const;
	virtual void RemoveClass(const string& name);
	virtual void Clear();
};

SP_NMSPACE_END
