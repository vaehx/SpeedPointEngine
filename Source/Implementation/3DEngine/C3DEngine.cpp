#include <Implementation\3DEngine\Terrain.h>
#include <Implementation\3DEngine\CSkyBox.h>
#include <Implementation\3DEngine\C3DEngine.h>
#include <Abstract\IResourcePool.h>
#include <Abstract\IEntity.h>
#include <Abstract\IScene.h>
#include <Abstract\IGameEngine.h>
#include <sstream>

using std::stringstream;

SP_NMSPACE_BEG


S_API C3DEngine::C3DEngine(IRenderer* pRenderer, IGameEngine* pEngine)
	: m_pRenderer(pRenderer),
	m_pEngine(pEngine),
	m_pRenderObjects(0),
	m_pSkyBox(0),
	m_pTerrain(0)
{
	CreateHelperPrefab<CPointHelper>();
	CreateHelperPrefab<CLineHelper>();
	CreateHelperPrefab<CBoxHelper>();
	CreateHelperPrefab<CSphereHelper>();
}

S_API C3DEngine::~C3DEngine()
{
	Clear();
}

S_API void C3DEngine::Clear()
{
	ClearHelperPrefabs();
	ClearHelperRenderObjects();

	ClearRenderObjects();
	if (IS_VALID_PTR(m_pRenderObjects))
		delete m_pRenderObjects;

	m_pRenderObjects = 0;
	m_pRenderer = 0;
	m_pEngine = 0;	

	if (IS_VALID_PTR(m_pSkyBox))
		delete m_pSkyBox;

	m_pSkyBox = 0;

	if (IS_VALID_PTR(m_pTerrain))
		delete m_pTerrain;

	m_pTerrain = 0;
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
			if (prefab->nSubsets > 0 && IS_VALID_PTR(prefab->pSubsets))
			{
				for (unsigned int i = 0; i < prefab->nSubsets; ++i)
				{
					pResources->RemoveVertexBuffer(&prefab->pSubsets[i].drawCallDesc.pVertexBuffer);
					pResources->RemoveIndexBuffer(&prefab->pSubsets[i].drawCallDesc.pIndexBuffer);
				}
			}
		}
	}

	m_HelperPrefabs.clear();
}

//////////////////////////////////////////////////////////////////////////////////////////////

S_API void C3DEngine::SetRenderObjectPool(IComponentPool<IRenderObject>* pPool)
{
	assert(IS_VALID_PTR(pPool));
	m_pRenderObjects = pPool;
}

//////////////////////////////////////////////////////////////////////////////////////////////

S_API void C3DEngine::ClearRenderObjects()
{
	if (!IS_VALID_PTR(m_pRenderObjects))
		return;

	// Need to deallocate subset arrays of render descs
	unsigned int iterator;	
	IRenderObject* pRenderObject = m_pRenderObjects->GetFirst(iterator);
	while (pRenderObject)
	{
		pRenderObject->OnRelease();

		/*if (pRenderObject->deallocateRenderDesc)
			delete pRenderObject->pRenderDesc;

		pRenderObject->pRenderDesc = 0;
		pRenderObject->deallocateRenderDesc = false;*/

		pRenderObject = m_pRenderObjects->GetNext(iterator);
	}

	m_pRenderObjects->ReleaseAll();
}


//////////////////////////////////////////////////////////////////////////////////////////////

S_API unsigned int C3DEngine::CollectVisibleObjects(const SCamera* pCamera)
{
	unsigned int budgetTimer = m_pEngine->StartBudgetTimer("C3DEngine::CollectVisiableObjects()");

	// TERRAIN	
	if (IS_VALID_PTR(m_pTerrain) && m_pTerrain->IsInited())
	{
		m_pTerrain->UpdateRenderDesc(&m_TerrainRenderDesc);	
	}


	// SKYBOX
	if (IS_VALID_PTR(m_pSkyBox))
	{
		m_pSkyBox->Update();
	}



	// RENDER OBJECTS

	//TODO: Determine which render objects are visible!


	m_pEngine->StopBudgetTimer(budgetTimer);
	return 0;
}



//////////////////////////////////////////////////////////////////////////////////////////////

