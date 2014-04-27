// *******************************************************************************************************

//	This file is part of the SpeedPoint Game Engine

//	(c) 2011-2014 Pascal Rosenkranz aka iSmokiieZz
//	All rights reserved.

// *******************************************************************************************************

#include <SSpeedPointEngine.h>
#include <Abstract\Pipelines\SRenderPipeline.h>

//~~~~~~~~~
// TODO: Try to eliminate implementations from the SCore Project
#include <Implementation\DirectX9\SDirectX9Renderer.h>
#include <Implementation\DirectX9\SDirectX9ResourcePool.h>
//~~~~~~~~~

#include <Lighting\SLightSystem.h>
#include <Abstract\SSolidSystem.h>

//~~~~~~~~~
// TODO: Try to eliminate implementations from the SCore Project
#include <Implementation\Geometry\SBasicSolidSystem.h>
//~~~~~~~~~~

//#include <Implementation\DirectX10\SDirectX10Renderer.h>
//#include <Implementation\DirectX11\SDirectX11Renderer.h>
//#include <Implementation\OpenGL\SOpenGLRenderer.h>

namespace SpeedPoint
{
	// *******************************************************************************************************

	S_API bool SSpeedPointEngine::IsValidSettings(const SSettings& settings)
	{
		if (settings.hWnd == 0)
		{
			if (m_pLoggingStream)
				m_pLoggingStream->Report(S_INVALIDPARAM, "Invalid settings given: Window handle is zero.");

			return false;
		}
		else if (settings.nXResolution < 600 || settings.nYResolution < 400)
		{
			if (m_pLoggingStream)
				m_pLoggingStream->Report(S_INVALIDPARAM, "Invalid settings given: Resolution lower than 600x400 not supported!");

			return false;
		}

		return true;
	}

	// *******************************************************************************************************

	S_API SResult SSpeedPointEngine::UpdateSettings(const SSettings& customSettings)
	{
		// Validate the settings given and update vars		
		if (IsValidSettings(customSettings))
		{
			m_Settings = customSettings;
			if (IsInitialized())
			{
				if (m_pRenderer)
				{
					// Update viewport size
					SIZE szTargetViewport = m_pRenderer->GetTargetViewport()->GetSize();
					if (szTargetViewport.cx != m_Settings.nXResolution || szTargetViewport.cy != m_Settings.nYResolution)						
						m_pRenderer->GetTargetViewport()->SetSize(customSettings.nXResolution, customSettings.nYResolution);
// ~~~~~~~
// TODO: Update device size
// ~~~~~~~
				}
			}
		}

		return S_SUCCESS;
	}

	// *******************************************************************************************************

