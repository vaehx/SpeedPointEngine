#include "Terrain.h"
#include "CSkyBox.h"
#include "C3DEngine.h"
#include <Renderer\IResourcePool.h>
#include <Common\SPrerequisites.h>
#include <Common\ProfilingSystem.h>
#include <sstream>
#include <Windows.h>

#include <Renderer\DirectX11\DX11Renderer.h>

using std::stringstream;

SP_NMSPACE_BEG

S_API C3DEngine* C3DEngine::__p3DEngine = 0;


S_API C3DEngine::C3DEngine(IRenderer* pRenderer)
	: m_pRenderer(pRenderer),
	m_pMeshes(0),
	m_pLights(0),
	m_pSkyBox(0),
	m_pTerrain(0),
	m_MatMgr(this),
	m_pDebugTexture(0),
	m_bDrawNormals(false)
{
	CreateHelperPrefab<CPointHelper>();
	CreateHelperPrefab<CVectorHelper>();
	CreateHelperPrefab<CLineHelper>();
	CreateHelperPrefab<CBoxHelper>();
	CreateHelperPrefab<CSphereHelper>();
	CreateHelperPrefab<CPlaneHelper>();
	CreateHelperPrefab<CCylinderHelper>();

	C3DEngine::Set(this);

	m_ParticleSystem.Init(pRenderer);
}

S_API void C3DEngine::Init()
{
	SInitialGeometryDesc geomDesc;

	MakeSphere(&geomDesc, Vec3f(0), 1.0f, 25, 25);
	CreateLightVolume(eLIGHT_TYPE_OMNIDIRECTIONAL, &geomDesc);
	CleanupInitialGeometryDesc(geomDesc);

	CreateFullscreenPlane();
}

S_API void C3DEngine::CreateFullscreenPlane()
{
	IResourcePool* pResourcePool = m_pRenderer->GetResourcePool();

	SPMatrixOrthoRH(&m_FullscreenPlane.projMtx, 1.0f, 1.0f, -3.0f, 3.0f);
	m_FullscreenPlane.transform = Mat44::Identity;
	m_FullscreenPlane.bCustomViewProjMtx = true;
	m_FullscreenPlane.bDepthStencilEnable = false;
	m_FullscreenPlane.bInverseDepthTest = false;
	m_FullscreenPlane.nSubsets = 1;
	m_FullscreenPlane.pSubsets = new SRenderSubset[1];
	m_FullscreenPlane.pSubsets[0].bOnce = false;
	m_FullscreenPlane.pSubsets[0].render = true;
	m_FullscreenPlane.pSubsets[0].enableAlphaTest = false;

	SVertex fsPlaneVerts[] =
	{
		SVertex(-1.0f,  1.0f, 2.0f, 0, 0, 1.0f, 1.0f, 0, 0),
		SVertex(-1.0f, -1.0f, 2.0f, 0, 0, 1.0f, 1.0f, 0, 0),
		SVertex(1.0f, -1.0f, 2.0f, 0, 0, 1.0f, 1.0f, 0, 0),
		SVertex(1.0f,  1.0f, 2.0f, 0, 0, 1.0f, 1.0f, 0, 0)
	};

	SIndex fsPlaneIndices[] =
	{
		0, 1, 2,
		0, 2, 3
	};

	pResourcePool->AddVertexBuffer(&m_FullscreenPlane.pSubsets[0].drawCallDesc.pVertexBuffer);
	m_FullscreenPlane.pSubsets[0].drawCallDesc.pVertexBuffer->Initialize(m_pRenderer, eVBUSAGE_STATIC, fsPlaneVerts, 4);
	m_FullscreenPlane.pSubsets[0].drawCallDesc.iStartVBIndex = 0;
	m_FullscreenPlane.pSubsets[0].drawCallDesc.iEndVBIndex = 3;

	pResourcePool->AddIndexBuffer(&m_FullscreenPlane.pSubsets[0].drawCallDesc.pIndexBuffer);
	m_FullscreenPlane.pSubsets[0].drawCallDesc.pIndexBuffer->Initialize(m_pRenderer, eIBUSAGE_STATIC, fsPlaneIndices, 6);
	m_FullscreenPlane.pSubsets[0].drawCallDesc.iStartIBIndex = 0;
	m_FullscreenPlane.pSubsets[0].drawCallDesc.iEndIBIndex = 5;

	m_FullscreenPlane.pSubsets[0].drawCallDesc.primitiveType = PRIMITIVE_TYPE_TRIANGLELIST;
}

