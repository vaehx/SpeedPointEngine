#pragma once

#include <SPrerequisites.h>
#include <Abstract\I3DEngine.h>
#include <Abstract\ChunkedObjectPool.h>
#include <Abstract\IRenderer.h>
#include <vector>

SP_NMSPACE_BEG

struct S_API IScene;
struct S_API IRenderableObject;

class S_API C3DEngine : public I3DEngine
{
private:	
	IRenderer* m_pRenderer;


	// TODO: Consider storing the RenderDescs in the engine, passing a pointer to Object::UpdateRenderDesc() and thereby letting the
	// object decide, how the Render Desc is filled (maybe by a copy in the object itself?)
	std::vector<SRenderDesc*> m_RenderDescs;


	STerrainRenderDesc m_TerrainRenderDesc;

public:
	C3DEngine(IRenderer* pRenderer);
	virtual ~C3DEngine();
	
	ILINE virtual unsigned int CollectVisibleObjects(IScene* pScene, const SCamera* pCamera);

	// Render lastly collected visible objects
	ILINE virtual void RenderCollected();
};


SP_NMSPACE_END