	S_API SResult SSpeedPointEngine::Start()
	{
		// Check if the Engine is already running
		SP_ASSERTX(m_bRunning, this, "Tried to start SpeedPointEngine but it is already running.");

		// Initialize default logging stream if SetCustomLoggingStream() hasnt been called yet
		if (!m_bCustomLogStream) m_pLoggingStream = new SLogStream();

		// Check if settings have been set properly
		SP_ASSERTX(!IsValidSettings(m_Settings), this, "Invalid settings!");		
	
		// Initialize the renderer and its resource pool
		if (m_Settings.tyRendererType == S_DIRECTX9)
		{
			m_pRenderer = (SRenderer*)new SDirectX9Renderer();
			if (Failure(m_pRenderer->Initialize(this, m_Settings.hWnd, m_Settings.nXResolution, m_Settings.nYResolution, false)))
			{
				return LogReport(S_ERROR, "Failed to initialize DirectX9-Renderer!");
			}

			m_pResourcePool = (SResourcePool*)new SDirectX9ResourcePool();
			if (Failure(m_pResourcePool->Initialize(this, m_pRenderer)))
			{				
				return LogReport(S_ERROR, "Failed to initialize Resource Pool!");
			}

			// Initialize the viewports array
			m_pViewports = (SViewport**)(malloc(sizeof(SViewport*)* SP_MAX_VIEWPORTS));
			if (m_pViewports == NULL)
			{
				return LogReport(S_ERROR, "Failed to initialize viewport storage!");
			}

			ZeroMemory(m_pViewports, sizeof(SViewport*)* SP_MAX_VIEWPORTS);
		}
		else
		{
			return LogReport(S_ABORTED, "This Renderer type is not supported yet");
		}

		// Initialize the default viewport		
		m_pViewports[0] = m_pRenderer->GetDefaultViewport();
		m_pViewports[0]->SetCamera(new SCamera());

		// Initialize the root solid system				
		if (Failure(m_PhysWorld.Initialize(this)))
			return LogReport(S_ERROR, "Could not initialize PhysWorld of SpeedPointEngine!");

		// Initialize the FramePipeline
		if (Failure(m_FrameEngine.Initialize(this)))
			return LogReport(S_ERROR, "Could not initialize FramePipeline of SpeedPointEngine!");
		else
			LogReport(S_INFO, "Initialize FramePipeline successfully!");


		// Check for disabled FramePipeline Stages		
#ifdef SP_DEFAULT_SKIPSTAGES
		if (SP_DEFAULT_SKIPSTAGES & S_BEGINFRAME) LogReport(S_INFO, "Will skip S_BEGINFRAME stage as it is currently disabled!");
		if (SP_DEFAULT_SKIPSTAGES & S_DYNAMICS) LogReport(S_INFO, "Will skip all S_DYNAMICS stages as it they are currently disabled!");
		else
		{
			if (SP_DEFAULT_SKIPSTAGES & S_DYNAMICS_ANIM) LogReport(S_INFO, "Will skip S_DYNAMICS_ANIM stage as it is currently disabled!");
			if (SP_DEFAULT_SKIPSTAGES & S_DYNAMICS_INPUT) LogReport(S_INFO, "Will skip S_DYNAMICS_INPUT stage as it is currently disabled!");
			if (SP_DEFAULT_SKIPSTAGES & S_DYNAMICS_PHYSICS) LogReport(S_INFO, "Will skip S_DYNAMICS_PHYSICS stage as it is currently disabled!");
			if (SP_DEFAULT_SKIPSTAGES & S_DYNAMICS_SCRIPT) LogReport(S_INFO, "Will skip S_DYNAMICS_SCRIPT stage as it is currently disabled!");
			if (SP_DEFAULT_SKIPSTAGES & S_DYNAMICS_RENDERSCRIPT) LogReport(S_INFO, "Will skip S_DYNAMICS_RENDERSCRIPT stage as it is currently disabled!");
		}

		if (SP_DEFAULT_SKIPSTAGES & S_RENDER) LogReport(S_INFO, "Will skip all S_RENDER stages as they are currently disabled!");
		else
		{
			if (SP_DEFAULT_SKIPSTAGES & S_RENDER_BEGIN) LogReport(S_INFO, "Will skip S_RENDER_BEGIN stage as it is currently disabled!");
			if (SP_DEFAULT_SKIPSTAGES & S_RENDER_GEOMETRY) LogReport(S_INFO, "Will skip S_RENDER_GEOMETRY stage as it is currently disabled!");
			if (SP_DEFAULT_SKIPSTAGES & S_RENDER_LIGHTING) LogReport(S_INFO, "Will skip S_RENDER_LIGHTING stage as it is currently disabled!");
			if (SP_DEFAULT_SKIPSTAGES & S_RENDER_POST) LogReport(S_INFO, "Will skip S_RENDER_POST stage as it is currently disabled!");
			if (SP_DEFAULT_SKIPSTAGES & S_RENDER_PRESENT) LogReport(S_INFO, "Will skip S_RENDER_PRESENT stage as it is currently disabled!");
		}

		if (SP_DEFAULT_SKIPSTAGES & S_ENDFRAME) LogReport(S_INFO, "Will skip S_ENDFRAME stage as it is currently disabled!");
#endif

		// Set the running flag of the SpeedPointEngine
		m_bRunning = true;
		return S_SUCCESS;
	}

	// ********************************************************************************************

	S_API SResult SSpeedPointEngine::Start(const SSettings& customSettings)
	{
		if (Failure(UpdateSettings(customSettings)))
			return S_ABORTED;

		return Start();
	}

	// ********************************************************************************************

