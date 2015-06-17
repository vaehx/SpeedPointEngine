#pragma once

#include <SPrerequisites.h>

SP_NMSPACE_BEG

struct S_API IScene;

// Handles order and batching of renderable objects in the scene
struct S_API I3DEngine
{
	virtual ~I3DEngine() {};

	virtual void SetScene(IScene* pScene) = 0;


	// Schedule Building:

	virtual void CollectRenderingObjects(const Vec3f& camPos, const Vec3f& camDir) = 0;
	virtual void RenderCollected() = 0;	



	// Schedule unleash

	virtual void Unleash() = 0;
	virtual void UnleashRenderObjects() = 0;	
};


SP_NMSPACE_END