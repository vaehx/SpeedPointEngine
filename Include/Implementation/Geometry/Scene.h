
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

public:
	virtual ~Scene()
	{
		Clear();
	}

	virtual SResult Initialize(IGameEngine* pGameEngine);

	virtual ITerrain* GetTerrain() { return m_pTerrain; }
	virtual ITerrain* CreateTerrain(float width, float depth, unsigned int nX, unsigned int nZ, float baseHeight, ITexture* pColorMap, ITexture* pDetailMap);

	virtual IStaticObject* LoadStaticObjectFromFile(const char* file);

	virtual void Clear();
};

SP_NMSPACE_END