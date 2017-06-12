//////////////////////////////////////////////////////////////////////////////////////////////////
//
//	SpeedPoint Game Engine
//	Copyright (c) 2011-2017 Pascal Rosenkranz, All rights reserved.
//
//////////////////////////////////////////////////////////////////////////////////////////////////

#include "SpeedPointEngine.h"
#include "EngineSettings.h"
#include "..\IApplication.h"
#include "FileLogListener.h"
#include "ProfilingDebugView.h"
#include "PhysicsDebugRenderer.h"
#include <GameFramework\Implementation\FileSPW.h>
#include <GameFramework\PhysicalComponent.h>
#include <GameFramework\RenderableComponent.h>
#include <GameFramework\EntityClasses.h>
#include <EntitySystem\Implementation\EntityClass.h>
#include <EntitySystem\Implementation\Scene.h>
#include <EntitySystem\Implementation\EntitySystem.h>
#include <Physics\Implementation\CPhysics.h>
#include <Physics\Implementation\PhysDebug.h>
#include <3DEngine\Implementation\Material.h>
#include <3DEngine\Implementation\C3DEngine.h>
#include <Renderer\IRenderer.h>
#include <Renderer\IResourcePool.h>
#include <Renderer\DirectX11\DX11Renderer.h>
#include <Common\ProfilingSystem.h>
#include <Common\SAssert_Impl.h>
#include <Common\IShutdownHandler.h>
#include <fstream>

SP_NMSPACE_BEG

IGameEngine* SpeedPointEnv::m_pEngine = 0;

// ----------------------------------------------------------------------------------
S_API SpeedPointEngine::SpeedPointEngine()
: m_bRunning(false),
m_bLoggedSkipstages(false),
m_pApplication(nullptr),
m_pPhysicsDebugRenderer(0),
m_FramesSinceGC(0)
{
	SpeedPointEnv::SetEngine(this);

	m_pProfilingDebugView = new ProfilingDebugView();

	m_pSettings = new EngineSettings();
	m_pSettings->Initialize(this);
}

// ----------------------------------------------------------------------------------
S_API SpeedPointEngine::~SpeedPointEngine()
{
	Shutdown();
	SpeedPointEnv::SetEngine(0);
}


//////////////////////////////////////////////////////////////////////////////////////////////////
//
//	Initialization
//
//////////////////////////////////////////////////////////////////////////////////////////////////

// ----------------------------------------------------------------------------------
S_API SResult SpeedPointEngine::Initialize(const SGameEngineInitParams& params)
{
	m_pFileLogListener.SetOwn(new FileLogListener(params.logFilename));
	CLog::RegisterListener(m_pFileLogListener);

	// Renderer
	switch (params.rendererImplementation)
	{
	case S_DIRECTX11:
		m_pRenderer.SetOwn(new DX11Renderer());
		break;
	default:
		return CLog::Log(S_ERROR, "Given rendererImplementation not supported (anymore).");
	}

	if (Failure(m_pRenderer->Initialize(params.rendererParams)))
		return CLog::Log(S_ERROR, "Failed initialize renderer (%s)", GetRendererTypeName(m_pRenderer->GetType()));

	m_pResourcePool.SetCustom(m_pRenderer->GetResourcePool());

	char execPath[200];
	GetModuleFileName(NULL, execPath, 200);
	m_pResourcePool->SetResourceRootPath(execPath);

	CLog::Log(S_INFO, "Initialized renderer (%s)", GetRendererTypeName(m_pRenderer->GetType()));

	// 3D Engine
	m_p3DEngine.SetOwn(new C3DEngine(m_pRenderer));

	m_p3DEngine->SetRenderMeshImplementation<CRenderMeshComponent>();
	m_p3DEngine->SetRenderLightImplementation<CRenderLightComponent>();
	m_p3DEngine->GetParticleSystem()->SetEmitterImplementation<CParticleEmitterComponent>();

	m_p3DEngine->Init();

	CLog::Log(S_INFO, "Initialized 3D Engine");

	// Physics
	geo::FillIntersectionTestTable();
	
	m_pPhysics.SetOwn(new CPhysics());
	m_pPhysics->CreatePhysObjectPool<CPhysicalComponent>();

	m_pPhysicsDebugRenderer = new PhysicsDebugRenderer();
	PhysDebug::SetRenderer(m_pPhysicsDebugRenderer);

	CLog::Log(S_INFO, "Initialized physics");

	// Scene / Entity System
	if (IS_VALID_PTR(params.pCustomScene))
		m_pScene.SetCustom(params.pCustomScene);
	else
		m_pScene.SetOwn(new Scene());

	m_pEntitySystem.SetOwn(new EntitySystem());

	CLog::Log(S_INFO, "Initialized entity system");

	// Debug
	m_pProfilingDebugView->Show(params.showDebugInfo);

	return S_SUCCESS;
}

