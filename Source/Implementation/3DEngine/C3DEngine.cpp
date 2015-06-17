#include <Implementation\3DEngine\C3DEngine.h>
#include <Abstract\Renderable.h>
#include <Abstract\IScene.h>

SP_NMSPACE_BEG

S_API C3DEngine::C3DEngine(IRenderer* pRenderer)
	: m_pRenderer(pRenderer)
{
	m_pRenderObjects = new std::vector<IRenderableObject*>();
}

S_API C3DEngine::~C3DEngine()
{
	delete m_pRenderObjects;
	m_pRenderObjects = 0;

	m_pScene = 0;
	m_pRenderer = 0;
}

S_API void C3DEngine::SetScene(IScene* pScene)
{
	m_pScene = pScene;
}

S_API void C3DEngine::CollectRenderingObjects(const Vec3f& camPos, const Vec3f& camDir)
{	
	m_pRenderObjects->clear();

	std::vector<SSceneNode>* pNodes = m_pScene->GetSceneNodes();
	for (auto itNode = pNodes->begin(); itNode != pNodes->end(); itNode++)
	{
		m_pRenderObjects->push_back(dynamic_cast<IRenderableObject*>(itNode->pObject));
	}
}

S_API void C3DEngine::Unleash()
{
	if (!IS_VALID_PTR(m_pRenderObjects))
		return;

	if (m_pRenderObjects->size() == 0)
		return;

	auto itObj = m_pRenderObjects->begin();
	auto itEnd = m_pRenderObjects->end();
	for (; itObj != itEnd; itObj++)
	{
		IRenderableObject* pObject = *itObj;
		m_pRenderer->Render(pObject->GetRenderable()->Get)
	}
}

SP_NMSPACE_END