//////////////////////////////////////////////////////////////////////////////////////////////

S_API C3DEngine::~C3DEngine()
{
	Clear();
	C3DEngine::Set(0);
}

S_API void C3DEngine::Clear()
{
	ClearHelperPrefabs();
	ClearHelperRenderObjects();

	ClearRenderMeshes();
	delete m_pMeshes;
	m_pMeshes = 0;

	ClearRenderLights();
	delete m_pLights;
	m_pLights = 0;

	for (auto& lightVolume : m_LightVolumes)
	{
		SRenderDesc* rd = &lightVolume.second;
		if (!rd->pSubsets)
			continue;

		for (u16 iSubset = 0; iSubset < rd->nSubsets; ++iSubset)
		{
			SRenderSubset* subset = &rd->pSubsets[0];
			SP_SAFE_RELEASE(subset->drawCallDesc.pVertexBuffer);
			SP_SAFE_RELEASE(subset->drawCallDesc.pIndexBuffer);
		}

		delete[] rd->pSubsets;
		rd->pSubsets = 0;
	}

	m_LightVolumes.clear();

	m_pRenderer = 0;

	if (IS_VALID_PTR(m_pSkyBox))
		delete m_pSkyBox;

	m_pSkyBox = 0;

	if (IS_VALID_PTR(m_pTerrain))
		delete m_pTerrain;

	m_pTerrain = 0;

	m_GeoMgr.Clear();
}

S_API void C3DEngine::ClearHelperPrefabs()
{
	IResourcePool* pResources = 0;
	if (IS_VALID_PTR(m_pRenderer))
		pResources = m_pRenderer->GetResourcePool();

	if (IS_VALID_PTR(pResources))
	{
		for (auto itPrefab = m_HelperPrefabs.begin(); itPrefab != m_HelperPrefabs.end(); ++itPrefab)
		{
			SRenderDesc* prefab = &itPrefab->second;
			if (IS_VALID_PTR(prefab->pSubsets))
			{
				for (unsigned int i = 0; i < prefab->nSubsets; ++i)
				{
					SP_SAFE_RELEASE(prefab->pSubsets[i].drawCallDesc.pVertexBuffer);
					SP_SAFE_RELEASE(prefab->pSubsets[i].drawCallDesc.pIndexBuffer);
				}

				delete[] prefab->pSubsets;
				prefab->pSubsets = 0;
			}
		}
	}

	m_HelperPrefabs.clear();
}

//////////////////////////////////////////////////////////////////////////////////////////////

S_API void C3DEngine::SetRenderMeshPool(IComponentPool<CRenderMesh>* pPool)
{
	assert(IS_VALID_PTR(pPool));

	if (IS_VALID_PTR(m_pMeshes))
		delete m_pMeshes;

	m_pMeshes = pPool;
}

S_API void C3DEngine::ClearRenderMeshes()
{
	if (m_pMeshes)
	{
		unsigned int iterator;
		CRenderMesh* pMesh = m_pMeshes->GetFirst(iterator);
		while (pMesh)
		{
			pMesh->Release();
			pMesh = m_pMeshes->GetNext(iterator);
		}

		m_pMeshes->ReleaseAll();
	}
}

ILINE CRenderMesh* C3DEngine::CreateMesh(const SInitialGeometryDesc* pGeomDesc)
{
	if (!m_pMeshes)
		return 0;

	CRenderMesh* mesh = m_pMeshes->Get();
	if (Failure(mesh->Init(pGeomDesc)))
		CLog::Log(S_ERROR, "Failed init RenderMesh from initial geometry desc");

	return mesh;
}

//////////////////////////////////////////////////////////////////////////////////////////////

S_API void C3DEngine::SetRenderLightPool(IComponentPool<CRenderLight>* pPool)
{
	assert(IS_VALID_PTR(pPool));

	if (IS_VALID_PTR(m_pLights))
		delete m_pLights;

	m_pLights = pPool;
}

