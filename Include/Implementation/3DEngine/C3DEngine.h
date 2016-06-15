#pragma once

#include <SPrerequisites.h>
#include <Abstract\I3DEngine.h>
#include <Abstract\ChunkedObjectPool.h>
#include <Abstract\IRenderer.h>
#include <vector>

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


	// TODO: Consider storing the RenderDescs in the engine, passing a pointer to Object::UpdateRenderDesc() and thereby letting the
	// object decide, how the Render Desc is filled (maybe by a copy in the object itself?)
	ChunkedObjectPool<SRenderObject, 40> m_RenderObjects;

	// For deferred shading only
	ChunkedObjectPool<SRenderLight> m_RenderLights;

	SRenderDesc* m_pSkyBoxRenderDesc;
	STerrainRenderDesc m_TerrainRenderDesc;

	void AddVisibleStatic(IStaticObject* pStaticObject, const AABB& aabb);
	void AddVisibleEntity(IEntity* pEntity, const AABB& aabb);
	void AddVisibleLight(ILight* pLight, const AABB& aabb);


public:
	C3DEngine(IRenderer* pRenderer, IGameEngine* pEngine);
	virtual ~C3DEngine();
	
	ILINE virtual unsigned int CollectVisibleObjects(IScene* pScene, const SCamera* pCamera);

	// Returns the pointer to a renderObject of this frame.
	// DO NOT USE THE POINTER AFTER RENDERING!
	ILINE virtual SRenderObject* GetCustomRenderObject();

	// Render lastly collected visible objects
	ILINE virtual void RenderCollected();

	ILINE virtual void ClearRenderObjects();
};


SP_NMSPACE_END