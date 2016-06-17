#include <Implementation\3DEngine\Terrain.h>
#include <Implementation\3DEngine\CSkyBox.h>
#include <Implementation\3DEngine\C3DEngine.h>
#include <Abstract\IEntity.h>
#include <Abstract\IScene.h>
#include <Abstract\ITerrain.h>
#include <Abstract\IGameEngine.h>
#include <Abstract\ISkyBox.h>
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
}

S_API C3DEngine::~C3DEngine()
{
	Clear();
}

S_API void C3DEngine::Clear()
{
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
		pRenderObject->Clear();

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
	if (IS_VALID_PTR(m_pTerrain))
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
	return m_pRenderObjects->Get();
}

S_API void C3DEngine::ReleaseRenderObject(IRenderObject** pObject)
{
	assert(IS_VALID_PTR(m_pRenderObjects));
	m_pRenderObjects->Release(pObject);
}




//////////////////////////////////////////////////////////////////////////////////////////////

S_API ITerrain* C3DEngine::CreateTerrain(const STerrainInfo& info)
{
	ClearTerrain();
	m_pTerrain = new Terrain();
	return m_pTerrain;
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