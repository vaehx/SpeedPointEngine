// SpeedPoint Engine Settings

#include <EngineSettings.h>

SP_NMSPACE_BEG


// ------------------------------------------------------------------------------------------
S_API EngineSettings::EngineSettings()
: m_pGameEngine(nullptr)
{
}

// ------------------------------------------------------------------------------------------
S_API EngineSettings::~EngineSettings()
{
}

// ------------------------------------------------------------------------------------------
S_API SResult EngineSettings::Initialize(IGameEngine* pGameEngine)
{
	m_pGameEngine = pGameEngine;
	return (m_pGameEngine) ? S_SUCCESS : S_INVALIDPARAM;
}

// ------------------------------------------------------------------------------------------
S_API SResult EngineSettings::Set(const SSettingsDesc& dsc)
{
	// APPLICATION
	if (dsc.CheckMask(ENGSETTING_HWND)) m_Desc.app.hWnd = dsc.app.hWnd;
	if (dsc.CheckMask(ENGSETTING_RESOLUTION))
	{
		m_Desc.app.nXResolution = dsc.app.nXResolution;
		m_Desc.app.nYResolution = dsc.app.nYResolution;
	}
	if (dsc.CheckMask(ENGSETTING_MULTITHREADING)) m_Desc.app.bMultithreaded = dsc.app.bMultithreaded;
	if (dsc.CheckMask(ENGSETTING_WINDOWEDMODE)) m_Desc.app.bWindowed = dsc.app.bMultithreaded;


	// RENDER
	if (dsc.CheckMask(ENGSETTING_ENABLEVSYNC)) m_Desc.render.bEnableVSync = dsc.render.bEnableVSync;
	if (dsc.CheckMask(ENGSETTING_VSYNCINTERVAL)) m_Desc.render.vsyncInterval = dsc.render.vsyncInterval;
	if (dsc.CheckMask(ENGSETTING_AAQUALITY)) m_Desc.render.antiAliasingQuality = dsc.render.antiAliasingQuality;
	if (dsc.CheckMask(ENGSETTING_SHADOWQUALITY)) m_Desc.render.shadowQuality = dsc.render.shadowQuality;
	if (dsc.CheckMask(ENGSETTING_FRONTFACE_TYPE)) m_Desc.render.frontFaceType = dsc.render.frontFaceType;
	if (dsc.CheckMask(ENGSETTING_LIGHTING)) m_Desc.render.bRenderLighting = dsc.render.bRenderLighting;
	if (dsc.CheckMask(ENGSETTING_WIREFRAME)) m_Desc.render.bRenderWireframe = dsc.render.bRenderWireframe;
	if (dsc.CheckMask(ENGSETTING_CLIPPLANES))
	{
		m_Desc.render.fClipNear = dsc.render.fClipNear;
		m_Desc.render.fClipFar = dsc.render.fClipFar;
	}
	if (dsc.CheckMask(ENGSETTING_ENABLETEXTURES)) m_Desc.render.bRenderTextures = dsc.render.bRenderTextures;	
	if (dsc.CheckMask(ENGSETTING_RENDERAPI)) m_Desc.render.tyRendererType = dsc.render.tyRendererType;

	return S_SUCCESS;
}



SP_NMSPACE_END