S_API IRenderObject* C3DEngine::GetRenderObject()
{
	assert(IS_VALID_PTR(m_pRenderObjects));
	IRenderObject* pRO = m_pRenderObjects->Get();
	pRO->SetRenderer(this);
	return pRO;
}

S_API void C3DEngine::ReleaseRenderObject(IRenderObject** pObject)
{
	assert(IS_VALID_PTR(m_pRenderObjects));
	if (IS_VALID_PTR(*pObject))
		(*pObject)->OnRelease();

	m_pRenderObjects->Release(pObject);
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
	rd->renderPipeline = eRENDER_FORWARD;

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

			if (Failure(dcd->pIndexBuffer->Initialize(m_pRenderer, eIBUSAGE_STATIC, geometry->indices.size(), &geometry->indices[0])))
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
	return (m_HelperPrefabs.find(id) != m_HelperPrefabs.end());
}

S_API void C3DEngine::ClearHelperRenderObjects()
{
	m_HelperPool.Clear();
}



//////////////////////////////////////////////////////////////////////////////////////////////

S_API ITerrain* C3DEngine::CreateTerrain(const STerrainInfo& info)
{
	ClearTerrain();
	m_pTerrain = new Terrain();
	m_pTerrain->Init(m_pRenderer, info);
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

S_API void C3DEngine::RenderCollected()
{
	unsigned int budgetTimer = m_pEngine->StartBudgetTimer("C3DEngine::RenderCollected()");
	{



		//TODO: Render skybox deferred as well!

		if (IS_VALID_PTR(m_pSkyBox))
		{
			unsigned int skyboxTimer = m_pEngine->StartBudgetTimer("C3DEngine::RenderCollected() - Render Skybox");		

			m_pRenderer->BindShaderPass(eSHADERPASS_FORWARD);
			
			SRenderDesc* rd = m_pSkyBox->GetRenderDesc();
			m_pRenderer->Render(*rd);
			
			m_pEngine->StopBudgetTimer(skyboxTimer);
		}



		//TODO: Render terrain deferred as well!

		unsigned int terrainTimer = m_pEngine->StartBudgetTimer("C3DEngine::RenderCollected() - Render terrain");
		{
			m_pRenderer->RenderTerrain(m_TerrainRenderDesc);
			m_pEngine->StopBudgetTimer(terrainTimer);
		}


		if (IS_VALID_PTR(m_pRenderObjects))
		{


			stringstream objectsTimerName;
			objectsTimerName << "C3DEngine::RenderCollected() - Render RenderObjects (" << m_pRenderObjects->GetNumObjects() << ")";

			unsigned int renderObjectsTimer = m_pEngine->StartBudgetTimer(objectsTimerName.str().c_str());
			{






				//TODO: Use GBuffer Pass here to start rendering with the deferred pipeline
				//m_pRenderer->BindShaderPass(eSHADERPASS_GBUFFER);
				m_pRenderer->BindShaderPass(eSHADERPASS_FORWARD);





				unsigned int itRenderObject;
				IRenderObject* pRenderObject = m_pRenderObjects->GetFirst(itRenderObject);
				while (pRenderObject)
				{

#ifdef _DEBUG		
					if (m_pRenderer->DumpingThisFrame())
						CLog::Log(S_DEBUG, "Rendering %s", pRenderObject->_name.c_str());
#endif

					SRenderDesc* rd = pRenderObject->GetRenderDesc();
					m_pRenderer->Render(*rd);

					pRenderObject = m_pRenderObjects->GetNext(itRenderObject);

				}

				m_pEngine->StopBudgetTimer(renderObjectsTimer);
			}


		}


		// Render Helper objects:
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
				m_HelperPool.Release(&pHelper);
			}

			pHelper = m_HelperPool.GetNextUsedObject(itHelper);
		}


		//TODO: Implement deferred shading pass
		/*

		m_pRenderer->BindShaderPass(eSHADERPASS_SHADING);
		foreach (light : lights)
		{
			m_pRenderer->Render(light->pRenderDesc);
		}

		
		*/
		


		m_pEngine->StopBudgetTimer(budgetTimer);
	}
}

SP_NMSPACE_END