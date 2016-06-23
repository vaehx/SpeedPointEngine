#pragma once

#include "IRenderer.h"
#include "ITerrain.h"
#include "ComponentPool.h"
#include <SPrerequisites.h>

SP_NMSPACE_BEG

struct S_API ISkyBox;

struct S_API IRenderObject
{
#ifdef _DEBUG
	string _name;
#endif

	virtual void SetRenderer(I3DEngine* p3DEngine) = 0;

	virtual ~IRenderObject() {}

	virtual AABB GetAABB() = 0;
	
	virtual SRenderDesc* GetRenderDesc() = 0;

	// Called by the Renderer System
	virtual void Update() = 0;

	virtual void OnRelease() = 0;
};




//TODO: Refactor this to the actual "IRenderer"
struct S_API I3DEngine
{
protected:
	virtual void SetRenderObjectPool(IComponentPool<IRenderObject>* pPool) = 0;

public:
	virtual ~I3DEngine() {};

	template<class RenderObjImpl>
	ILINE void CreateRenderObjectPool()
	{
		SetRenderObjectPool(new ComponentPool<IRenderObject, RenderObjImpl>());
	}

	ILINE virtual IRenderer* GetRenderer() const = 0;

	// Summary:
	//	Finds and returns an empty Render Object
	ILINE virtual IRenderObject* GetRenderObject() = 0;

	// Summary:
	//	Releases the passed object in the pool and sets the pointer to 0
	ILINE virtual void ReleaseRenderObject(IRenderObject** pObject) = 0;

	ILINE virtual void ClearRenderObjects() = 0;

	ILINE virtual ITerrain* CreateTerrain(const STerrainInfo& info) = 0;
	ILINE virtual ITerrain* GetTerrain() = 0;
	ILINE virtual void ClearTerrain() = 0;

	ILINE virtual ISkyBox* GetSkyBox() = 0;

	// Summary:
	//	Collects Render Descs of visible objects (including terrain, skybox, ocean, ...)
	// Returns:
	//	Number of collected objects
	ILINE virtual unsigned int CollectVisibleObjects(const SCamera *pCamera) = 0;

	// Render lastly collected visible objects
	ILINE virtual void RenderCollected() = 0;

	ILINE virtual void Clear() = 0;
};


SP_NMSPACE_END
