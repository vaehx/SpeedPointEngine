#pragma once

#include "IRenderer.h"
#include "ChunkedObjectPool.h"
#include "ITerrain.h"
#include <SPrerequisites.h>

SP_NMSPACE_BEG

struct S_API ISkyBox;

struct S_API IRenderObject
{
#ifdef _DEBUG
	string _name;
#endif

	virtual ~IRenderObject() {}

	virtual const AABB& GetAABB() const = 0;
	
	virtual SRenderDesc* GetRenderDesc() = 0;

	// Called by the Renderer System
	virtual void Update() = 0;

	virtual void Clear() = 0;
};



struct S_API IRenderObjectPool
{	
	virtual IRenderObject* Get() = 0;
	
	// Returns:
	//	The first used render object in the pool or 0 if there is none in the pool
	// Arguments:
	//	id - is set to the index of the first used object
	virtual IRenderObject* GetFirst(unsigned int& id) const = 0;
	
	// Returns:
	//	The next render object in the pool after the given index or 0 if there is no more
	// Arguments:
	//	id - is set to the index of the next used object AFTER the given index
	virtual IRenderObject* GetNext(unsigned int& id) const = 0;

	// Returns:
	//	The number of used objects in the pool
	virtual unsigned int GetNumObjects() const = 0;

	// Returns true if the object was released, false if parameter is invalid
	virtual bool Release(IRenderObject** pObject) = 0;
	virtual void ReleaseAll() = 0;
};

// RenderObjImpl must be an implementation of IRenderObject
template<class RenderObjImpl>
class S_API RenderObjectPool : public IRenderObjectPool
{
private:
	ChunkedObjectPool<RenderObjImpl> m_Pool;

public:
	virtual IRenderObject* Get()
	{
		return dynamic_cast<IRenderObject*>(m_Pool.Get());
	}

	virtual IRenderObject* GetFirst(unsigned int& id) const
	{
		return dynamic_cast<IRenderObject*>(m_Pool.GetFirstUsedObject(id));
	}

	virtual IRenderObject* GetNext(unsigned int& id) const
	{
		return dynamic_cast<IRenderObject*>(m_Pool.GetNextUsedObject(id));
	}

	virtual unsigned int GetNumObjects() const
	{
		return m_Pool.GetUsedObjectCount();
	}

	virtual bool Release(IRenderObject** ppObject)
	{
		if (!ppObject)
			return false;

		IRenderObject* pObject = *ppObject;
		if (!pObject)
			return false;

		RenderObjImpl* pImplObject = dynamic_cast<RenderObjImpl*>(pObject);
		m_Pool.Release(&pImplObject);

		*ppObject = 0;
		return true;
	}

	virtual void ReleaseAll()
	{
		m_Pool.ReleaseAll();
	}

	ChunkedObjectPool<RenderObjImpl>* GetPool() const
	{
		return &m_Pool;
	}
};




//TODO: Refactor this to the actual "IRenderer"
struct S_API I3DEngine
{
protected:
	virtual void SetRenderObjectPool(IRenderObjectPool* pPool) = 0;

public:
	virtual ~I3DEngine() {};

	template<class RenderObjImpl>
	ILINE void CreateRenderObjectPool()
	{
		SetRenderObjectPool(new RenderObjectPool<RenderObjImpl>());
	}

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
