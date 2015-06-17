#pragma once

#include <SPrerequisites.h>
#include <Abstract\I3DEngine.h>
#include <vector>

SP_NMSPACE_BEG

struct S_API IScene;
struct S_API IRenderableObject;

class S_API C3DEngine : public I3DEngine
{
private:
	IScene* m_pScene;
	IRenderer* m_pRenderer;
	std::vector<IRenderableObject*>* m_pRenderObjects;	

public:
	C3DEngine(IRenderer* pRenderer);
	virtual ~C3DEngine();

	virtual void SetScene(IScene* pScene);

	virtual void CollectRenderingObjects(const Vec3f& camPos, const Vec3f& camDir);
	virtual void Unleash();
	virtual void UnleashRenderObjects();
	virtual void RenderTerrain();
	virtual void RenderSkyBox();
};


SP_NMSPACE_END