S_API void C3DEngine::ClearRenderLights()
{
	if (m_pLights)
	{
		unsigned int iterator;
		CRenderLight* pLight = m_pLights->GetFirst(iterator);
		while (pLight)
		{
			pLight->Release();
			pLight = m_pLights->GetNext(iterator);
		}

		m_pLights->ReleaseAll();
	}
}

S_API CRenderLight* C3DEngine::CreateLight()
{
	if (!m_pLights)
		return 0;

	return m_pLights->Get();
}

S_API void C3DEngine::CreateLightVolume(ELightType type, const SInitialGeometryDesc* pGeomDesc)
{
	IResourcePool* pResourcePool = 0;
	if (!m_pRenderer || !(pResourcePool = m_pRenderer->GetResourcePool()))
	{
		CLog::Log(S_ERROR, "Failed create light volume for type '%s': Renderer or resource pool not set or initialized", GetLightTypeName(type));
		return;
	}

	SRenderDesc* rd = &m_LightVolumes[type];
	rd->bCustomViewProjMtx = false;
	rd->bDepthStencilEnable = true;
	rd->bInverseDepthTest = false;
	rd->nSubsets = 1;
	rd->pSubsets = new SRenderSubset[1];
	
	SRenderSubset* subset = &rd->pSubsets[1];
	subset->bOnce = false;
	subset->enableAlphaTest = false;
	subset->render = true;
	
	SDrawCallDesc* dcd = &subset->drawCallDesc;
	SInitialSubsetGeometryDesc* subsetDesc = &pGeomDesc->pSubsets[0];
	dcd->primitiveType = pGeomDesc->primitiveType;

	pResourcePool->AddVertexBuffer(&dcd->pVertexBuffer);
	dcd->pVertexBuffer->Initialize(m_pRenderer, eVBUSAGE_STATIC, pGeomDesc->pVertices, pGeomDesc->nVertices);
	dcd->iStartVBIndex = 0;
	dcd->iEndVBIndex = pGeomDesc->nVertices - 1;

	pResourcePool->AddIndexBuffer(&dcd->pIndexBuffer);
	dcd->pIndexBuffer->Initialize(m_pRenderer, eIBUSAGE_STATIC, subsetDesc->pIndices, subsetDesc->nIndices);
	dcd->iStartIBIndex = 0;
	dcd->iEndIBIndex = subsetDesc->nIndices - 1;
}

//////////////////////////////////////////////////////////////////////////////////////////////

S_API unsigned int C3DEngine::CollectVisibleObjects(const SCamera* pCamera)
{
	unsigned int budgetTimer = ProfilingSystem::StartSection("C3DEngine::CollectVisibleObjects()");

	// TERRAIN	
	if (IS_VALID_PTR(m_pTerrain) && m_pTerrain->IsInited())
		m_pTerrain->UpdateRenderDesc(&m_TerrainRenderDesc);
	else
		m_TerrainRenderDesc.bRender = false;


	// SKYBOX
	if (IS_VALID_PTR(m_pSkyBox))
	{
		m_pSkyBox->Update();
	}

	// PARTICLES
	m_ParticleSystem.Update(0.0f);

	// MESHES
	m_GlobalAABB.Reset();
	unsigned int itMesh = 0;
	CRenderMesh* pMesh = m_pMeshes->GetFirst(itMesh);
	while (pMesh)
	{
		AABB aabb = pMesh->GetAABB().Transform(pMesh->GetRenderDesc()->transform);
		m_GlobalAABB.AddAABB(aabb);

		pMesh = m_pMeshes->GetNext(itMesh);
	}

	ProfilingSystem::EndSection(budgetTimer);
	return 0;
}




//////////////////////////////////////////////////////////////////////////////////////////////

