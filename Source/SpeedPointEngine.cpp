// *******************************************************************************************************

//	This file is part of the SpeedPoint Game Engine

//	(c) 2011-2015 Pascal Rosenkranz aka iSmokiieZz
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
#include <fstream>
#include <Abstract\IScene.h>
#include <Implementation\Geometry\Material.h>
#include <Implementation\3DEngine\C3DEngine.h>

SP_NMSPACE_BEG

CLogIntrnl g_LogIntrnl;
	


S_API void EngineFileLog::Clear()
{
	if (m_LogFile.is_open())
		m_LogFile.close();
}

S_API SResult EngineFileLog::SetLogFile(const SString& file)
{
	if (m_LogFile.is_open())
		m_LogFile.close();

	m_LogFile.open(file, std::ofstream::trunc);
	if (!m_LogFile.is_open())
		return S_ERROR;

	CLog::Log(S_DEBUG, "Opened %s as log file!", (char*)file);

	return S_SUCCESS;
}

S_API SResult EngineFileLog::RegisterLogHandler(IFileLogHandler* pLogHandler)
{
	assert(IS_VALID_PTR(pLogHandler));

	// check if already registered
	for (auto itLH = m_LogHandlers.begin(); itLH != m_LogHandlers.end(); itLH++)
	{
		if (*itLH == pLogHandler)
			return S_SUCCESS;
	}

	m_LogHandlers.push_back(pLogHandler);
	return S_SUCCESS;
}

S_API SResult EngineFileLog::SetLogLevel(ELogLevel loglevel)
{
	m_LogLevel = loglevel;
	return S_SUCCESS;
}

S_API ELogLevel EngineFileLog::GetLogLevel() const
{
	return m_LogLevel;
}

S_API SResult EngineFileLog::Log(SResult res, const SString& msg)
{
	SString formattedMsg;
	/*
	switch (res.result)
	{
	case S_INFO:
	case S_SUCCESS:	
		formattedMsg = SString("INFO: ") + msg;
		break;
	case S_ERROR:
	case S_ABORTED:
		formattedMsg = SString("ERR: ") + msg;
		break;
	case S_WARN:
		if (m_LogLevel < ELOGLEVEL_WARN)
			return res;
		formattedMsg = SString("WARN: ") + msg;
		break;
	case S_DEBUG:
		if (m_LogLevel < ELOGLEVEL_DEBUG)
			return res;
		formattedMsg = SString("DEBG: ") + msg;
		break;
	default:
		formattedMsg = SString("INFO: ") + msg;
		break;
	}
	*/

	//formattedMsg += "\n";
	if (m_LogHandlers.size() > 0)
	{
		for (auto itLH = m_LogHandlers.begin(); itLH != m_LogHandlers.end(); itLH++)
			(*itLH)->OnLog(res, msg);
	}

	if (m_LogFile.is_open())
		m_LogFile << msg;

	return res;
}





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

	// Handle all shutdown handlers first	
	unsigned int nHandled = m_ShutdownHandlers.size();
	for (auto itShutdownHandler = m_ShutdownHandlers.begin(); itShutdownHandler != m_ShutdownHandlers.end();)
	{	
		IShutdownHandler* pSH = *itShutdownHandler;

		// definetly increments, no matter if pSH was found or not.
		itShutdownHandler = UnregisterShutdownHandlerIntrnl(pSH, itShutdownHandler);

		if (Failure(pSH->HandleShutdown()))
			LogD("Shutdown-Handler failed: " + (*itShutdownHandler)->GetShutdownHandlerDesc() + "!");
	}
	
	if (nHandled > 0)
	{
		char* pWarnStr = new char[200];
		memset(pWarnStr, 0, 200);
		sprintf_s(pWarnStr, 200, "Cleared %d objects via shutdown handler!", nHandled);
		LogW(pWarnStr);
		delete[] pWarnStr;
	}

	m_ShutdownHandlers.clear();

	// clear scene
	if (IS_VALID_PTR(m_pScene.pComponent))
		m_pScene->Clear();
	
	m_pFontRenderer.Clear();

	m_pMaterialManager.Clear();

	// calls IRenderer::~IRenderer implementation which will destruct the resource pool	
	m_pRenderer.Clear();

	m_pFramePipeline.Clear();	
	
	if (m_pSettings)
	{
		delete m_pSettings;
		m_pSettings = nullptr;
	}

	CLog::Log(S_INFO, "Engine shut down");
	m_FileLog.Clear();
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
		m_pApplication->OnInit(m_pFramePipeline, this);

		if (!IS_VALID_PTR(m_pFontRenderer.pComponent))
			CLog::Log(S_ERROR, "Font Renderer is not initialized!");
	}
}

