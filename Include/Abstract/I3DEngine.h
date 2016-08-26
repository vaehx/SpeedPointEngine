#pragma once

#include "IRenderer.h"
#include "ITerrain.h"
#include "CHelper.h"
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
	virtual void SetTransform(const SMatrix& transform) = 0;

	// viewProj - The custom view-projection matrix to apply when rendering this renderobject.
	//			  If viewproj == 0, the custom viewproj matrix is unset and the renderer uses the viewproj of the viewport
	virtual void SetCustomViewProjMatrix(const SMatrix* viewProj) = 0;

	virtual IVertexBuffer* GetVertexBuffer() = 0;
	virtual IIndexBuffer* GetIndexBuffer(unsigned int subset = 0) = 0;

	// Called by the Renderer System
	virtual void Update() = 0;

	virtual void OnRelease() = 0;
};


//////////////////////////////////////////////////////////////////////////////////////////////////////////////////

struct SHelperRenderObject
{
	CHelper* pHelper;
	bool releaseAfterRender;

	SHelperRenderObject()
		: pHelper(0),
		releaseAfterRender(false)
	{
	}

	~SHelperRenderObject()
	{
		if (pHelper)
			delete pHelper;

		pHelper = 0;
		releaseAfterRender = false;
	}
};


//////////////////////////////////////////////////////////////////////////////////////////////////////////////////

//TODO: Refactor this to the actual "IRenderer"
struct S_API I3DEngine
{
protected:
	virtual void SetRenderObjectPool(IComponentPool<IRenderObject>* pPool) = 0;
	virtual SHelperRenderObject* GetHelperRenderObject() = 0;
	virtual SResult _CreateHelperPrefab(unsigned int id, const SHelperGeometryDesc* geometry) = 0;
	virtual bool _HelperPrefabExists(unsigned int id) const = 0;

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


	// Will not override existing prefabs!
	template<typename T>
	ILINE SResult CreateHelperPrefab()
	{
		unsigned int typeId = T().GetTypeId();
		if (typeId != SP_HELPER_DYNAMIC_MESH)
		{
			if (_HelperPrefabExists(typeId))
				return S_SUCCESS;

			if (Failure(_CreateHelperPrefab(typeId * 2, T::GetBaseGeometry(false))))
				return S_ERROR;

			if (Failure(_CreateHelperPrefab(typeId * 2 + 1, T::GetBaseGeometry(true))))
				return S_ERROR;

			return S_SUCCESS;
		}
		else
		{
			return S_INVALIDPARAM;
		}
	}

	template<typename T>
	ILINE T* AddHelper(
		const typename T::Params& params = T::Params(),
		const SHelperRenderParams& renderParams = SHelperRenderParams(),
		bool releaseAfterRender = false)
	{
		T* pInst = new T();
		pInst->SetParams(params);

		SHelperRenderObject* pHO = GetHelperRenderObject();
		pHO->pHelper = (CHelper*)pInst;
		pHO->pHelper->SetRenderParams(renderParams);
		pHO->releaseAfterRender = releaseAfterRender;

		return pInst;
	}

	template<typename T>
	ILINE T* AddHelper(const typename T::Params& params, bool releaseAfterRender)
	{
		return AddHelper<T>(params, releaseAfterRender);
	}

	ILINE virtual void ClearHelperRenderObjects() = 0;



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