S_API SResult C3DEngine::_CreateHelperPrefab(unsigned int id, const SHelperGeometryDesc* geometry)
{
	if (!IS_VALID_PTR(m_pRenderer))
		return CLog::Log(S_NOTINIT, "Failed create helper prefab: Renderer not initialized");

	IResourcePool* pResources = m_pRenderer->GetResourcePool();
	if (!IS_VALID_PTR(pResources))
		return CLog::Log(S_ERROR, "Failed create helper prefab: resource pool not initialized");


	auto inserted = m_HelperPrefabs.insert(std::pair<unsigned int, SRenderDesc>(id, SRenderDesc()));

	SRenderDesc* rd = &inserted.first->second;
	rd->bCustomViewProjMtx = false;
	rd->bDepthStencilEnable = true;
	rd->bInverseDepthTest = false;

	bool lines = (geometry->topology == PRIMITIVE_TYPE_LINES || geometry->topology == PRIMITIVE_TYPE_LINESTRIP);

	if (!geometry->vertices.empty() && (lines || (!lines && !geometry->indices.empty())))
	{
		rd->nSubsets = 1;
		rd->pSubsets = new SRenderSubset[1];

		SRenderSubset* subset = &rd->pSubsets[0];
		subset->bOnce = false;
		subset->render = true;
		subset->shaderResources.illumModel = eILLUM_HELPER;
		
		SDrawCallDesc* dcd = &subset->drawCallDesc;
		dcd->primitiveType = geometry->topology;

		if (Failure(pResources->AddVertexBuffer(&dcd->pVertexBuffer)))
			return CLog::Log(S_ERROR, "Failed create helper prefab: cannot create vertex buffer");

		if (Failure(dcd->pVertexBuffer->Initialize(m_pRenderer, eVBUSAGE_STATIC, &geometry->vertices[0], geometry->vertices.size())))
			return CLog::Log(S_ERROR, "Failed create helper prefab: cannot fill vertex buffer");

		dcd->iStartVBIndex = 0;
		dcd->iEndVBIndex = geometry->vertices.size() - 1;

		if (!lines)
		{
			if (Failure(pResources->AddIndexBuffer(&dcd->pIndexBuffer)))
				return CLog::Log(S_ERROR, "Failed create helper prefab: cannot create index buffer");

			if (Failure(dcd->pIndexBuffer->Initialize(m_pRenderer, eIBUSAGE_STATIC, &geometry->indices[0], geometry->indices.size())))
				return CLog::Log(S_ERROR, "Failed create helper prefab: cannot fill index buffer");

			dcd->iStartIBIndex = 0;
			dcd->iEndIBIndex = geometry->indices.size() - 1;
		}
	}
	else
	{
		rd->nSubsets = 0;
		rd->pSubsets = 0;
		return CLog::Log(S_ERROR, "Failed create helper prefab: no vertices or indices given");
	}

	return S_SUCCESS;
}

S_API bool C3DEngine::_HelperPrefabExists(unsigned int id) const
{
	return (m_HelperPrefabs.find(id * 2) != m_HelperPrefabs.end());
}

S_API void C3DEngine::ClearHelperRenderObjects()
{
	m_HelperPool.Clear();
}



//////////////////////////////////////////////////////////////////////////////////////////////

S_API ITerrain* C3DEngine::CreateTerrain(const STerrainParams& params)
{
	ClearTerrain();
	m_pTerrain = new Terrain();
	m_pTerrain->Init(m_pRenderer, params);
	return m_pTerrain;
}

S_API ITerrain* C3DEngine::GetTerrain()
{
	return m_pTerrain;
}

S_API void C3DEngine::ClearTerrain()
{
	if (IS_VALID_PTR(m_pTerrain))
		delete m_pTerrain;

	m_pTerrain = 0;
}


//////////////////////////////////////////////////////////////////////////////////////////////

S_API ISkyBox* C3DEngine::GetSkyBox()
{
	if (!IS_VALID_PTR(m_pSkyBox))
		m_pSkyBox = new CSkyBox();

	return m_pSkyBox;
}

//////////////////////////////////////////////////////////////////////////////////////////////

