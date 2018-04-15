#pragma once

#include "Geometry.h"
#include "..\I3DEngine.h"
#include "Material.h"
#include "ParticleSystem.h"
#include <Renderer\IRenderer.h>
#include <Renderer\IOutputPlane.h>
#include <Common\ChunkedObjectPool.h>
#include <Common\SPrerequisites.h>
#include <map>

using std::map;

#ifdef _DEBUG
#include <string>
#endif

SP_NMSPACE_BEG

class S_API C3DEngine : public I3DEngine
{
private:	
	IRenderer* m_pRenderer;	
	CGeometryManager m_GeoMgr;
	MaterialManager m_MatMgr;
	CParticleSystem m_ParticleSystem;

	IComponentPool<CRenderMesh>* m_pMeshes;
	IComponentPool<CRenderLight>* m_pLights;

	AABB m_GlobalAABB;

	ChunkedObjectPool<SHelperRenderObject> m_HelperPool;
	map<unsigned int, SRenderDesc> m_HelperPrefabs; // index = (uint)type * 2 + (uint)bOutline

	map<ELightType, SRenderDesc> m_LightVolumes;
	SRenderDesc m_FullscreenPlane;

	ISkyBox* m_pSkyBox;
	SEnvironmentSettings m_EnvironmentSettings;
	
	STerrainRenderDesc m_TerrainRenderDesc;
	ITerrain* m_pTerrain;

	ChunkedObjectPool<SHUDElement> m_HUDElements;
	SRenderDesc m_HUDRenderDesc;

	ITexture* m_pDebugTexture;
	bool m_bDrawNormals;


	void CreateFullscreenPlane();
	void CreateLightVolume(ELightType type, const SInitialGeometryDesc* pGeomDesc);

	void ClearHelperPrefabs();
	void CreateHUDRenderDesc();
	void UpdateSunViewProj();

	void RenderMeshes(unsigned int flags);
	void DrawNormalsForMesh(const SRenderDesc* rd);
	void RenderHelpers();
	void RenderDeferredLights();
	void RenderHUD();
	void RenderDebugTexture();

protected:
	virtual void SetRenderMeshPool(IComponentPool<CRenderMesh>* pPool);
	virtual void SetRenderLightPool(IComponentPool<CRenderLight>* pPool);
	
	virtual SHelperRenderObject* GetHelperRenderObject()
	{
		return m_HelperPool.Get();
	}

	virtual SResult _CreateHelperPrefab(unsigned int id, const SHelperGeometryDesc* geometry);
	virtual bool _HelperPrefabExists(unsigned int id) const;

public:
	C3DEngine(IRenderer* pRenderer);
	virtual ~C3DEngine();

	virtual void Init();

	ILINE virtual IRenderer* GetRenderer() const { return m_pRenderer; }	
	ILINE virtual IGeometryManager* GetGeometryManager() { return &m_GeoMgr; }
	ILINE virtual IMaterialManager* GetMaterialManager() { return &m_MatMgr; }
	ILINE virtual IParticleSystem* GetParticleSystem() { return &m_ParticleSystem; }
	
	ILINE virtual unsigned int CollectVisibleObjects(const SCamera* pCamera);

	ILINE virtual CRenderMesh* CreateMesh(const SInitialGeometryDesc* pGeomDesc = 0);
	ILINE virtual void ClearRenderMeshes();

	ILINE virtual CRenderLight* CreateLight();
	ILINE virtual void ClearRenderLights();


	ILINE virtual void ClearHelperRenderObjects();

	ILINE virtual ISkyBox* GetSkyBox();

	ILINE virtual void SetEnvironmentSettings(const SEnvironmentSettings& settings);
	ILINE const SEnvironmentSettings& GetEnvironmentSettings() const { return m_EnvironmentSettings; };

	// Render lastly collected visible objects
	ILINE virtual void RenderCollected();

	ILINE virtual ITerrain* CreateTerrain(const STerrainParams& info);
	ILINE virtual ITerrain* GetTerrain();
	ILINE virtual void ClearTerrain();

	ILINE virtual SHUDElement* CreateHUDElement();
	ILINE virtual void RemoveHUDElement(SHUDElement** pHUDElement);

	ILINE virtual void DebugTexture(const string& name);
	ILINE virtual void DebugNormals(bool showNormals = true) { m_bDrawNormals = showNormals; };

	ILINE virtual void Clear();


	// Static method hack to easily access C3DEngine implementation everywhere without
	// accessing the IGameEngine interface...
private:
	static C3DEngine* __p3DEngine;
public:
	inline static void Set(C3DEngine* p3DEngine)
	{
		__p3DEngine = p3DEngine;
	}
	inline static C3DEngine* Get()
	{
		return __p3DEngine;
	}
};


SP_NMSPACE_END
