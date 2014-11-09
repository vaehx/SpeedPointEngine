// *******************************************************************************************************

//	This file is part of the SpeedPoint Game Engine

//	(c) 2011-2014 Pascal Rosenkranz aka iSmokiieZz
//	All rights reserved.

// *******************************************************************************************************

#include <SpeedPointEngine.h>
#include <Abstract\IRenderer.h>
#include <Abstract\IRenderPipeline.h>
#include <Pipelines\FramePipeline.h>
#include <Util\SLogStream.h>
#include <EngineSettings.h>
#include <Abstract\IApplication.h>
#include <Pipelines\RenderPipeline.h>

SP_NMSPACE_BEG
	

// ----------------------------------------------------------------------------------
S_API SpeedPointEngine::SpeedPointEngine()
: m_bRunning(false),
m_bLoggedSkipstages(false),
m_pApplication(nullptr)
{
	m_pSettings = new EngineSettings();
	m_pSettings->Initialize(this);
}

// ----------------------------------------------------------------------------------
S_API SpeedPointEngine::~SpeedPointEngine()
{
	Shutdown();
}

// ----------------------------------------------------------------------------------
S_API void SpeedPointEngine::Shutdown(void)
{
	m_bRunning = false;

	// calls IRenderer::~IRenderer implementation which will destruct the resource pool
	m_pRenderer.Clear();

	m_pFramePipeline.Clear();
	m_pLoggingStream.Clear();
	
	if (m_pSettings)
	{
		delete m_pSettings;
		m_pSettings = nullptr;
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
		&& IS_VALID_PTR(m_pResourcePool.pComponent)
		&& IS_VALID_PTR(m_pLoggingStream.pComponent))
	{		
		m_pApplication->OnInit(m_pFramePipeline, this);
	}
}

// ----------------------------------------------------------------------------------
S_API SResult SpeedPointEngine::FinishInitialization()
{	
	// Register the framepipeline sections
	if (!IS_VALID_PTR(m_pRenderer.pComponent)) return S_ERROR;
	
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

	LogD("Initialized Frame pipeline.");

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
	if (Failure(m_pRenderer->Initialize(this, m_pSettings->Get().app.hWnd, m_pSettings->Get().app.nXResolution, m_pSettings->Get().app.nYResolution, false)))
		return S_ERROR;


	LogD("Initialized Renderer.");

	return S_SUCCESS;
}

// ----------------------------------------------------------------------------------
S_API SResult SpeedPointEngine::InitializeResourcePool()
{
	assert(IS_VALID_PTR(m_pRenderer.pComponent));
	m_pResourcePool.SetCustom(m_pRenderer->GetResourcePool());

	LogD("Initiailzed Resource pool!");

	return S_SUCCESS;
}

// ----------------------------------------------------------------------------------
S_API SResult SpeedPointEngine::InitializeLogger(SLogStream* pCustomLogStream /* = 0 */)
{
	if (IS_VALID_PTR(pCustomLogStream))
		m_pLoggingStream.SetCustom(pCustomLogStream);
	else
		m_pLoggingStream.SetOwn(new SLogStream());



	m_pLoggingStream->Initialize();


	LogD("Initialized Logger!");


	return S_SUCCESS;
}


















// ----------------------------------------------------------------------------------
S_API SResult SpeedPointEngine::ExecuteFramePipeline(usint32 iSkippedSections /* = DEFAULT_SKIPPED_SECTIONS */)
{
	// NOTIFY FOR SKIPPED SECTIONS	
	if (!m_bLoggedSkipstages && iSkippedSections)
	{		
		if (iSkippedSections & eFPSEC_AI) LogD("AI calculation will be skipped (Skipped Section)!");
		if (iSkippedSections & eFPSEC_ANIMATION) LogD("Animation calculation will be skipped (Skipped Section)!");
		if (iSkippedSections & eFPSEC_PHYSICS) LogD("Physics calculation will be skipped (Skipped Section)!");
		if (iSkippedSections & eFPSEC_SCRIPT) LogD("Script execution will be skipped (Skipped Section)!");
		if (iSkippedSections & eFPSEC_RENDERING) LogD("Rendering will be skipped  (Skipped Section)!");
		m_bLoggedSkipstages = true;
	}


	assert(IS_VALID_PTR(m_pFramePipeline.pComponent));
	return m_pFramePipeline->ExecuteSections(iSkippedSections);
}
















// ----------------------------------------------------------------------------------
S_API SResult SpeedPointEngine::LogReport( const SResult& res, const SString& msg )
{
	if( m_pLoggingStream == NULL ) return S_ABORTED;		

	char* cPrefix;
	switch( m_pSettings->Get().render.tyRendererType )
	{
	case S_DIRECTX9: cPrefix = "[SpeedPoint:DX9]"; break;
	case S_DIRECTX10: cPrefix = "[SpeedPoint:DX10]"; break;
	case S_DIRECTX11: cPrefix = "[SpeedPoint:DX11]"; break;
	case S_OPENGL: cPrefix = "[SpeedPoint:GL]"; break;
	default: cPrefix = "[SpeedPoint:??]"; break;
	}
		
	usint32 nActualMsgLen = msg.GetLength() + strlen(cPrefix) + 10;		
	SString sActualMsg(nActualMsgLen);
		
	sprintf_s((char*)sActualMsg, nActualMsgLen, "%s %s", cPrefix, (char*)msg);		

	if (IS_VALID_PTR(m_pApplication))
		m_pApplication->OnLogReport(res, sActualMsg);

	return m_pLoggingStream->Report( res, sActualMsg );
}

// ----------------------------------------------------------------------------------
S_API SResult SpeedPointEngine::LogE(const SString& msg) { return LogReport(S_ERROR, msg); }
S_API SResult SpeedPointEngine::LogW(const SString& msg) { return LogReport(S_WARN, msg); }
S_API SResult SpeedPointEngine::LogI(const SString& msg) { return LogReport(S_INFO, msg); }
S_API SResult SpeedPointEngine::LogD(const SString& msg) { return LogReport(S_DEBUG, msg); }

// ----------------------------------------------------------------------------------
S_API void SpeedPointEngine::HandleException(char* msg)
{
	LogReport(S_ERROR, msg);
	MessageBox(nullptr, msg, "Assertion failed", MB_ICONERROR | MB_OK);
	m_bRunning = false;
}

// ----------------------------------------------------------------------------------
S_API SResult SpeedPointEngine::AddLogHandler(PLogHandler pHandler)
{
	assert(IS_VALID_PTR(pHandler));
	assert(IS_VALID_PTR(m_pLoggingStream.pComponent));

	m_pLoggingStream->RegisterHandler(pHandler);

	return IS_VALID_PTR(pHandler) ? S_SUCCESS : S_INVALIDPARAM;
}






SP_NMSPACE_END