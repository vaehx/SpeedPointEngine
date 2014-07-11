//////////////////////////////////////////////////////////////////////////////////////////////////
//
//	This file is part of the SpeedPoint Game Engine
//
//	(c) 2011-2014 Pascal R. aka iSmokiieZz
//	All rights reserved.
//
//////////////////////////////////////////////////////////////////////////////////////////////////

#pragma once

#include <Implementation\DirectX11\DirectX11LightingRenderSection.h>
#include <Implementation\DirectX11\DirectX11RenderPipeline.h>
#include <Abstract\IRenderer.h>
#include <SpeedPointEngine.h>
#include <Implementation\DirectX11\DirectX11Renderer.h>
#include <Implementation\DirectX11\DirectX11OutputPlane.h>

SP_NMSPACE_BEG


// ********************************************************************************************

S_API DirectX11LightingRenderSection::DirectX11LightingRenderSection()
: m_pEngine(0),
m_pDXRenderPipeline(0),
m_pLightSources(0)
{
}

// ********************************************************************************************

S_API DirectX11LightingRenderSection::~DirectX11LightingRenderSection()
{
	Clear();
}

// ********************************************************************************************

S_API SResult DirectX11LightingRenderSection::Initialize(SpeedPointEngine* pEngine, IRenderPipeline* pRenderPipeline)
{
	if (pEngine == 0 || pRenderPipeline == 0)
		return S_INVALIDPARAM;

	if (pRenderPipeline->GetRenderer() == 0 || pRenderPipeline->GetRenderer()->GetType() != S_DIRECTX11)
	{
		return pEngine->LogReport(S_NOTINIT, "Could not initialize Lighting Render Section: Renderer invalid or not initialized!");
	}

	// -----------------------------------------------------------------------------------

	m_pEngine = pEngine;
	m_pDXRenderPipeline = (DirectX11RenderPipeline*)pRenderPipeline;

	// Prepare Lighting buffer
	SSettings settings = pEngine->GetSettings();
	if (Failure(m_LightingBuffer.Initialize(eFBO_LIGHT, m_pEngine, m_pEngine->GetRenderer(), settings.app.nXResolution, settings.app.nYResolution)))
	{
		return m_pEngine->LogE("Cannot initialize lighting render section: failed to initialize lighting output buffer");
	}

	// Load and compile lighting shader
	char* pLightingFXFile;
#ifdef _DEBUG
	pLightingFXFile = new char[500];
	sprintf_s(pLightingFXFile, 500, "%s..\\Effects\\light.fx", SOL_DIR);
#else
	pLightingFXFile = "Effects\\light.fx";
#endif

	// Initialize the lighting effect
	if (Failure(m_LightingEffect.Initialize(m_pEngine, pLightingFXFile)))
	{
		return m_pEngine->LogE("Failed to load lighting creation effect");
	}


	return S_SUCCESS;
}

// ********************************************************************************************

S_API SResult DirectX11LightingRenderSection::Clear(void)
{
	FreeLightSources();

	m_LightingBuffer.Clear();
	m_LightingEffect.Clear();
	m_pEngine = 0;
	m_pDXRenderPipeline = 0;

	return S_SUCCESS;
}

// ********************************************************************************************

S_API SResult DirectX11LightingRenderSection::PrepareSection()
{
	if (m_pEngine == 0) return S_NOTINIT;

	if (m_pDXRenderPipeline == 0 || !m_pDXRenderPipeline->IsInitialized())
		return m_pEngine->LogReport(S_NOTINIT, "Cannot prepare lighting render section: Render pipeline not initialized!");

	// Get the DX Renderer
	DirectX11Renderer* pDXRenderer = (DirectX11Renderer*)m_pDXRenderPipeline->GetRenderer();
	if (pDXRenderer == 0)
		return m_pEngine->LogReport(S_NOTINIT, "Cannot prepare lighting render section: Renderer not initialized!");

	// -----------------------------------------------------------------------------------
	// Check buffers and effects in advance

	if (!m_LightingBuffer.IsInitialized())	
		return m_pEngine->LogReport(S_NOTINIT, "Could not prepare lighting render section: Lighting buffer not initialized!");

	if (!m_LightingEffect.IsInitialized())
		return m_pEngine->LogReport(S_NOTINIT, "Could not prepare lighting render section: Lighting effect not initialized!");

	// -----------------------------------------------------------------------------------
	// Set the render target
	// We assume, that the previous render sections properly cleared all render targets with index >0

	if (Failure(pDXRenderer->SetRenderTarget(&m_LightingBuffer)))
		return m_pEngine->LogE("Could not prepare Lighting render section: Failed to Set Lighting Buffer as render target 0!");

	// -----------------------------------------------------------------------------------
	// Clear the frame buffer object

	if (Failure(pDXRenderer->ClearRenderTargets()))
		return m_pEngine->LogE("Could not prepare Lighting render section: Cannot clear lighting frame buffer object!");

	return S_SUCCESS;
}

