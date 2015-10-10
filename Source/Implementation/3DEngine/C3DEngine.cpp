#include <Implementation\3DEngine\C3DEngine.h>
#include <Abstract\IObject.h>
#include <Abstract\IScene.h>
#include <Abstract\ITerrain.h>

SP_NMSPACE_BEG


S_API C3DEngine::C3DEngine(IRenderer* pRenderer)
	: m_pRenderer(pRenderer)
{		
}

S_API C3DEngine::~C3DEngine()
{	
	m_RenderObjects.Clear();
}

//////////////////////////////////////////////////////////////////////////////////////////////

S_API void C3DEngine::ClearRenderObjects()
{
	// Need to deallocate subset arrays of render descs
	unsigned int iterator = 0;
	SRenderObject* pRenderObject = 0;
	while (pRenderObject = m_RenderObjects.GetNextUsedObject(iterator))
	{
		if (IS_VALID_PTR(pRenderObject->renderDesc.pSubsets))
			delete[] pRenderObject->renderDesc.pSubsets;
	}

	m_RenderObjects.Clear();
}


//////////////////////////////////////////////////////////////////////////////////////////////

S_API unsigned int C3DEngine::CollectVisibleObjects(IScene* pScene, const SCamera* pCamera)
{
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
		pSkyBox->GetUpdatedRenderDesc(&m_SkyBoxRenderDesc);
	}



	// SCENE NODES
	vector<SSceneNode>* pSceneNodes = pScene->GetSceneNodes();
	if (!IS_VALID_PTR(pSceneNodes))
	{
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

	SRenderObject* pRenderObject = m_RenderObjects.Get();

	pRenderable->GetUpdatedRenderDesc(&pRenderObject->renderDesc);
	pRenderObject->aabb = aabb;
	pRenderObject->name = pEntity->GetName();
}

S_API void C3DEngine::AddVisibleLight(ILight* pLight, const AABB& aabb)
{
	// Add light for deferred light rendering
	SRenderLight* pRenderLight = m_RenderLights.Get();
	pLight->GetLightDesc(&pRenderLight->lightDesc);

	// Check found statics and entities if they are lit by this light - ONLY FOR FORWARD RENDERING
	unsigned int iterator = 0;
	SRenderObject* pRenderObject = 0;
	while (pRenderObject = m_RenderObjects.GetNextUsedObject(iterator))
	{
		if (pRenderObject->renderDesc.renderPipeline != eRENDER_FORWARD || pRenderObject->nAffectingLights >= 4)
			continue;


		// TODO: Check if storing pointers to the lights for deferred rendering is safe here, or if we rather
		//		should create separate RenderLights for forward rendering.
		pRenderObject->affectingLights[pRenderObject->nAffectingLights] = pRenderLight;


		++pRenderObject->nAffectingLights;
	}
}

S_API void C3DEngine::AddVisibleStatic(IStaticObject* pStatic, const AABB& aabb)
{	
	SRenderObject* pRenderObject = m_RenderObjects.Get();

	const SRenderDesc* pStaticRenderDesc = pStatic->GetRenderDesc();	

	pRenderObject->renderDesc.Copy(*pStaticRenderDesc);
	pRenderObject->aabb = aabb;

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
	m_pRenderer->Render(m_SkyBoxRenderDesc);
	m_pRenderer->RenderTerrain(m_TerrainRenderDesc);

	unsigned int itRenderObject = 0;
	SRenderObject* pRenderObject = 0;
	while (pRenderObject = m_RenderObjects.GetNextUsedObject(itRenderObject))
	{

#ifdef _DEBUG		
		if (m_pRenderer->DumpingThisFrame())
			CLog::Log(S_DEBUG, "Rendering %s", pRenderObject->name.c_str());
#endif

		m_pRenderer->Render(pRenderObject->renderDesc);

	}	
}

SP_NMSPACE_END