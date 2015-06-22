
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
	ITerrain* m_pTerrain;
	ISkyBox* m_pSkyBox;
	std::vector<SSceneNode>* m_pSceneNodes;

	void CheckSceneNodesArray();

public:
	Scene();

	virtual ~Scene()
	{
		Clear();
	}

	virtual SResult Initialize(IGameEngine* pGameEngine);

	virtual void SetSkyBox(ISkyBox* pSkyBox);
	virtual ISkyBox* GetSkyBox() const;

	virtual ITerrain* GetTerrain() const { return m_pTerrain; }
	virtual ITerrain* CreateTerrain(unsigned int nSegs, unsigned int nChunkSegs, float fSideSz, float baseHeight, float fChunkStepDist, unsigned int nLodLevels);

	virtual IStaticObject* LoadStaticObjectFromFile(const char* file);
	
	virtual SResult CreateNormalsGeometry(IRenderableObject* object, IRenderableObject** pNormalGeometryObject) const;

	virtual std::vector<SSceneNode>* GetSceneNodes();

	virtual void AddObject(IObject* pObject);
	virtual SResult AddSceneNode(const SSceneNode& node);

	virtual void Clear();
};

SP_NMSPACE_END