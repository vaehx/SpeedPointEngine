//////////////////////////////////////////////////////////////////////////////////////////////////
//
//	SpeedPoint Game Engine
//	Copyright (c) 2011-2017 Pascal Rosenkranz, All rights reserved.
//
//////////////////////////////////////////////////////////////////////////////////////////////////

#include "SpeedPointEngine.h"
#include "EngineSettings.h"
#include <GameFacilities\FileSPW.h>
#include <GameFacilities\EntityReceipt.h>
#include <3DEngine\Material.h>
#include <3DEngine\C3DEngine.h>
#include <Physics\CPhysics.h>
#include <Abstract\SAssert_Impl.h>
#include <Abstract\IRenderer.h>
#include <Abstract\IRenderPipeline.h>
#include <Abstract\IApplication.h>
#include <Abstract\IScene.h>
#include "Pipelines\FramePipeline.h"
#include "Pipelines\RenderPipeline.h"
#include "Util\FileLogListener.h"
#include <fstream>

SP_NMSPACE_BEG

IGameEngine* SpeedPointEnv::m_pEngine;

// ----------------------------------------------------------------------------------
S_API SpeedPointEngine::SpeedPointEngine(const char* logFileName)
: m_bRunning(false),
m_bLoggedSkipstages(false),
m_pApplication(nullptr)
{
	SpeedPointEnv::SetEngine(this);

	m_pFileLogListener.SetOwn(new FileLogListener(logFileName));
	CLog::RegisterListener(m_pFileLogListener);

	m_pSettings = new EngineSettings();
	m_pSettings->Initialize(this);
	
	char execPath[200];
	GetModuleFileName(NULL, execPath, 200);

	m_pSettings->Get().resources.rootDir = execPath;
}

// ----------------------------------------------------------------------------------
S_API SpeedPointEngine::~SpeedPointEngine()
{
	Shutdown();
	SpeedPointEnv::SetEngine(0);
}

// ----------------------------------------------------------------------------------
S_API void SpeedPointEngine::Shutdown(void)
{
	m_bRunning = false;

	m_pApplication = 0;

	// Handle all shutdown handlers first	
	unsigned int nHandled = m_ShutdownHandlers.size();
	for (auto itShutdownHandler = m_ShutdownHandlers.begin(); itShutdownHandler != m_ShutdownHandlers.end();)
	{
		IShutdownHandler* pSH = *itShutdownHandler;

		// definetly increments, no matter if pSH was found or not.
		itShutdownHandler = UnregisterShutdownHandlerIntrnl(pSH, itShutdownHandler);

		if (Failure(pSH->HandleShutdown()))
			CLog::Log(S_ERROR, "Shutdown-Handler failed: " + (*itShutdownHandler)->GetShutdownHandlerDesc() + "!");
	}
	
	if (nHandled > 0)
		CLog::Log(S_WARN, "Cleared %d objects via shutdown handler!", nHandled);

	m_ShutdownHandlers.clear();

	// clear scene
	if (IS_VALID_PTR(m_pScene.pComponent))
		m_pScene->Clear();
	
	m_pFontRenderer.Clear();

	m_pMaterialManager.Clear();

	m_pPhysics.Clear();

	// has to be called before clearing renderer (RenderAPI)
	m_p3DEngine.Clear();

	// calls IRenderer::~IRenderer implementation which will destruct the resource pool	
	m_pRenderer.Clear();

	m_pFramePipeline.Clear();	
	
	if (m_pSettings)
	{
		delete m_pSettings;
		m_pSettings = nullptr;
	}

	CLog::Log(S_INFO, "Engine shut down");

	if (m_pFileLogListener)
	{
		m_pFileLogListener->ReleaseQueue();
		m_pFileLogListener->Clear();
		m_pFileLogListener.Clear();
	}
}

// ----------------------------------------------------------------------------------
S_API void SpeedPointEngine::RegisterShutdownHandler(IShutdownHandler* pShutdownHandler)
{
	// no search for probably already existing handler. Probability that this is the case is very low
	// and a multiple call of Clear() should not be the problem.
	m_ShutdownHandlers.push_back(pShutdownHandler);
}

