#pragma once

#include <SPrerequisites.h>

SP_NMSPACE_BEG

struct S_API IScene;
struct S_API SCamera;
struct S_API SRenderObject;

// Handles order and batching of renderable objects in the scene
struct S_API I3DEngine
{
	virtual ~I3DEngine() {};	

	// Summary:
	//	Collects Render Descs of visible objects (including terrain, skybox, ocean, ...)
	// Returns:
	//	Number of collected objects
	ILINE virtual unsigned int CollectVisibleObjects(IScene* pScene, const SCamera *pCamera) = 0;
	
	// Can be used by the application to directly add render objects per-frame.
	// List of RenderObjects is cleared after frame. DO NOT USE POINTER AFTER RENDER!!
	ILINE virtual SRenderObject* GetCustomRenderObject() = 0;

	// Render lastly collected visible objects
	ILINE virtual void RenderCollected() = 0;

	ILINE virtual void ClearRenderObjects() = 0;
};


SP_NMSPACE_END