S_API void C3DEngine::CreateHUDRenderDesc()
{
	IResourcePool* pResources = m_pRenderer->GetResourcePool();

	SIZE vpSz = m_pRenderer->GetTargetViewport()->GetSize();
	SPMatrixOrthoRH(&m_HUDRenderDesc.projMtx, (float)vpSz.cx, (float)vpSz.cy, 0.1f, 1000.f);

	m_HUDRenderDesc.bCustomViewProjMtx = true;
	m_HUDRenderDesc.bDepthStencilEnable = false;
	m_HUDRenderDesc.nSubsets = 1;
	m_HUDRenderDesc.pSubsets = new SRenderSubset[1];

	SRenderSubset& subset = m_HUDRenderDesc.pSubsets[0];
	subset.bOnce = false;
	subset.render = true;
	subset.enableAlphaTest = true;
	
	vector<SVertex> vertices = {
		SVertex(-0.5f, -0.5f, 1.0f, 0, 0, -1.0f, 1.0f, 0, 0, 0, 1.0f),
		SVertex(-0.5f,  0.5f, 1.0f, 0, 0, -1.0f, 1.0f, 0, 0, 0, 0),
		SVertex( 0.5f,  0.5f, 1.0f, 0, 0, -1.0f, 1.0f, 0, 0, 1.0f, 0),
		SVertex( 0.5f, -0.5f, 1.0f, 0, 0, -1.0f, 1.0f, 0, 0, 1.0f, 1.0f)
	};

	vector<SIndex> indices = {
		0, 2, 1,
		0, 3, 2
	};

	pResources->AddVertexBuffer(&subset.drawCallDesc.pVertexBuffer);
	subset.drawCallDesc.pVertexBuffer->Initialize(m_pRenderer, eVBUSAGE_STATIC, &vertices[0], vertices.size());
	subset.drawCallDesc.iStartVBIndex = 0;
	subset.drawCallDesc.iEndVBIndex = vertices.size() - 1;

	pResources->AddIndexBuffer(&subset.drawCallDesc.pIndexBuffer);
	subset.drawCallDesc.pIndexBuffer->Initialize(m_pRenderer, eIBUSAGE_STATIC, &indices[0], indices.size());
	subset.drawCallDesc.iStartIBIndex = 0;
	subset.drawCallDesc.iEndIBIndex = indices.size() - 1;

	subset.drawCallDesc.primitiveType = PRIMITIVE_TYPE_TRIANGLELIST;
}

S_API SHUDElement* C3DEngine::CreateHUDElement()
{
	return m_HUDElements.Get();
}

S_API void C3DEngine::RemoveHUDElement(SHUDElement** pHUDElement)
{
	m_HUDElements.Release(pHUDElement);
}


//////////////////////////////////////////////////////////////////////////////////////////////

S_API void C3DEngine::SetEnvironmentSettings(const SEnvironmentSettings& env)
{
	m_EnvironmentSettings = env;

	SSceneConstants* pSceneConstants = m_pRenderer->GetSceneConstants();
	pSceneConstants->sunPosition = Vec4f(env.sunPosition, 1.0f);
	pSceneConstants->fogStart = env.fogStart;
	pSceneConstants->fogEnd = env.fogEnd;
}

//////////////////////////////////////////////////////////////////////////////////////////////

S_API void C3DEngine::UpdateSunViewProj()
{
	// Calculate the minimum orthogonal view frustum for the sun that fits the camera view frustum

	SSceneConstants* pSceneConstants = m_pRenderer->GetSceneConstants();

	m_pRenderer->GetTargetViewport()->RecalculateCameraViewMatrix();
	const Mat44& mtxView = m_pRenderer->GetTargetViewport()->GetCameraViewMatrix();
	const Mat44& mtxProj = m_pRenderer->GetTargetViewport()->GetProjectionMatrix();

	const float maxCamZ = -35.0f;
	const Vec3f dirToSunNormalized = Vec3Normalize(pSceneConstants->sunPosition.xyz());

	ViewFrustum frustum(mtxView, mtxProj); // camera view frustum
	Vec3f camFrustumCorners[8]; // in view-space of camera
	frustum.GetCorners(camFrustumCorners, true);
	float nearZ = camFrustumCorners[0].z;
	float farZ = camFrustumCorners[4].z;	
	float farZScale = (farZ < maxCamZ) ? (maxCamZ / (farZ - nearZ)) : 1.0f; // Limit maximum z

	Vec3f globalAABBCorners[8];
	m_GlobalAABB.GetCorners(globalAABBCorners);

	Mat44 mtxViewInv = SMatrixInvert(mtxView);
	SMatrixTranspose(&mtxViewInv);

	Mat44 mtxSunView;
	SPMatrixLookAtRH(&mtxSunView, dirToSunNormalized, Vec3f(0), Vec3f(0, 1.0f, 0));
	SMatrixTranspose(&mtxSunView);

	AABB aabb; // in view-space of light
	aabb.Reset();
	for (int i = 0; i < 8; ++i)
	{
		// Cam frustum corner
		if (i >= 4)
			camFrustumCorners[i] = camFrustumCorners[i - 4] + farZScale * (camFrustumCorners[i] - camFrustumCorners[i - 4]);

		aabb.AddPoint((mtxSunView * (mtxViewInv * Vec4f(camFrustumCorners[i], 1.0f))).xyz());

		// Global aabb corner
		float z =
			mtxSunView._31 * globalAABBCorners[i].x +
			mtxSunView._32 * globalAABBCorners[i].y +
			mtxSunView._33 * globalAABBCorners[i].z +
			mtxSunView._34;
		Vec3f p = (mtxSunView * Vec4f(globalAABBCorners[i], 1.0f)).xyz();

		if (z > aabb.vMax.z)
			aabb.vMax.z = z;
	}

	Mat44 mtxSunViewInv = SMatrixInvert(mtxSunView);
	Vec3f fittedSunPosVS = (Vec3f(aabb.vMin.x, aabb.vMin.y, aabb.vMax.z) + Vec3f(aabb.vMax.x, aabb.vMax.y, aabb.vMax.z)) * 0.5f;
	Vec3f fittedSunPos = (mtxSunViewInv * Vec4f(fittedSunPosVS, 1.0f)).xyz();

	SPMatrixLookAtRH(&mtxSunView, fittedSunPos, fittedSunPos - dirToSunNormalized, Vec3f(0, 1.0f, 0));

	Mat44 mtxSunProj;
	SPMatrixOrthoRH(&mtxSunProj, aabb.vMax.x - aabb.vMin.x, aabb.vMax.y - aabb.vMin.y, 0, (aabb.vMax.z - aabb.vMin.z));

	pSceneConstants->mtxSunViewProj = mtxSunView * mtxSunProj;
}