// ----------------------------------------------------------------------------------
#define CHECK_COMPONENT_INITIALIZED(component, componentName) \
	if (!IS_VALID_PTR(component)) \
		return CLog::Log(S_ERROR, "CheckFinishInit() failed: " componentName " not set");

S_API SResult SpeedPointEngine::CheckFinishInit()
{
	CHECK_COMPONENT_INITIALIZED(m_pApplication, "Application");
	CHECK_COMPONENT_INITIALIZED(m_pRenderer.pComponent, "Renderer");
	CHECK_COMPONENT_INITIALIZED(m_pResourcePool.pComponent, "Resource pool");
	CHECK_COMPONENT_INITIALIZED(m_pPhysics.pComponent, "Physics");

	return S_SUCCESS;
}

// ----------------------------------------------------------------------------------
S_API SResult SpeedPointEngine::FinishInitialization()
{
	// Register entity components
	m_pEntitySystem->RegisterComponent<CRenderMeshComponent>();
	m_pEntitySystem->RegisterComponent<CRenderLightComponent>();
	m_pEntitySystem->RegisterComponent<CParticleEmitterComponent>();
	m_pEntitySystem->RegisterComponent<CPhysicalComponent>();
	
	// Initialize entity receipts
	m_pEntityClassManager.SetOwn(new EntityClassManager());
	m_pEntityClassManager->RegisterEntityClass<EntityClasses::Renderable>();
	m_pEntityClassManager->RegisterEntityClass<EntityClasses::Physical>();
	m_pEntityClassManager->RegisterEntityClass<EntityClasses::RigidBody>();

	// Check that all components where initialized
	if (Failure(CheckFinishInit()))
		return S_ERROR;

	// Initialize application
	m_pApplication->OnInit(this);

	return S_SUCCESS;
}

// ----------------------------------------------------------------------------------
S_API void SpeedPointEngine::Shutdown(void)
{
	m_bRunning = false;

	m_pApplication = 0;

	// Handle all shutdown handlers first	
	vector<IShutdownHandler*>* pShutdownHandlers = ShutdownManager::GetHandlers();
	vector<IShutdownHandler*> handledShutdownHandlers;
	unsigned int nHandled = pShutdownHandlers->size();
	for (auto itSH = pShutdownHandlers->begin(); itSH != pShutdownHandlers->end();)
	{
		IShutdownHandler* pSH = *itSH;
		if (!pSH)
		{
			++itSH;
			continue;
		}

		bool alreadyHandled = false;
		for (auto itHandledSH = handledShutdownHandlers.begin(); itHandledSH != handledShutdownHandlers.end(); ++itHandledSH)
		{
			if (*itHandledSH == *itSH)
			{
				alreadyHandled = true;
				break;
			}
		}

		if (alreadyHandled)
		{
			++itSH;
			continue;
		}

		// Erase first, then handle shutdown.
		// Otherwise, the object will call UnregisterShutdownHandler(), which will alter the vector of shutdown handlers..
		itSH = pShutdownHandlers->erase(itSH);

		if (Failure(pSH->HandleShutdown()))
			CLog::Log(S_ERROR, "Shutdown-Handler failed: " + pSH->GetShutdownHandlerDesc() + "!");

		handledShutdownHandlers.push_back(pSH);
	}

	if (nHandled > 0)
		CLog::Log(S_WARN, "Cleared %d objects via shutdown handler!", nHandled);

	pShutdownHandlers->clear();

	// clear scene
	if (IS_VALID_PTR(m_pScene.pComponent))
		m_pScene->Clear();

	if (m_pPhysicsDebugRenderer) delete m_pPhysicsDebugRenderer;
	m_pPhysicsDebugRenderer = 0;
	PhysDebug::SetRenderer(0);
	m_pPhysics.Clear();

	// has to be called before clearing renderer (RenderAPI)
	m_p3DEngine.Clear();

	// calls IRenderer::~IRenderer implementation which will destruct the resource pool	
	m_pRenderer.Clear();

	if (m_pSettings)
	{
		delete m_pSettings;
		m_pSettings = nullptr;
	}

	delete m_pProfilingDebugView;
	m_pProfilingDebugView = 0;

	CLog::Log(S_INFO, "Engine shut down");

	if (m_pFileLogListener)
	{
		CLog::UnregisterListener(m_pFileLogListener);
		m_pFileLogListener->ReleaseQueue();
		m_pFileLogListener->Clear();
		m_pFileLogListener.Clear();
	}
}

// ----------------------------------------------------------------------------------

