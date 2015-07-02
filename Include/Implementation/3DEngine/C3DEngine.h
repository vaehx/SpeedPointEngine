#pragma once

#include <SPrerequisites.h>
#include <Abstract\I3DEngine.h>
#include <Abstract\ChunkedObjectPool.h>
#include <Abstract\IRenderer.h>
#include <vector>

SP_NMSPACE_BEG

struct S_API IScene;
struct S_API IRenderableObject;

struct S_API SRenderLight
{
	SLightDesc lightDesc;
};

struct S_API SRenderObject
{
	SRenderDesc renderDesc;
	AABB aabb;
	
	// For forward rendering:
	SRenderLight* affectingLights[4];
	int nAffectingLights;

	SRenderObject()
		: nAffectingLights(0)
	{
		memset(&affectingLights, 0, sizeof(affectingLights));
	}
};

class S_API C3DEngine : public I3DEngine
{
private:	
	IRenderer* m_pRenderer;


	// TODO: Consider storing the RenderDescs in the engine, passing a pointer to Object::UpdateRenderDesc() and thereby letting the
	// object decide, how the Render Desc is filled (maybe by a copy in the object itself?)
	ChunkedObjectPool<SRenderObject> m_RenderObjects;

	// For deferred shading only
	ChunkedObjectPool<SRenderLight> m_RenderLights;


	STerrainRenderDesc m_TerrainRenderDesc;

	void AddVisibleStatic(IStaticObject* pStaticObject, const AABB& aabb);
	void AddVisibleEntity(IEntity* pEntity, const AABB& aabb);
	void AddVisibleLight(ILight* pLight, const AABB& aabb);

	void ClearRenderObjects();


public:
	C3DEngine(IRenderer* pRenderer);
	virtual ~C3DEngine();
	
	ILINE virtual unsigned int CollectVisibleObjects(IScene* pScene, const SCamera* pCamera);

	// Render lastly collected visible objects
	ILINE virtual void RenderCollected();
};


SP_NMSPACE_END