//////////////////////////////////////////////////////////////////////////////////////////////

S_API void C3DEngine::RenderCollected()
{
	unsigned int budgetTimer = ProfilingSystem::StartSection("C3DEngine::RenderCollected()");
	{
		UpdateSunViewProj();
		m_pRenderer->UpdateSceneConstants();
		
		// Shadowmap Prepass
		// TODO: Only render objects that are inside the ViewFrustum !!!!!
		m_pRenderer->BindShaderPass(eSHADERPASS_SHADOWMAP);
		RenderMeshes();

		// Skybox
		if (IS_VALID_PTR(m_pSkyBox))
		{
			m_pRenderer->BindShaderPass(eSHADERPASS_FORWARD);
			SRenderDesc* rd = m_pSkyBox->GetRenderDesc();
			m_pRenderer->Render(*rd);
		}

		// Z Prepass / GBuffer pass
		IShaderPass* pGBufferPass = m_pRenderer->BindShaderPass(eSHADERPASS_GBUFFER);
		m_pRenderer->RenderTerrain(m_TerrainRenderDesc);

		pGBufferPass->Bind();
		RenderMeshes();

		// Deferred light prepass
		m_pRenderer->BindShaderPass(eSHADERPASS_LIGHTPREPASS);
		RenderDeferredLights();

		// Shading and merging post-pass
		IShaderPass* pShadingPass = m_pRenderer->BindShaderPass(eSHADERPASS_SHADING);
		m_pRenderer->RenderTerrain(m_TerrainRenderDesc);

		pShadingPass->Bind();
		RenderMeshes();


		// Particles
		m_pRenderer->BindShaderPass(eSHADERPASS_PARTICLES);
		m_ParticleSystem.Render();

		// Helpers
		m_pRenderer->BindShaderPass(eSHADERPASS_FORWARD);
		RenderHelpers();

		// HUD
		RenderHUD();

		// Debugging stuff
		RenderDebugTexture();

		ProfilingSystem::EndSection(budgetTimer);
	}
}

//////////////////////////////////////////////////////////////////////////////////////////////

