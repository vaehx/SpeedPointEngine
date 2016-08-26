#pragma once

#include <SPrerequisites.h>
#include <Abstract\I3DEngine.h>
#include <Abstract\ChunkedObjectPool.h>
#include <Abstract\IRenderer.h>
#include <vector>
#include <map>

using std::map;

#ifdef _DEBUG
#include <string>
#endif

SP_NMSPACE_BEG

struct S_API IScene;
struct S_API IRenderableObject;

class S_API C3DEngine : public I3DEngine
{
private:	
	IRenderer* m_pRenderer;	
	IGameEngine* m_pEngine;

	IComponentPool<IRenderObject>* m_pRenderObjects;
	ChunkedObjectPool<SHelperRenderObject> m_HelperPool;
	map<unsigned int, SRenderDesc> m_HelperPrefabs; // index = (uint)type * 2 + (uint)bOutline

	ISkyBox* m_pSkyBox;
	
	STerrainRenderDesc m_TerrainRenderDesc;
	ITerrain* m_pTerrain;


	void ClearHelperPrefabs();

protected:
	virtual void SetRenderObjectPool(IComponentPool<IRenderObject>* pPool);
	virtual SHelperRenderObject* GetHelperRenderObject()
	{
		return m_HelperPool.Get();
	}

	virtual SResult _CreateHelperPrefab(unsigned int id, const SHelperGeometryDesc* geometry);
	virtual bool _HelperPrefabExists(unsigned int id) const;

public:
	C3DEngine(IRenderer* pRenderer, IGameEngine* pEngine);
	virtual ~C3DEngine();

	ILINE virtual IRenderer* GetRenderer() const
	{
		return m_pRenderer;
	}
	
	ILINE virtual unsigned int CollectVisibleObjects(const SCamera* pCamera);
	ILINE virtual IRenderObject* GetRenderObject();
	ILINE virtual void ReleaseRenderObject(IRenderObject** pObject);
	ILINE virtual void ClearRenderObjects();

	ILINE virtual void ClearHelperRenderObjects();

	ILINE virtual ISkyBox* GetSkyBox();

	// Render lastly collected visible objects
	ILINE virtual void RenderCollected();

	ILINE virtual ITerrain* CreateTerrain(const STerrainInfo& info);
	ILINE virtual ITerrain* GetTerrain();
	ILINE virtual void ClearTerrain();

	ILINE virtual void Clear();
};


SP_NMSPACE_END