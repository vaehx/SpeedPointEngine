#include <Implementation\3DEngine\C3DEngine.h>
#include <Abstract\IObject.h>
#include <Abstract\IScene.h>
#include <Abstract\ITerrain.h>
#include <Abstract\IGameEngine.h>
#include <sstream>

using std::stringstream;

SP_NMSPACE_BEG


S_API C3DEngine::C3DEngine(IRenderer* pRenderer, IGameEngine* pEngine)
	: m_pRenderer(pRenderer),
	m_pEngine(pEngine),
	m_pSkyBoxRenderDesc(0)
{		
}

S_API C3DEngine::~C3DEngine()
{	
	m_RenderObjects.Clear();
	m_pRenderer = 0;
	m_pEngine = 0;
	m_pSkyBoxRenderDesc = 0;
}

//////////////////////////////////////////////////////////////////////////////////////////////

S_API void C3DEngine::ClearRenderObjects()
{
	// Need to deallocate subset arrays of render descs
	unsigned int iterator;	
	SRenderObject* pRenderObject = m_RenderObjects.GetFirstUsedObject(iterator);
	while (pRenderObject)
	{
		if (pRenderObject->deallocateRenderDesc)
			delete pRenderObject->pRenderDesc;

		pRenderObject->pRenderDesc = 0;
		pRenderObject->deallocateRenderDesc = false;
		pRenderObject = m_RenderObjects.GetNextUsedObject(iterator);
	}

	m_RenderObjects.ReleaseAll();
}


//////////////////////////////////////////////////////////////////////////////////////////////

S_API unsigned int C3DEngine::CollectVisibleObjects(IScene* pScene, const SCamera* pCamera)
{
	unsigned int budgetTimer = m_pEngine->StartBudgetTimer("C3DEngine::CollectVisiableObjects()");

	// TERRAIN
	ITerrain* pTerrain = pScene->GetTerrain();
	if (IS_VALID_PTR(pTerrain))
	{
		pTerrain->UpdateRenderDesc(&m_TerrainRenderDesc);	
	}


	// SKYBOX
	ISkyBox* pSkyBox = pScene->GetSkyBox();
	if (IS_VALID_PTR(pSkyBox))
	{
		m_pSkyBoxRenderDesc = pSkyBox->GetUpdatedRenderDesc();
	}



	// SCENE NODES
	vector<SSceneNode>* pSceneNodes = pScene->GetSceneNodes();
	if (!IS_VALID_PTR(pSceneNodes))
	{
		m_pEngine->StopBudgetTimer(budgetTimer);
		return 0;
	}
	
	unsigned int nVisibles = 0;
	for (auto itSceneNode = pSceneNodes->begin(); itSceneNode != pSceneNodes->end(); itSceneNode++)
	{
		bool bSceneNodeVisible = true; // TODO

		// Increase visibles counter
		if (bSceneNodeVisible)
		{
			if (itSceneNode->type != eSCENENODE_LIGHT)		
				nVisibles++;		
		}

		// Add the object
		switch (itSceneNode->type)
		{
		case eSCENENODE_STATIC:
			if (bSceneNodeVisible)				
				AddVisibleStatic(itSceneNode->pStatic, itSceneNode->aabb);
			break;
		case eSCENENODE_ENTITY:
			if (bSceneNodeVisible)
				AddVisibleEntity(itSceneNode->pObject, itSceneNode->aabb);
			break;
		case eSCENENODE_LIGHT:
			AddVisibleLight(itSceneNode->pLight, itSceneNode->aabb);
			break;
		default:
			break;
		}		
	}

	m_pEngine->StopBudgetTimer(budgetTimer);
	return 0;
}


//////////////////////////////////////////////////////////////////////////////////////////////

S_API void C3DEngine::AddVisibleEntity(IEntity* pEntity, const AABB& aabb)
{
	if (!pEntity->IsRenderable())
	{
		return;
	}

	IRenderableComponent* pRenderable = pEntity->GetRenderable();
	if (!IS_VALID_PTR(pRenderable))
	{
		// todo: print error message?
		return;
	}

	SRenderDesc* pRenderDesc = pRenderable->GetUpdatedRenderDesc();
	if (!IS_VALID_PTR(pRenderDesc))
		return; // invalid render desc - will not be added to the RenderObjects

	SRenderObject* pRenderObject = m_RenderObjects.Get();
	pRenderObject->pRenderDesc = pRenderDesc;
	pRenderObject->aabb = aabb;
#ifdef _DEBUG
	pRenderObject->name = pEntity->GetName();
#endif
}