S_API void C3DEngine::RenderMeshes()
{
	if (!m_pMeshes)
		return;

	stringstream objectsTimerName;
	objectsTimerName << "C3DEngine::RenderCollected() - Render RenderObjects (" << m_pMeshes->GetNumObjects() << ")";

	unsigned int renderObjectsTimer = ProfilingSystem::StartSection(objectsTimerName.str().c_str());
	{
		unsigned int itMesh;
		CRenderMesh* pMesh = m_pMeshes->GetFirst(itMesh);
		while (pMesh)
		{

#ifdef _DEBUG		
			if (m_pRenderer->DumpingThisFrame())
				CLog::Log(S_DEBUG, "Rendering %s", pMesh->_name.c_str());
#endif

			pMesh->OnRender();

			SRenderDesc* rd = pMesh->GetRenderDesc();

			if (m_bDrawNormals)
				DrawNormalsForMesh(rd);

			m_pRenderer->Render(*rd);

			pMesh = m_pMeshes->GetNext(itMesh);

		}

		ProfilingSystem::EndSection(renderObjectsTimer);
	}
}

S_API void C3DEngine::DrawNormalsForMesh(const SRenderDesc* rd)
{
	if (!rd || !rd->pSubsets || rd->nSubsets == 0) return;

	for (unsigned int i = 0; i < rd->nSubsets; ++i)
	{
		SRenderSubset* subset = &rd->pSubsets[i];
		if (!subset->render) continue;

		SDrawCallDesc* dcd = &subset->drawCallDesc;
		if (!dcd->pVertexBuffer) continue;

		const SVertex* verts = dcd->pVertexBuffer->GetShadowBuffer();
		if (!verts) continue;

		for (unsigned int ivtx = dcd->iStartVBIndex; ivtx < dcd->iEndVBIndex; ++ivtx)
		{
			AddHelper<CVectorHelper>(CVectorHelper::Params(
				(rd->transform * Vec4f(verts[ivtx].x, verts[ivtx].y, verts[ivtx].z, 1.0f)).xyz(),
				(rd->transform * Vec4f(verts[ivtx].nx, verts[ivtx].ny, verts[ivtx].nz, 0.0f)).xyz(),
				1.0f), true);
		}
	}
}

//////////////////////////////////////////////////////////////////////////////////////////////

S_API void C3DEngine::RenderDeferredLights()
{
	DeferredLightShaderPass* pLightPass = dynamic_cast<DeferredLightShaderPass*>(m_pRenderer->GetShaderPass(eSHADERPASS_LIGHTPREPASS));
	
	DeferredLightShaderPass::SLightObjectConstants lightConstants;
	lightConstants.mtxViewportProjInv = SMatrixInvert(m_pRenderer->GetTargetViewport()->GetProjectionMatrix());

	unsigned int iLight;
	CRenderLight* pLight = m_pLights->GetFirst(iLight);
	while (pLight)
	{
		SLightParams& params = pLight->GetParams();
		lightConstants.lightPos = params.position;
		//lightConstants.lightDirection = params.direction;
		lightConstants.lightIntensity = params.intensity.ToFloat3();
		lightConstants.lightDecay = params.decay;
		lightConstants.lightMaxDistance = params.radius;

		switch (params.type)
		{
		case eLIGHT_TYPE_OMNIDIRECTIONAL:
		default:
			lightConstants.lightType = 1;
			break;
		}

		pLightPass->SetLightConstants(lightConstants);

		// Get light volume and render it
		auto lightVolume = m_LightVolumes.find(pLight->GetParams().type);
		if (lightVolume == m_LightVolumes.end())
			return;

		SRenderDesc* rd = &lightVolume->second;
		m_pRenderer->RenderDeferredLight(*rd);


		pLight = m_pLights->GetNext(iLight);
	}

	// Sun
	lightConstants.lightType = 0;
	lightConstants.lightDirection = -m_EnvironmentSettings.sunPosition;
	lightConstants.lightIntensity = m_EnvironmentSettings.sunIntensity.ToFloat3();

	pLightPass->SetLightConstants(lightConstants);

	// We don't use the view matrix in the LightPrepass VS shader, so we can use it for
	// the scene camera's view matrix.
	m_FullscreenPlane.viewMtx = m_pRenderer->GetTargetViewport()->GetCameraViewMatrix();

	m_pRenderer->RenderDeferredLight(m_FullscreenPlane);
}

//////////////////////////////////////////////////////////////////////////////////////////////

