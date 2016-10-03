
#pragma once

#include <SPrerequisites.h>
#include <Abstract\IScene.h>
#include <vector>

using std::vector;

SP_NMSPACE_BEG

struct S_API ITerrain;
struct S_API IGameEngine;

class S_API Scene : public IScene
{
private:
	string m_Name;
	IGameEngine* m_pEngine;
	vector<IEntity*> m_Entities;

	void CheckSceneNodesArray();

public:
	Scene();

	virtual ~Scene()
	{
		Clear();
	}

	virtual SResult Initialize(IGameEngine* pGameEngine);
	virtual void Clear();

	virtual const vector<IEntity*>& GetEntities() const
	{
		return m_Entities;
	}

	virtual void SetName(const string& name);
	virtual const string& GetName() const;

	virtual IEntity* LoadObjectFromFile(const char* file, const char* objName);
	virtual IEntity* SpawnEntity();
	virtual void AddObject(IEntity* pEntity);
};

SP_NMSPACE_END
