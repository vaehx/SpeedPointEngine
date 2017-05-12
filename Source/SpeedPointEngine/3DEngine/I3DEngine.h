#pragma once

#include "RenderMesh.h"
#include "RenderLight.h"
#include "ITerrain.h"
#include "CHelper.h"
#include <Renderer\IRenderer.h>
#include <Common\ComponentPool.h>
#include <Common\SPrerequisites.h>

SP_NMSPACE_BEG

struct S_API ISkyBox;
struct S_API IGeometryManager;
struct S_API ITexture;
struct S_API IMaterialManager;
struct S_API IParticleSystem;


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

struct S_API SHUDElement
{
	ITexture* pTexture;
	unsigned int pos[2];
	unsigned int size[2];
};

//////////////////////////////////////////////////////////////////////////////////////////////////////////////////

struct S_API SEnvironmentSettings
{
	Vec3f sunPosition;
	float fogStart;
	float fogEnd;
};

//////////////////////////////////////////////////////////////////////////////////////////////////////////////////

//TODO: Refactor this to the actual "IRenderer"
struct S_API I3DEngine
{
protected:
	virtual void SetRenderMeshPool(IComponentPool<CRenderMesh>* pPool) = 0;
	virtual void SetRenderLightPool(IComponentPool<CRenderLight>* pPool) = 0;

	virtual SHelperRenderObject* GetHelperRenderObject() = 0;
	virtual SResult _CreateHelperPrefab(unsigned int id, const SHelperGeometryDesc* geometry) = 0;
	virtual bool _HelperPrefabExists(unsigned int id) const = 0;

public:
	virtual ~I3DEngine() {};

	ILINE virtual IRenderer* GetRenderer() const = 0;
	ILINE virtual IGeometryManager* GetGeometryManager() = 0;
	ILINE virtual IMaterialManager* GetMaterialManager() = 0;
	ILINE virtual IParticleSystem* GetParticleSystem() = 0;

	template<class RenderMeshImpl> void SetRenderMeshImplementation() { SetRenderMeshPool(new ComponentPool<CRenderMesh, RenderMeshImpl>()); }
	ILINE virtual CRenderMesh* CreateMesh(const SRenderMeshParams& params = SRenderMeshParams()) = 0;
	ILINE virtual void ClearRenderMeshes() = 0;

	template<class RenderLightImpl> void SetRenderLightImplementation() { SetRenderLightPool(new ComponentPool<CRenderLight, RenderLightImpl>()); }
	ILINE virtual CRenderLight* CreateLight() = 0;
	ILINE virtual void ClearRenderLights() = 0;


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
		return AddHelper<T>(params, SHelperRenderParams(), releaseAfterRender);
	}

	ILINE virtual void ClearHelperRenderObjects() = 0;



	ILINE virtual ITerrain* CreateTerrain(const STerrainParams& info) = 0;
	ILINE virtual ITerrain* GetTerrain() = 0;
	ILINE virtual void ClearTerrain() = 0;

	ILINE virtual ISkyBox* GetSkyBox() = 0;

	ILINE virtual void SetEnvironmentSettings(const SEnvironmentSettings& settings) = 0;

	ILINE virtual SHUDElement* CreateHUDElement() = 0;
	ILINE virtual void RemoveHUDElement(SHUDElement** pHUDElement) = 0;


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
