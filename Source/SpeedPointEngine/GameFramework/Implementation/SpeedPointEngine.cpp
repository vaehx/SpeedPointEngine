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
#include <GameFramework\Implementation\FileSPW.h>
#include <GameFramework\PhysicalComponent.h>
#include <GameFramework\RenderableComponent.h>
#include <GameFramework\EntityReceipts.h>
#include <EntitySystem\Implementation\EntityReceipt.h>
#include <EntitySystem\Implementation\Scene.h>
#include <Physics\Implementation\CPhysics.h>
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
m_pApplication(nullptr)
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

	CLog::Log(S_INFO, "Initialized 3D Engine");

	// Physics
	m_pPhysics.SetOwn(new CPhysics());

	m_pPhysics->CreatePhysObjectPool<CPhysicalComponent>();

	CLog::Log(S_INFO, "Initialized physics");

	// Scene
	if (IS_VALID_PTR(params.pCustomScene))
		m_pScene.SetCustom(params.pCustomScene);
	else
		m_pScene.SetOwn(new Scene());

	CLog::Log(S_INFO, "Initialized scene");

	return S_SUCCESS;
}

// ----------------------------------------------------------------------------------
#define CHECK_COMPONENT_INITIALIZED(component, componentName) \
	if (!IS_VALID_PTR(component)) \
		return CLog::Log(S_ERROR, "CheckFinishInit() failed: componentName not set");

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
	// Initialize entity receipts
	m_pEntityReceiptManager.SetOwn(new EntityReceiptManager());
	m_pEntityReceiptManager->RegisterReceipt<EntityReceipts::Renderable>();
	m_pEntityReceiptManager->RegisterReceipt<EntityReceipts::Physical>();
	m_pEntityReceiptManager->RegisterReceipt<EntityReceipts::RigidBody>();

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
S_API void SpeedPointEngine::LoadWorld(const string& file)
{
	SSettingsDesc& settings = GetSettings()->Get();

	size_t lastDelim = file.find_last_of('\\');
	if (lastDelim == file.npos)
	{
		m_pResourcePool->SetResourceRootPath("");
		settings.resources.worldFile = file;
	}
	else
	{
		m_pResourcePool->SetResourceRootPath(file.substr(0, lastDelim));
		settings.resources.worldFile = file.substr(lastDelim);
	}

	CLog::Log(S_DEBUG, "rootDir := '%s'", m_pResourcePool->GetResourceRootPath().c_str());

	GetScene()->Clear();
	GetScene()->Initialize();

	Get3DEngine()->ClearRenderMeshes();
	Get3DEngine()->ClearRenderLights();
	Get3DEngine()->ClearTerrain();

	CSPWLoader loader;
	loader.Load(m_p3DEngine, m_pScene, file);
}

// ----------------------------------------------------------------------------------
S_API string SpeedPointEngine::GetShaderDirectoryPath() const
{


	// TODO: Determine a better way here!!
	//		- Possibly by using the resource root path as well?


	return string(PROJ_DIR) + "..\\..\\Effects";
}

// ----------------------------------------------------------------------------------
S_API string SpeedPointEngine::GetResourcePath(const string& file) const
{
	return m_pResourcePool->GetResourcePath(file);
}




// ----------------------------------------------------------------------------------
S_API void SpeedPointEngine::DoFrame()
{
	ProfilingSystemIntrnl* pProfilingSystem = ProfilingSystem::Get();

	// Update
	m_pProfilingDebugView->Update(m_pRenderer);

	if (m_pApplication)
		m_pApplication->Update((float)pProfilingSystem->GetLastFrameDuration());

	m_pPhysics->Update(pProfilingSystem->GetLastFrameDuration());

	// Render
	m_pRenderer->BeginScene();

	const SCamera* camera = m_pRenderer->GetTargetViewport()->GetCamera();
	m_p3DEngine->CollectVisibleObjects(camera);
	m_p3DEngine->RenderCollected();

	m_pRenderer->EndScene();

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

SP_NMSPACE_END