// ********************************************************************************************

S_API SResult DirectX11LightingRenderSection::EndSection()
{
	if (m_pEngine == 0) return S_NOTINIT;

	if (m_pDXRenderPipeline == 0 || !m_pDXRenderPipeline->IsInitialized())
		return m_pEngine->LogReport(S_NOTINIT, "Cannot end lighting render section: Render pipeline not initialized!");

	// Get the DX Renderer
	DirectX11Renderer* pDXRenderer = (DirectX11Renderer*)m_pDXRenderPipeline->GetRenderer();
	if (pDXRenderer == 0)
		return m_pEngine->LogReport(S_NOTINIT, "Cannot end lighting render section: Renderer not initialized!");

	// -----------------------------------------------------------------------------------

	// Reset the render target

	// Note:
	//	it would be a waste of time to reset the render target as we overwrite it in the next render section already


	// okay. done.
	return S_SUCCESS;
}

// ********************************************************************************************

S_API SResult DirectX11LightingRenderSection::PrepareLightSources(SPool<SLight*>** pLightSources)
{
	if (m_pEngine == 0) return S_NOTINIT;

	if (m_pDXRenderPipeline == 0 || !m_pDXRenderPipeline->IsInitialized())
		return m_pEngine->LogReport(S_NOTINIT, "Cannot prepare light sources in lighting render section: Render pipeline not initialized!");

	//~~~~~~~~~~~~~
	// TODO: Increase performance by checking if light source buffer has to be updated at all
	if (m_pLightSources != 0)
		return m_pEngine->LogReport(S_ABORTED, "Cannot prepare light sources in lighting render section: Last light source buffer not cleared!");
	//~~~~~~~~~~~~~

	// -----------------------------------------------------------------------------------
	// Load all default lights if full custom lighting is not requested

	if (!m_pEngine->GetSettings().render.bCustomLighting)
	{
		SLightSystem* pLightSystem = m_pEngine->GetGlobalLightSystem();
		usint32 nLightSources = pLightSystem->GetLightCount();
		usint32 nMaxLightSources;

		if (pLightSources != 0 && (*pLightSources) != 0)
		{
			m_pLightSources = *pLightSources;
			nMaxLightSources = m_pLightSources->GetUsageSize();
		}
		else
		{
			nMaxLightSources = nLightSources;
			m_pLightSources = new SPool<SLight*>(nMaxLightSources);
		}

		// Now fill buffer with light sources according to free slots in the pool
		if (nMaxLightSources == 0 && nLightSources > nMaxLightSources)
			return m_pEngine->LogE("Could not prepare light sources in light render section: Given light source pool is not sufficient for global lights");

		SP_ID* pLightIDs = 0;
		usint32 nLightIDs = 0;
		// ~~~~~~~~~~
		// TODO: Let this be specifiable by parameter
		SCamera* pCamera = m_pEngine->GetRenderer()->GetTargetViewport()->GetCamera();
		//~~~~~~~~~~
		if (Failure(pLightSystem->GetLightsInRange(pCamera->vPosition, pCamera->GetViewRadius(), &pLightIDs, &nLightIDs)))
			return m_pEngine->LogE("Could not prepare light sources in light render section: Could not retrieve lights in range!");

		if (nLightIDs > 0)
		{
			for (usint32 i = 0; i < nLightIDs; ++i)
			{
				SLight* pLight = pLightSystem->GetLight(pLightIDs[i]);
				if (pLight != 0)
					m_pLightSources->AddItem(pLight, 0);
			}
		}
	}
	else
	{
		// At least prepare Light source buffer if not already done 			
		if (pLightSources != 0 && (*pLightSources) != 0)
			m_pLightSources = *pLightSources;
		else
			m_pLightSources = new SPool<SLight*>();
	}

	if (pLightSources != 0)
		*pLightSources = m_pLightSources;

	return S_SUCCESS;
}

// ********************************************************************************************