	S_API SResult SSpeedPointEngine::Shutdown(void)
	{
		bool bFailed = false;

		m_bRunning = false;

		// Clear Resources
		bFailed = false;
		if (m_pResourcePool)
		{
			if (Failure(m_pResourcePool->ClearAll())) bFailed = true;			
			delete m_pResourcePool;
			m_pResourcePool = NULL;

			SP_ASSERTX(bFailed, this, "Could not clear ResourcePool!");
		}

		// Clear Viewports
		bFailed = false;
		m_pDefaultViewport = NULL;
		for (UINT iViewport = 1; iViewport < SP_MAX_VIEWPORTS; iViewport++)
		{
			if (m_pViewports[iViewport] != NULL)
			{
				if (Failure(m_pViewports[iViewport]->Clear()))
					bFailed = true;

				delete m_pViewports[iViewport];
			}
		}

		memset((void*)m_pViewports, 0, sizeof(SViewport*) * SP_MAX_VIEWPORTS);
		free(m_pViewports);

		SP_ASSERTX(bFailed, this, "Could not clear viewports!");		

		// Clear Physical Solid System
		SP_ASSERTX(Failure(m_PhysWorld.Clear()), this);

		// Clear Lighting System
		bFailed = false;
		/*if (m_pLightSystem)
		{
			if( Failure( pLightSystem->Clear() ) ) bFailed = true;
			delete pLightSystem;
			pLightSystem = NULL;

			if(bFailed && m_pLoggingStream)
				m_pLogginStream->Report(S_ERROR, "Could not properly clear light system of SpeedPointEngine!");
		}
		*/

		// Clear Renderer
		bFailed = false;
		if (m_pRenderer)
		{
			if (Failure(m_pRenderer->Shutdown())) bFailed = true;
			delete m_pRenderer;
			m_pRenderer = NULL;

			SP_ASSERTX(bFailed, this, "Failed shutdown Renderer!");
		}

		// Clear Frame Pipeline
		bFailed = false;
		if (Failure(m_FrameEngine.Clear()))
			bFailed = true;

		SP_ASSERTX(bFailed, this, "Failed clear frame pipeline!");

		// Clear logging stream		
		if (!m_bCustomLogStream && m_pLoggingStream)
			delete m_pLoggingStream;

		m_pLoggingStream = NULL;
		m_bCustomLogStream = false;

		// Reset settings				
		m_Settings = SSettings();

		return S_SUCCESS;
	}

	// ********************************************************************************************

	S_API SViewport* SSpeedPointEngine::GetViewport(UINT index)
	{
		SP_ASSERTXR(!m_pViewports, 0, this);
		return m_pViewports[index];
	}

	// ********************************************************************************************

	S_API SViewport* SSpeedPointEngine::AddViewport(void)
	{
		SP_ASSERTXR(!m_pViewports, 0, "Viewports array is zero!");		

		// Check for a free viewport slot
		UINT iFreeSlot = 99999999;
		for (UINT i = 0; i < SP_MAX_VIEWPORTS; i++)
		{
			if (m_pViewports[i] == NULL)
			{
				iFreeSlot = i;
				break;
			}
		}

		if (iFreeSlot > SP_MAX_VIEWPORTS)		
			LogReport(S_ERROR, "Cannot add additional Viewport: No free slot!");		

		if (Failure(m_pRenderer->CreateAdditionalViewport(&m_pViewports[iFreeSlot])))
			LogReport(S_ERROR, "Failed add addition viewport!");

		return m_pViewports[iFreeSlot];
	}

	// ********************************************************************************************

	S_API SP_ID SSpeedPointEngine::AddSolid(SResult* pResult)
	{
		SP_ID id = m_PhysWorld.AddSolid();
		if (pResult) *pResult = S_SUCCESS;

		return id;
	}

	// ********************************************************************************************

	S_API SSolid* SSpeedPointEngine::AddSolid(SP_ID* pUID, SResult* pResult)
	{		
		SP_ID id = m_PhysWorld.AddSolid();		
		if (pUID) *pUID = id;
		
		SResult res = (id.iIndex != __TRIVIAL && id.iVersion != __TRIVIAL) ? S_SUCCESS : S_ERROR;
		if (pResult) *pResult = res;

		if (res == S_SUCCESS)
			return m_PhysWorld.GetSolid(id);
		else
			return 0;
	}

