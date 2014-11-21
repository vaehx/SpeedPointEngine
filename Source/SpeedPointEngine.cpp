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
#include <fstream>

SP_NMSPACE_BEG
	


S_API void EngineLog::Clear()
{
	m_Buffer = "";
}

S_API SResult EngineLog::SaveToFile(const SString& file, bool replace)
{
	std::ofstream of;
	of.open(file, std::ofstream::out | (replace ? std::ofstream::trunc : std::ofstream::app));
	if (!of.is_open())
	{
		OutputDebugString(SString("Could not open file: ") + file);
		return S_ERROR;
	}

	of << m_Buffer;
	of.close();
	return S_SUCCESS;
}

S_API SResult EngineLog::RegisterLogHandler(ILogHandler* pLogHandler)
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

S_API SResult EngineLog::SetLogLevel(ELogLevel loglevel)
{
	m_LogLevel = loglevel;
	return S_SUCCESS;
}

S_API ELogLevel EngineLog::GetLogLevel() const
{
	return m_LogLevel;
}

S_API SResult EngineLog::Log(SResult res, const SString& msg)
{
	SString formattedMsg;
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

	formattedMsg += "\n";
	m_Buffer += formattedMsg;	
	if (m_LogHandlers.size() > 0)
	{
		for (auto itLH = m_LogHandlers.begin(); itLH != m_LogHandlers.end(); itLH++)
			(*itLH)->OnLog(res, formattedMsg);
	}

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

	// calls IRenderer::~IRenderer implementation which will destruct the resource pool
	m_pRenderer.Clear();

	m_pFramePipeline.Clear();	
	
	if (m_pSettings)
	{
		delete m_pSettings;
		m_pSettings = nullptr;
	}

	if (IS_VALID_PTR(m_pLog))
	{
		m_pLog->LogI("Engine shut down");
		m_pLog->Clear();
		delete m_pLog;
		m_pLog = nullptr;
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
		&& IS_VALID_PTR(m_pLog))
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
S_API SResult SpeedPointEngine::InitializeLogger(ILogHandler* pCustomLogHandler /* = 0 */)
{
	m_pLog = new EngineLog();

	if (IS_VALID_PTR(pCustomLogHandler))
		m_pLog->RegisterLogHandler(pCustomLogHandler);

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
	if( !IS_VALID_PTR(m_pLog) )
		return S_ABORTED;		

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

	return m_pLog->Log( res, sActualMsg );
}

// ----------------------------------------------------------------------------------
S_API SResult SpeedPointEngine::LogE(const SString& msg) { return LogReport(S_ERROR, msg); }
S_API SResult SpeedPointEngine::LogW(const SString& msg) { return LogReport(S_WARN, msg); }
S_API SResult SpeedPointEngine::LogI(const SString& msg) { return LogReport(S_INFO, msg); }
S_API SResult SpeedPointEngine::LogD(const SString& msg) { return LogReport(S_DEBUG, msg); }
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
S_API void SpeedPointEngine::HandleException(char* msg)
{
	LogReport(S_ERROR, msg);
	MessageBox(nullptr, msg, "Assertion failed", MB_ICONERROR | MB_OK);
	m_bRunning = false;
}





SP_NMSPACE_END