S_API SResult DirectX11LightingRenderSection::FreeLightSources()
{
	if (m_pLightSources != 0)
	{
		m_pLightSources->Clear();
		delete m_pLightSources;
		m_pLightSources = 0;
	}

	return S_SUCCESS;
}

// ********************************************************************************************

S_API SResult DirectX11LightingRenderSection::RenderLighting(DirectX11GeometryRenderSection* pDXGeometrySection, DirectX11OutputPlane* pOutputPlane)
{
	SResult res = S_SUCCESS;

	SP_ASSERTR(m_pEngine && m_pDXRenderPipeline && m_pDXRenderPipeline->IsInitialized(), S_NOTINIT);	
	SP_ASSERTR(pOutputPlane && pDXGeometrySection, S_INVALIDPARAM);

	if (m_pLightSources == 0)
		return S_SUCCESS; // no light sources, so just finish that up

	if (!m_LightingBuffer.IsInitialized())
		return m_pEngine->LogReport(S_NOTINIT, "Cannot render lighting in lighting render section: lighting buffer is not initialized!");	

	// check if nothing has to be rendered
	if (m_pLightSources->GetUsageSize() == 0)
		return S_SUCCESS;

	// Get the DirectX9 Renderer
	DirectX11Renderer* pDXRenderer = (DirectX11Renderer*)m_pDXRenderPipeline->GetRenderer();
	if (pDXRenderer == 0)
		return m_pEngine->LogReport(S_NOTINIT, "Cannot render lighting in lighting render section: Renderer is not initialized");

	// -----------------------------------------------------------------------------------
	// Setup output plane geometry streams

	if (!pOutputPlane->IsInitialized())
		return m_pEngine->LogReport(S_NOTINIT, "Cannot render lighting in lighting render section: Given output plane is not initialized!");

	if (Failure(pDXRenderer->SetVBStream(pOutputPlane->GetVertexBuffer())))
		return m_pEngine->LogE("Cannot render lighting in lighting render section: Failed to set VB Stream source!");

	if (Failure(pDXRenderer->SetIBStream(pOutputPlane->GetIndexBuffer())))
		return m_pEngine->LogE("Cannot render lighting in lighting render section: Failed to set IB Stream source!");	

	// -----------------------------------------------------------------------------------		
	// Matrices and begin the effect
	// Notice: we don't have to worry about the world matrix here as we use as fixed-in in the shader

	if (Failure(m_LightingEffect.Enable()))
		return S_ERROR;

	pDXRenderer->SetViewportMatrices(pOutputPlane->GetViewMatrix(), pOutputPlane->GetProjectionMatrix());		
	if (Failure(pDXRenderer->UpdateMatrixCB()))
		return S_ERROR;

	// -----------------------------------------------------------------------------------		
	// Textures

	if (Failure(pDXRenderer->BindTexture(pDXGeometrySection->GetAlbedoFBO(), 0)))
		res = m_pEngine->LogE("Failed to set Albedo FBO as texture 0 for lighting shader!");

	if (Failure(pDXRenderer->BindTexture(pDXGeometrySection->GetPositionsFBO(), 1)))
		res = m_pEngine->LogE("Failed to set Positions FBO as texture 1 for lighting shader!");

	if (Failure(pDXRenderer->BindTexture(pDXGeometrySection->GetNormalsFBO(), 2)))
		res = m_pEngine->LogE("Failed to set Normals FBO as texture 2 for lighting shader!");

	if (Failure(pDXRenderer->BindTexture(pDXGeometrySection->GetTangentsFBO(), 3)))
		res = m_pEngine->LogE("Failed to set Normals FBO as texture 3 for lighting shader!");

	if (Failure(res))
		return res;

	// -----------------------------------------------------------------------------------	
	// Make sure that we set up proper render states

	pDXRenderer->EnableBackfaceCulling(false);
	pDXRenderer->UpdatePolygonType(S_PRIM_COMPLEX);

	// -----------------------------------------------------------------------------------	
	// Now draw
	
	pDXRenderer->GetD3D11DeviceContext()->DrawIndexed(pOutputPlane->GetIndexCount(), 0, 0);

	// -----------------------------------------------------------------------------------		
	// Reset unused textures

	pDXRenderer->BindTexture(static_cast<ITexture*>(nullptr), 1);
	pDXRenderer->BindTexture(static_cast<ITexture*>(nullptr), 2);
	pDXRenderer->BindTexture(static_cast<ITexture*>(nullptr), 3);	

	return res;
}



SP_NMSPACE_END