// ----------------------------------------------------------------------------------
S_API SResult SpeedPointEngine::FinishInitialization()
{	
	// Register the framepipeline sections
	if (!IS_VALID_PTR(m_pRenderer.pComponent)) return S_ERROR;
	
	// Hook render pipeline
	IRenderPipeline* pRenderPipeline = new RenderPipeline();
	pRenderPipeline->Init(this);
	m_pFramePipeline->RegisterSection(pRenderPipeline);

	// Hook dynamics pipeline	

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
	if (Failure(m_pRenderer->Initialize(this, false)))
	{
		return S_ERROR;
	}


	LogD("Initialized Renderer.");

	return S_SUCCESS;
}

// ----------------------------------------------------------------------------------
S_API SResult SpeedPointEngine::Initialize3DEngine()
{	
	if (!IS_VALID_PTR(m_pRenderer.pComponent))
	{
		return LogE("Failed Initialize 3DEngine: Renderer must be initialized first!");		
	}

	m_p3DEngine.SetOwn(new C3DEngine(m_pRenderer));	

	LogD("Initialized 3DEngine.");
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

	LogD("Initialized Font Renderer");
	return S_SUCCESS;
}

// ----------------------------------------------------------------------------------
S_API SResult SpeedPointEngine::InitializeResourcePool()
{
	assert(IS_VALID_PTR(m_pRenderer.pComponent));
	m_pResourcePool.SetCustom(m_pRenderer->GetResourcePool());

	LogD("Initialized Resource pool!");

	m_pMaterialManager.SetOwn(new BasicMaterialManager());

	return S_SUCCESS;
}