// ----------------------------------------------------------------------------------
S_API std::vector<IShutdownHandler*>::iterator SpeedPointEngine::UnregisterShutdownHandlerIntrnl(
	IShutdownHandler* pShutdownHandler, std::vector<IShutdownHandler*>::iterator cur)
{
	if (m_ShutdownHandlers.empty())
		return m_ShutdownHandlers.end();

	int iSHFirstPrev = -1;
	unsigned int iSHFirstPrevCnt = 0;
	for (auto itShutdownHandler = m_ShutdownHandlers.begin(); itShutdownHandler != m_ShutdownHandlers.end();)
	{
		if (*itShutdownHandler == pShutdownHandler)
		{
			itShutdownHandler = m_ShutdownHandlers.erase(itShutdownHandler);
			if (m_ShutdownHandlers.size() == 0)
				return m_ShutdownHandlers.end();

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
		return (cur != m_ShutdownHandlers.end() ? cur++ : cur);

	if (iSHFirstPrev == m_ShutdownHandlers.size())
		return m_ShutdownHandlers.end();	

	auto itFirstPrev = m_ShutdownHandlers.begin() + iSHFirstPrev;
	if (itFirstPrev != m_ShutdownHandlers.end())
		return itFirstPrev++;
	else
		return m_ShutdownHandlers.end();
}

// ----------------------------------------------------------------------------------
S_API void SpeedPointEngine::UnregisterShutdownHandler(IShutdownHandler* pShutdownHandler)
{	
	if (m_ShutdownHandlers.empty())
		return;

	for (auto itShutdownHandler = m_ShutdownHandlers.begin(); itShutdownHandler != m_ShutdownHandlers.end();)
	{
		if (*itShutdownHandler == pShutdownHandler)
			itShutdownHandler = m_ShutdownHandlers.erase(itShutdownHandler);
		else
			itShutdownHandler++;
	}
}

// ----------------------------------------------------------------------------------
S_API void SpeedPointEngine::RegisterApplication(IApplication* pApp)
{
	assert(IS_VALID_PTR(pApp));
	m_pApplication = pApp;
}

// ----------------------------------------------------------------------------------
S_API void SpeedPointEngine::CheckFinishInit()
{
	if (!IS_VALID_PTR(m_pApplication))
		return;

	if (IS_VALID_PTR(m_pFramePipeline.pComponent)
		&& IS_VALID_PTR(m_pRenderer.pComponent)
		&& IS_VALID_PTR(m_pResourcePool.pComponent))
	{		
		m_pApplication->OnInit(this);

		if (!IS_VALID_PTR(m_pFontRenderer.pComponent))
			CLog::Log(S_ERROR, "Font Renderer is not initialized!");
	}
}

// ----------------------------------------------------------------------------------
S_API SResult SpeedPointEngine::FinishInitialization()
{
	// Initialize entity receipts
	m_pEntityReceiptManager.SetOwn(new EntityReceiptManager());
	m_pEntityReceiptManager->RegisterReceipt<EntityReceipts::Renderable>();
	m_pEntityReceiptManager->RegisterReceipt<EntityReceipts::Physical>();
	m_pEntityReceiptManager->RegisterReceipt<EntityReceipts::RigidBody>();

	// Register the framepipeline sections
	if (!IS_VALID_PTR(m_pRenderer.pComponent)) return S_ERROR;
	
	// Hook render pipeline
	IRenderPipeline* pRenderPipeline = new RenderPipeline();
	pRenderPipeline->Init(this);
	m_pFramePipeline->RegisterSection(pRenderPipeline);

	CheckFinishInit();

	return S_SUCCESS;
}

// ----------------------------------------------------------------------------------
S_API SResult SpeedPointEngine::InitializeFramePipeline(IFramePipeline* pCustomFramePipeline /* = nullptr */)
{
	if (IS_VALID_PTR(pCustomFramePipeline))
		m_pFramePipeline.SetCustom(pCustomFramePipeline);
	else
		m_pFramePipeline.SetOwn(new FramePipeline());

	m_pFramePipeline->Initialize(this);

	CLog::Log(S_INFO, "Initialized Frame pipeline.");

	return S_SUCCESS;
}


// ----------------------------------------------------------------------------------
S_API SResult SpeedPointEngine::InitializeRenderer(const S_RENDERER_TYPE& type, IRenderer* pRenderer, bool bManageDealloc /* = true */)
{
	assert(IS_VALID_PTR(pRenderer));
	if (bManageDealloc)
		m_pRenderer.SetOwn(pRenderer);
	else
		m_pRenderer.SetCustom(pRenderer);


	// Initialize the renderer
	if (Failure(m_pRenderer->Initialize(this, false)))
	{
		return S_ERROR;
	}


	CLog::Log(S_INFO, "Initialized Renderer.");

	return S_SUCCESS;
}

// ----------------------------------------------------------------------------------
S_API SResult SpeedPointEngine::Initialize3DEngine(I3DEngine* p3DEngine, bool bManageDealloc /*=true*/)
{
	if (!IS_VALID_PTR(m_pRenderer.pComponent))
	{
		return CLog::Log(S_ERROR, "Failed Initialize 3DEngine: Renderer must be initialized first!");
	}

	if (bManageDealloc)
		m_p3DEngine.SetOwn(p3DEngine);
	else
		m_p3DEngine.SetCustom(p3DEngine);

	CLog::Log(S_INFO, "Initialized 3DEngine.");
	return S_SUCCESS;
}

// ----------------------------------------------------------------------------------
S_API SResult SpeedPointEngine::InitializeFontRenderer()
{
	IRenderer* pRenderer = (IRenderer*)m_pRenderer;
	if (!IS_VALID_PTR(pRenderer))
		return S_NOTINIT;

	IFontRenderer* pFontRenderer = pRenderer->InitFontRenderer();
	if (!IS_VALID_PTR(pFontRenderer))
		return S_ERROR;

	m_pFontRenderer.SetOwn(pFontRenderer);

	CLog::Log(S_INFO, "Initialized Font Renderer");
	return S_SUCCESS;
}

// ----------------------------------------------------------------------------------
S_API SResult SpeedPointEngine::InitializeResourcePool(IMaterialManager* pMatMgr, bool bManageDealloc /*=true*/)
{
	assert(IS_VALID_PTR(m_pRenderer.pComponent));
	m_pResourcePool.SetCustom(m_pRenderer->GetResourcePool());

	CLog::Log(S_INFO, "Initialized Resource pool!");

	if (bManageDealloc)
		m_pMaterialManager.SetOwn(pMatMgr);
	else
		m_pMaterialManager.SetCustom(pMatMgr);

	return S_SUCCESS;
}

// ----------------------------------------------------------------------------------
S_API SResult SpeedPointEngine::InitializePhysics()
{
	m_pPhysics.SetOwn(new CPhysics());
	return S_SUCCESS;
}

// ----------------------------------------------------------------------------------
S_API SResult SpeedPointEngine::InitializeScene(IScene* pScene)
{
	SP_ASSERTR(IS_VALID_PTR(pScene), S_INVALIDPARAM);
	m_pScene.SetOwn(pScene);
	m_pScene->Initialize(this);

	return S_SUCCESS;
}

// ----------------------------------------------------------------------------------
S_API void SpeedPointEngine::LoadWorld(const string& file)
{
	SSettingsDesc& settings = GetSettings()->Get();

	size_t lastDelim = file.find_last_of('\\');
	if (lastDelim == file.npos)
	{
		settings.resources.rootDir = "";
		settings.resources.worldFile = file;
	}
	else
	{
		settings.resources.rootDir = file.substr(0, lastDelim);
		settings.resources.worldFile = file.substr(lastDelim);
	}

	CLog::Log(S_DEBUG, "rootDir := '%s'", settings.resources.rootDir.c_str());

	GetScene()->Clear();
	GetScene()->Initialize(this);

	Get3DEngine()->ClearRenderMeshes();
	Get3DEngine()->ClearRenderLights();
	Get3DEngine()->ClearTerrain();

	CSPWLoader loader;
	loader.Load(file);
}

// ----------------------------------------------------------------------------------
S_API string SpeedPointEngine::GetShaderPath(EShaderFileType shaderFile) const
{
	char* relativePath;
	switch (shaderFile)
	{
		// FORWARD:
	case eSHADERFILE_SKYBOX:
		relativePath = "Effects\\skybox.fx";
		break;
	case eSHADERFILE_FORWARD:
		relativePath = "Effects\\illum.fx";	// PS_forward
		break;
	case eSHADERFILE_FORWARD_HELPER:
		relativePath = "Effects\\helper.fx";
		break;
	case eSHADERFILE_FONT:
		relativePath = "Effects\\font.fx";
		break;
	case eSHADERFILE_GUI:
		relativePath = "Effects\\gui.fx";
		break;

		// Deferred Shading:
	case eSHADERFILE_DEFERRED_ZPASS:
		relativePath = "Effects\\zpass.fx";	// PS_zpass
		break;
	case eSHADERFILE_DEFERRED_SHADING:
		relativePath = "Effects\\illum.fx";	// PS_illum
		break;
	case eSHADERFILE_TERRAIN:
		relativePath = "Effects\\terrain.fx";
		break;

	default:
		return "???";
	}

	string path = PROJ_DIR;
	path += "..\\..\\";
	path += relativePath;

	return path;
}

// ----------------------------------------------------------------------------------
S_API string SpeedPointEngine::GetResourcePath(const string& file) const
{
	ISettings* pSettings = GetSettings();
	if (!pSettings)
		return file;

	string& rootDir = pSettings->Get().resources.rootDir;
	return rootDir + ((rootDir.empty() || file.empty()) ? "" : "\\") + file;
}











// ----------------------------------------------------------------------------------
S_API SResult SpeedPointEngine::ExecuteFramePipeline(usint32 iSkippedSections /* = DEFAULT_SKIPPED_SECTIONS */)
{
	// NOTIFY FOR SKIPPED SECTIONS	
	if (!m_bLoggedSkipstages && iSkippedSections)
	{		
		if (iSkippedSections & eFPSEC_AI) CLog::Log(S_DEBUG, "AI calculation will be skipped (Skipped Section)!");
		if (iSkippedSections & eFPSEC_ANIMATION) CLog::Log(S_DEBUG, "Animation calculation will be skipped (Skipped Section)!");
		if (iSkippedSections & eFPSEC_PHYSICS) CLog::Log(S_DEBUG, "Physics calculation will be skipped (Skipped Section)!");
		if (iSkippedSections & eFPSEC_SCRIPT) CLog::Log(S_DEBUG, "Script execution will be skipped (Skipped Section)!");
		if (iSkippedSections & eFPSEC_RENDERING) CLog::Log(S_DEBUG, "Rendering will be skipped  (Skipped Section)!");
		m_bLoggedSkipstages = true;
	}


	assert(IS_VALID_PTR(m_pFramePipeline.pComponent));
	if (Failure(m_pFramePipeline->ExecuteSections(iSkippedSections)))
		return S_ERROR;

	// Write queued log lines to disk	
	//! TODO:  Dont do this after EACH frame.

	unsigned int ioReleaseTimer = StartBudgetTimer("SpeedPointEngine::ExecuteFramePipeline() - Release log io queue");
	
	if (m_pFileLogListener)
		m_pFileLogListener->ReleaseQueue();

	StopBudgetTimer(ioReleaseTimer);


	return S_SUCCESS;
}








// ----------------------------------------------------------------------------------
S_API unsigned int SpeedPointEngine::StartBudgetTimer(const char* name)
{
	if (IS_VALID_PTR((IFramePipeline*)m_pFramePipeline))
		return m_pFramePipeline->StartBudgetTimer(name);

	return 0;
}

// ----------------------------------------------------------------------------------
S_API void SpeedPointEngine::ResumeBudgetTimer(unsigned int id)
{
	if (IS_VALID_PTR((IFramePipeline*)m_pFramePipeline))
		m_pFramePipeline->ResumeBudgetTimer(id);
}

// ----------------------------------------------------------------------------------
S_API void SpeedPointEngine::StopBudgetTimer(unsigned int id)
{
	if (IS_VALID_PTR((IFramePipeline*)m_pFramePipeline))
		m_pFramePipeline->StopBudgetTimer(id);
}

SP_NMSPACE_END