// How does this algorithm work????
S_API std::vector<IShutdownHandler*>::iterator SpeedPointEngine::UnregisterShutdownHandlerIntrnl(
	IShutdownHandler* pShutdownHandler, std::vector<IShutdownHandler*>::iterator cur)
{
	vector<IShutdownHandler*>* handlers = ShutdownManager::GetHandlers();
	if (handlers->empty())
		return handlers->end();

	int iSHFirstPrev = -1;
	unsigned int iSHFirstPrevCnt = 0;
	for (auto itShutdownHandler = handlers->begin(); itShutdownHandler != handlers->end();)
	{
		if (*itShutdownHandler == pShutdownHandler)
		{
			itShutdownHandler = handlers->erase(itShutdownHandler);
			if (handlers->size() == 0)
				return handlers->end();

			if (iSHFirstPrev < 0)
				iSHFirstPrev = iSHFirstPrevCnt;
		}
		else
		{
			itShutdownHandler++;
			if (iSHFirstPrev < 0)
				iSHFirstPrevCnt++;
		}
	}

	// not found
	if (iSHFirstPrev < 0)
		return (cur != handlers->end() ? cur++ : cur);

	if (iSHFirstPrev == handlers->size())
		return handlers->end();

	auto itFirstPrev = handlers->begin() + iSHFirstPrev;
	if (itFirstPrev != handlers->end())
		return itFirstPrev++;
	else
		return handlers->end();
}





//////////////////////////////////////////////////////////////////////////////////////////////////
//
//	Game / Application Interface
//
//////////////////////////////////////////////////////////////////////////////////////////////////

// ----------------------------------------------------------------------------------
S_API IMaterialManager* SpeedPointEngine::GetMaterialManager() const
{
	return m_p3DEngine->GetMaterialManager();
}

// ----------------------------------------------------------------------------------
S_API void SpeedPointEngine::RegisterApplication(IApplication* pApp)
{
	assert(IS_VALID_PTR(pApp));
	m_pApplication = pApp;
}

// ----------------------------------------------------------------------------------
S_API SResult SpeedPointEngine::LoadWorld(const string& absResourcePath)
{
	if (absResourcePath.empty())
		return CLog::Log(S_ERROR, "Failed load world: Empty path given");

	CLog::Log(S_DEBUG, "Attempting to load world '%s'...", absResourcePath.c_str());

	GetScene()->Clear();
	GetScene()->Initialize();

	Get3DEngine()->ClearRenderMeshes();
	Get3DEngine()->ClearRenderLights();
	Get3DEngine()->ClearTerrain();

	CSPWLoader loader;
	loader.Load(m_p3DEngine, m_pScene, absResourcePath);
	
	CLog::Log(S_INFO, "Loaded world '%s'", absResourcePath.c_str());
	return S_SUCCESS;
}

// ----------------------------------------------------------------------------------
S_API string SpeedPointEngine::GetShaderDirectoryPath() const
{


	// TODO: Determine a better way here!!
	//		- Possibly by using the resource root path as well?


	return string(PROJ_DIR) + "..\\..\\Effects";
}

// ----------------------------------------------------------------------------------
S_API string SpeedPointEngine::GetResourceSystemPath(const string& absResourcePath) const
{
	return m_pResourcePool->GetResourceSystemPath(absResourcePath);
}




// ----------------------------------------------------------------------------------
S_API void SpeedPointEngine::DoFrame()
{
	ProfilingSystemIntrnl* pProfilingSystem = ProfilingSystem::Get();
	float fTime = (float)pProfilingSystem->GetLastFrameDuration();

	// Update
	m_pProfilingDebugView->Update(m_pRenderer);

	if (m_pApplication)
		m_pApplication->Update(fTime);

	m_pPhysics->Update(fTime);

	// Render
	m_pRenderer->BeginScene();

	const SCamera* camera = m_pRenderer->GetTargetViewport()->GetCamera();
	m_p3DEngine->CollectVisibleObjects(camera);
	m_p3DEngine->RenderCollected();

	m_pRenderer->EndScene();

	// Garbage collection
	m_FramesSinceGC++;
	if (m_FramesSinceGC >= 10)
	{
		m_FramesSinceGC = 0;
		m_pRenderer->GetResourcePool()->RunGarbageCollection();
		m_p3DEngine->GetGeometryManager()->GarbageCollect();
	}

	// Release File Log Queue
	if (m_pFileLogListener)
	{
		unsigned int logReleaseSection = ProfilingSystem::StartSection("FileLogListener::ReleaseQueue()");
		m_pFileLogListener->ReleaseQueue();
		ProfilingSystem::EndSection(logReleaseSection);
	}

	// Notify profiling system about next frame
	pProfilingSystem->NextFrame();
}



// ----------------------------------------------------------------------------------
S_API void SpeedPointEngine::ShowDebugInfo(bool show)
{
	if (m_pProfilingDebugView)
		m_pProfilingDebugView->Show(show);
}

S_API bool SpeedPointEngine::IsDebugInfoShown() const
{
	if (m_pProfilingDebugView)
		return m_pProfilingDebugView->IsShown();
	else
		return false;
}

SP_NMSPACE_END