S_API void C3DEngine::AddVisibleLight(ILight* pLight, const AABB& aabb)
{
	// Add light for deferred light rendering
	SRenderLight* pRenderLight = m_RenderLights.Get();
	pLight->GetLightDesc(&pRenderLight->lightDesc);

	// Check found statics and entities if they are lit by this light - ONLY FOR FORWARD RENDERING
	unsigned int iterator;
	SRenderObject* pRenderObject = m_RenderObjects.GetFirstUsedObject(iterator);
	while (pRenderObject)
	{
		if (pRenderObject->pRenderDesc->renderPipeline != eRENDER_FORWARD || pRenderObject->nAffectingLights >= 4)
		{
			pRenderObject = m_RenderObjects.GetNextUsedObject(iterator);
			continue;
		}


		// TODO: Check if storing pointers to the lights for deferred rendering is safe here, or if we rather
		//		should create separate RenderLights for forward rendering.
		pRenderObject->affectingLights[pRenderObject->nAffectingLights] = pRenderLight;


		++pRenderObject->nAffectingLights;

		pRenderObject = m_RenderObjects.GetNextUsedObject(iterator);
	}
}

S_API void C3DEngine::AddVisibleStatic(IStaticObject* pStatic, const AABB& aabb)
{	
	SRenderDesc* pRenderDesc = pStatic->GetRenderDesc();
	if (!IS_VALID_PTR(pRenderDesc))
		return; // invalid render desc, do not add to the render objects

	SRenderObject* pRenderObject = m_RenderObjects.Get();	
	pRenderObject->pRenderDesc = pRenderDesc;
	pRenderObject->aabb = aabb;
#ifdef _DEBUG
	pRenderObject->name = pStatic->GetName();
#endif

	pRenderObject->nAffectingLights = 0;
}




//////////////////////////////////////////////////////////////////////////////////////////////

S_API SRenderObject* C3DEngine::GetCustomRenderObject()
{
	return m_RenderObjects.Get();
}











//////////////////////////////////////////////////////////////////////////////////////////////

S_API void C3DEngine::RenderCollected()
{









	//TODO:   IMPLEMENT NEW SHADER PASS BASED RENDERING HERE !










	unsigned int budgetTimer = m_pEngine->StartBudgetTimer("C3DEngine::RenderCollected()");
	{

		if (IS_VALID_PTR(m_pSkyBoxRenderDesc))
		{
			unsigned int skyboxTimer = m_pEngine->StartBudgetTimer("C3DEngine::RenderCollected() - Render Skybox");		
						
			m_pRenderer->Render(*m_pSkyBoxRenderDesc);
			
			m_pEngine->StopBudgetTimer(skyboxTimer);		
		}

		unsigned int terrainTimer = m_pEngine->StartBudgetTimer("C3DEngine::RenderCollected() - Render terrain");
		{
			m_pRenderer->RenderTerrain(m_TerrainRenderDesc);
			m_pEngine->StopBudgetTimer(terrainTimer);
		}


		stringstream objectsTimerName;
		objectsTimerName << "C3DEngine::RenderCollected() - Render RenderObjects (" << m_RenderObjects.GetUsedObjectCount() << ")";

		unsigned int renderObjectsTimer = m_pEngine->StartBudgetTimer(objectsTimerName.str().c_str());
		{
			unsigned int itRenderObject;
			SRenderObject* pRenderObject = m_RenderObjects.GetFirstUsedObject(itRenderObject);
			while (pRenderObject)
			{

#ifdef _DEBUG		
				if (m_pRenderer->DumpingThisFrame())
					CLog::Log(S_DEBUG, "Rendering %s", pRenderObject->name.c_str());
#endif

				m_pRenderer->Render(*pRenderObject->pRenderDesc);

				pRenderObject = m_RenderObjects.GetNextUsedObject(itRenderObject);

			}

			m_pEngine->StopBudgetTimer(renderObjectsTimer);
		}

		m_pEngine->StopBudgetTimer(budgetTimer);
	}
}

SP_NMSPACE_END