// ----------------------------------------------------------------------------------
S_API SResult SpeedPointEngine::InitializeLogger(IFileLogHandler* pCustomFileLogHandler /* = 0 */)
{
	if (IS_VALID_PTR(pCustomFileLogHandler))
		m_FileLog.RegisterLogHandler(pCustomFileLogHandler);

	CLog::RegisterListener((ILogListener*)this);

	LogD("Initialized File Logger!");


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
S_API SString SpeedPointEngine::GetShaderPath(EShaderFileType shaderFile)
{
	char* relativePath;
	switch (shaderFile)
	{
		// FORWARD:
	case eSHADERFILE_FORWARD:
		relativePath = "Effects\\illum.fx";	// PS_forward
		break;
	case eSHADERFILE_HELPER:
		relativePath = "Effects\\helper.fx";
		break;
	case eSHADERFILE_FONT:
		relativePath = "Effects\\font.fx";
		break;

		// Deferred Shading:
	case eSHADERFILE_ZPASS:
		relativePath = "Effects\\zpass.fx";	// PS_zpass
		break;
	case eSHADERFILE_DEFERRED_SHADING:
		relativePath = "Effects\\illum.fx";	// PS_illum
		break;
	case eSHADERFILE_TERRAIN:
		relativePath = "Effects\\terrain.fx";
		break;

		// Deferred lighting / Light prepass:
	case eSHADERFILE_DL_ZPASS:
		relativePath = "Effects\\zpass.fx";	// PS_dlzpass
		break;
	case eSHADERFILE_DL_LIGHT:
		relativePath = "Effects\\illum.fx";	// PS_dllight
		break;
	case eSHADERFILE_DL_COMPOSITE:
		relativePath = "Effects\composite.fx";

	default:
		return "???";
	}

#ifdef _DEBUG
	char pFXFile[500];
	sprintf_s(pFXFile, "%s..\\%s", SOL_DIR, relativePath);
	relativePath = pFXFile;
#endif

	return SString(relativePath);
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
	// --> CLog --> SpeedPointEngine::OnLog()
	return m_LogWrapper.Log(res, msg);



	/*
	char* cPrefix;
	switch( m_pSettings->Get().render.tyRendererType )
	{
	case S_DIRECTX9: cPrefix = "[SpeedPoint:DX9] "; break;
	case S_DIRECTX10: cPrefix = "[SpeedPoint:DX10] "; break;
	case S_DIRECTX11: cPrefix = "[SpeedPoint:DX11] "; break;
	case S_OPENGL: cPrefix = "[SpeedPoint:GL] "; break;
	default: cPrefix = "[SpeedPoint:??] "; break;
	}
		
	//usint32 nActualMsgLen = msg.GetLength() + strlen(cPrefix) + 10;			
	SString sActualMsg;
		
	sActualMsg = cPrefix + msg;	

	if (IS_VALID_PTR(m_pApplication))
		m_pApplication->OnLogReport(res, sActualMsg);

	return m_pLog->Log( res, sActualMsg );
	*/

}

// ----------------------------------------------------------------------------------
S_API SResult SpeedPointEngine::LogE(const SString& msg) { return LogReport(S_ERROR, msg); }
S_API SResult SpeedPointEngine::LogW(const SString& msg) { return LogReport(S_WARN, msg); }
S_API SResult SpeedPointEngine::LogI(const SString& msg) { return LogReport(S_INFO, msg); }
S_API SResult SpeedPointEngine::LogD(const SString& msg, SResultType defRetVal /* = S_DEBUG */)
{
	LogReport(S_DEBUG, msg);
	return defRetVal;
}

S_API void SpeedPointEngine::LogD(const SMatrix4& mtx, const SString& mtxname)
{
	LogD(SString("Dump matrix ") + mtxname);

	char* cDumpLn = new char[256];

	for (unsigned short i = 0; i < 4; ++i)
	{
		memset(cDumpLn, 0, 256);
		sprintf_s(cDumpLn, 256, "[ %f  %f  %f  %f ]",
			mtx.m[i][0], mtx.m[i][1], mtx.m[i][2], mtx.m[i][3]);

		LogD(cDumpLn);
	}

	delete[] cDumpLn;
}
S_API void SpeedPointEngine::LogD(const SVector3& vec, const SString& vecname)
{
	LogD(SString("Dump vector ") + vecname);
	char* cDumpLn = new char[256];
	memset(cDumpLn, 0, 256);
	sprintf_s(cDumpLn, 256, "( %f  %f  %f )", vec.x, vec.y, vec.z);
	LogD(cDumpLn);
	delete[] cDumpLn;
}
S_API void SpeedPointEngine::LogD(bool b, const SString& boolname)
{
	LogD(SString("Dump: ") + boolname + (b ? " = true" : " = false"));
}
S_API void SpeedPointEngine::LogD(unsigned int i, const SString& intname)
{
	char* cDump = new char[60];
	memset(cDump, 0, 60);
	sprintf_s(cDump, 60, "%d", i);
	LogD(SString("Dump: ") + intname + " = " + cDump);
	delete[] cDump;
}
S_API void SpeedPointEngine::LogD(float f, const SString& floatname)
{
	char* cDump = new char[60];
	memset(cDump, 0, 60);
	sprintf_s(cDump, 60, "%f", f);
	LogD(SString("Dump: ") + floatname + " = " + cDump);
	delete[] cDump;
}
S_API void SpeedPointEngine::LogD(const SString& str, const SString& strname)
{
	LogD(SString("Dump: ") + strname + " = \"" + str + "\"");	
}

// ----------------------------------------------------------------------------------
S_API void SpeedPointEngine::OnLog(SResult res, const SString& msg)
{
	// console output is done in CLog, which calls this function

	m_FileLog.Log(res, msg);

	if (IS_VALID_PTR(m_pApplication))
		m_pApplication->OnLogReport(res, msg);	
}

// ----------------------------------------------------------------------------------
S_API void SpeedPointEngine::HandleException(char* msg)
{
	LogReport(S_ERROR, msg);
	MessageBox(nullptr, msg, "Assertion failed", MB_ICONERROR | MB_OK);
	m_bRunning = false;
}





SP_NMSPACE_END