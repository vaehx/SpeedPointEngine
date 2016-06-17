
#pragma once

#include <SPrerequisites.h>
#include <Abstract\IScene.h>

SP_NMSPACE_BEG

struct S_API ITerrain;
struct S_API IGameEngine;

class S_API Scene : public IScene
{
private:
	IGameEngine* m_pEngine;
	std::vector<SSceneNode>* m_pSceneNodes;

	void CheckSceneNodesArray();

public:
	Scene();

	virtual ~Scene()
	{
		Clear();
	}

	virtual SResult Initialize(IGameEngine* pGameEngine);

	virtual ITerrain* CreateTerrain(unsigned int nSegs, unsigned int nChunkSegs, float fSideSz, float baseHeight, float fChunkStepDist, unsigned int nLodLevels, bool center = true);

	virtual IEntity* LoadObjectFromFile(const char* file, const char* objName);
	
	virtual SResult CreateNormalsGeometry(IRenderableComponent* renderable, SInitialGeometryDesc* pNormalsGeometry) const;

	virtual std::vector<SSceneNode>* GetSceneNodes();

	virtual SResult AddSceneNode(const SSceneNode& node);

	// Add dynamic object / entity
	virtual SResult AddObject(IObject* pObject);

	virtual void Clear();
};

SP_NMSPACE_END