S_API void C3DEngine::RenderHelpers()
{
	unsigned int itHelper;
	SHelperRenderObject* pHelper = m_HelperPool.GetFirstUsedObject(itHelper);
	while (pHelper)
	{
		bool trash = true;
		if (IS_VALID_PTR(pHelper->pHelper) && !pHelper->pHelper->IsTrash())
		{
			trash = false;

			SRenderDesc* rd = 0;
			const SHelperRenderParams* renderParams = pHelper->pHelper->GetRenderParams();

			if (renderParams->visible)
			{
				if (pHelper->pHelper->GetTypeId() == SP_HELPER_DYNAMIC_MESH)
				{
					rd = pHelper->pHelper->GetDynamicMesh();
				}
				else
				{
					unsigned int prefab = pHelper->pHelper->GetTypeId() * 2 + (unsigned int)renderParams->outline;
					auto foundPrefab = m_HelperPrefabs.find(prefab);
					if (foundPrefab != m_HelperPrefabs.end())
					{
						rd = &foundPrefab->second;
						rd->transform = pHelper->pHelper->GetTransform();
					}
				}
			}

			if (IS_VALID_PTR(rd) && rd->nSubsets > 0 && IS_VALID_PTR(rd->pSubsets))
			{
				SPGetColorFloat3(&rd->pSubsets[0].shaderResources.diffuse, renderParams->color);
				rd->bDepthStencilEnable = renderParams->depthTestEnable;

				m_pRenderer->Render(*rd);
			}
		}

		if (pHelper->releaseAfterRender || trash)
		{
			delete pHelper->pHelper;
			pHelper->pHelper = 0;
			m_HelperPool.Release(&pHelper);
		}

		pHelper = m_HelperPool.GetNextUsedObject(itHelper);
	}
}

//////////////////////////////////////////////////////////////////////////////////////////////

S_API void C3DEngine::RenderHUD()
{
	if (!m_HUDRenderDesc.pSubsets)
		CreateHUDRenderDesc();

	if (m_HUDElements.GetUsedObjectCount() == 0)
		return;

	m_pRenderer->BindShaderPass(eSHADERPASS_GUI);

	SIZE vpSz = m_pRenderer->GetTargetViewport()->GetSize();

	unsigned int iHUDElement;
	SHUDElement* pHUDElement = m_HUDElements.GetFirstUsedObject(iHUDElement);
	while (pHUDElement)
	{
		m_HUDRenderDesc.transform =
			Mat44::MakeTranslationMatrix(Vec3f((float)pHUDElement->pos[0] - 0.5f * vpSz.cx, (float)pHUDElement->pos[1] - 0.5f * vpSz.cy, 1.0f))
			* Mat44::MakeScaleMatrix(Vec3f((float)pHUDElement->size[0], (float)pHUDElement->size[1], 1.0f));

		m_HUDRenderDesc.pSubsets[0].shaderResources.textureMap = pHUDElement->pTexture;

		m_pRenderer->Render(m_HUDRenderDesc);

		pHUDElement = m_HUDElements.GetNextUsedObject(iHUDElement);
	}
}

//////////////////////////////////////////////////////////////////////////////////////////////

ILINE void C3DEngine::DebugTexture(const string& name)
{
	m_pDebugTexture = m_pRenderer->GetResourcePool()->FindTexture(name);
}

S_API void C3DEngine::RenderDebugTexture()
{
	if (!m_pDebugTexture)
		return;

	m_pRenderer->BindShaderPass(eSHADERPASS_GUI);

	SIZE vpSz = m_pRenderer->GetTargetViewport()->GetSize();

	unsigned int width = 400;
	unsigned int size[] = { width, (unsigned int)((float)width * ((float)vpSz.cy / (float)vpSz.cx)) };
	unsigned int pos[] = { (unsigned int)(size[0] * 0.5f), (unsigned int)(size[1] * 0.5f) };

	m_HUDRenderDesc.transform =
		Mat44::MakeTranslationMatrix(Vec3f((float)pos[0] - 0.5f * vpSz.cx, (float)pos[1] - 0.5f * vpSz.cy, 1.0f))
		* Mat44::MakeScaleMatrix(Vec3f((float)size[0], (float)size[1], 1.0f));

	m_HUDRenderDesc.pSubsets[0].shaderResources.textureMap = m_pDebugTexture;
	m_HUDRenderDesc.pSubsets[0].enableAlphaTest = false;
	
	m_pRenderer->Render(m_HUDRenderDesc);

	m_HUDRenderDesc.pSubsets[0].enableAlphaTest = true;
}

SP_NMSPACE_END
