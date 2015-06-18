#include <Implementation\3DEngine\C3DEngine.h>
#include <Abstract\Renderable.h>
#include <Abstract\IScene.h>
#include <Abstract\ITerrain.h>

SP_NMSPACE_BEG


S_API C3DEngine::C3DEngine(IRenderer* pRenderer)
	: m_pRenderer(pRenderer)
{	
}

S_API C3DEngine::~C3DEngine()
{
	m_RenderDescs.clear();
}

S_API unsigned int C3DEngine::CollectVisibleObjects(IScene* pScene, const SCamera* pCamera)
{
	// Checking if render Desc is already in the list is probably not faster, because
	// Objects might change or the camera is moving eagerly. Also, CollectVisibleObjects() should only
	// be called if the update is necessary.
	m_RenderDescs.clear();

	ITerrain* pTerrain = pScene->GetTerrain();
	pTerrain->UpdateRenderDesc(&m_TerrainRenderDesc);

	vector<SSceneNode>* pSceneNodes = pScene->GetSceneNodes();
	if (!IS_VALID_PTR(pSceneNodes))
	{
		return 0;
	}

	unsigned int numPushed = 0;
	for (auto itSceneNode = pSceneNodes->begin(); itSceneNode != pSceneNodes->end(); itSceneNode++)
	{
		if (!IS_VALID_PTR(itSceneNode->pObject) || !itSceneNode->pObject->IsRenderable())
		{
			continue;
		}



		// TODO: Handle view frustum culling here.		
		// Test intersection of itSceneNode->aabb against the camera view frustum.

		bool bSceneNodeVisible = true;
		if (!bSceneNodeVisible)
		{
			continue;
		}
		
		IRenderableObject* pRenderable = dynamic_cast<IRenderableObject*>(itSceneNode->pObject);
		if (!IS_VALID_PTR(pRenderable))
		{
			continue;
		}

		SRenderDesc* pRenderDesc = pRenderable->GetUpdatedRenderDesc();
		if (!IS_VALID_PTR(pRenderDesc))
		{
			continue;
		}
		
		m_RenderDescs.push_back(pRenderDesc);
		++numPushed;
	}

	return numPushed;
}

S_API void C3DEngine::RenderCollected()
{
	if (m_RenderDescs.empty())
	{
		return;
	}

	m_pRenderer->RenderTerrain(m_TerrainRenderDesc);
	
	for (auto itRS = m_RenderDescs.begin(); itRS != m_RenderDescs.end(); itRS++)
	{
		SRenderDesc* pRS = *itRS;
		m_pRenderer->Render(*pRS);
	}
}

SP_NMSPACE_END