	// ********************************************************************************************

	S_API SSolid* SSpeedPointEngine::GetSolid(const SP_ID& id)
	{
		return m_PhysWorld.GetSolid(id);
	}

	// ********************************************************************************************

	S_API SResult SSpeedPointEngine::BeginFrame(unsigned int iSkipStages)
	{		
		if (m_FrameEngine.GetCurrentStage() != S_IDLE)
			return LogE("Tried to BeginFrame while frame pipeline was still busy!");

		// Call the BeginFrame function of the frame pipeline which will call all other
		// states until frame has finished
		if (Failure(m_FrameEngine.BeginFrame()))
			return LogE("Failed to BeginFrame in SpeedPointEngine!");

		// Will get here as soon as Frame has ended successfully
		return S_SUCCESS;
	}

	// ********************************************************************************************

	S_API SResult SSpeedPointEngine::RenderSolid(const SP_ID& iSolid)
	{
		// This function may only be called if in S_RENDER_GEOMETRY state
		if (m_FrameEngine.GetCurrentStage() != S_E_RENDER_GEOMETRY)
			return LogW("Tried to Render solid, but frame pipeline didnt reach Render-Geometry already!");		

		// Get the solid and throw DrawCalls
		SSolid* pSolid = m_PhysWorld.GetSolid(iSolid);
		SP_ASSERTXRD(!pSolid, S_ERROR, this, "Cannot GetSolid. pSolid=%02x", pSolid);

		return m_pRenderer->GetRenderPipeline()->RenderSolidGeometry(pSolid, m_Settings.bEnableTextures);
	}

	// ********************************************************************************************

	S_API SResult SSpeedPointEngine::SetCustomLogStream( SLogStream* pLogStream )
	{
		if (pLogStream == NULL) return S_INVALIDPARAM;

		if (pLogStream == m_pLoggingStream) return S_SUCCESS;

		// Destroy old Logging stream instance if default log stream
		if( !m_bCustomLogStream && m_pLoggingStream != NULL )
			delete m_pLoggingStream;					

		m_bCustomLogStream = true;

		m_pLoggingStream = pLogStream;
		LogReport( S_INFO, "Switched Logging Stream successfully" );

		return S_SUCCESS;
	}

	// ********************************************************************************************
	
	S_API SResult SSpeedPointEngine::LogReport( SResult res, SString msg )
	{
		if( m_pLoggingStream == NULL ) return S_ABORTED;		

		char* cPrefix;
		switch( m_Settings.tyRendererType )
		{
		case S_DIRECTX9: cPrefix = "[SpeedPoint:DX9]"; break;
		case S_DIRECTX10: cPrefix = "[SpeedPoint:DX10]"; break;
		case S_DIRECTX11: cPrefix = "[SpeedPoint:DX11]"; break;
		case S_OPENGL: cPrefix = "[SpeedPoint:GL]"; break;
		default: cPrefix = "[SpeedPoint:??]"; break;
		}
		
		SString sActualMsg(new char[msg.GetLength() + strlen(cPrefix) + 2]);
		
		wsprintf((char*)sActualMsg, "%s %s", cPrefix, (char*)msg);

		return m_pLoggingStream->Report( res, sActualMsg );
	}

	// ********************************************************************************************
	
	S_API SResult SSpeedPointEngine::LogE(SString msg)
	{
		return LogReport(S_ERROR, msg);
	}

	// ********************************************************************************************

	S_API SResult SSpeedPointEngine::LogW(SString msg)
	{
		return LogReport(S_WARN, msg);
	}

	// ********************************************************************************************

	S_API SResult SSpeedPointEngine::LogI(SString msg)
	{
		return LogReport(S_INFO, msg);
	}

	// ********************************************************************************************

	S_API void SSpeedPointEngine::HandleException(char* msg)
	{
		LogReport(S_ERROR, msg);		
	}

	// ********************************************************************************************

	S_API bool SSpeedPointEngine::IsInitialized()
	{
		// Simply check the m_bRunning switch here, which is set at the end of the Initialize() method
		return m_bRunning;
	}	
}