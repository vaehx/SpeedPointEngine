#pragma once

#include "IRenderer.h"
#include <SPrerequisites.h>

SP_NMSPACE_BEG

struct S_API IScene;

struct S_API SRenderObject
{
#ifdef _DEBUG
	std::string name;
#endif
	SRenderDesc renderDesc;
	AABB aabb;
};

// Handles order and batching of renderable objects in the scene
struct S_API I3DEngine
{
	virtual ~I3DEngine() {};	

	// Returns a new render object
	ILINE virtual SRenderObject* GetRenderObject() = 0;

	// Flags that object as free and sets the pointer to 0
	ILINE virtual void ReleaseRenderObject(const SRenderObject** pObject) = 0;

	// Summary:
	//	Collects Render Descs of visible objects (including terrain, skybox, ocean, ...)
	// Returns:
	//	Number of collected objects
	ILINE virtual unsigned int CollectVisibleObjects(IScene* pScene, const SCamera *pCamera) = 0;

	// Render lastly collected visible objects
	ILINE virtual void RenderCollected() = 0;

	ILINE virtual void ClearRenderObjects() = 0;
};


SP_NMSPACE_END