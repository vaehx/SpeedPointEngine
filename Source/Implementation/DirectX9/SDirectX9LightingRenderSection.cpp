// ********************************************************************************************

//	This file is part of the SpeedPoint Game Engine

// ********************************************************************************************

#include <Implementation\DirectX9\SDirectX9LightingRenderSection.h>
#include <Implementation\DirectX9\SDirectX9RenderPipeline.h>
#include <Implementation\DirectX9\SDirectX9Renderer.h>
#include <Implementation\DirectX9\SDirectX9OutputPlane.h>
#include <Implementation\DirectX9\SDirectX9Utilities.h>
#include <Abstract\Pipelines\SRenderPipeline.h>
#include <Abstract\SRenderer.h>
#include <SSpeedPointEngine.h>
#include <Util\SVertex.h>

namespace SpeedPoint
{
	// ********************************************************************************************
	
	S_API SDirectX9LightingRenderSection::SDirectX9LightingRenderSection()
		: m_pEngine(0),
		m_pDX9RenderPipeline(0),
		m_pLightSources(0)
	{
	}

	// ********************************************************************************************

	S_API SDirectX9LightingRenderSection::~SDirectX9LightingRenderSection()
	{
		Clear();
	}

	// ********************************************************************************************
	
	S_API SResult SDirectX9LightingRenderSection::Initialize(SpeedPointEngine* pEngine, SRenderPipeline* pRenderPipeline)
	{
		if (pEngine == 0 || pRenderPipeline == 0)
			return S_INVALIDPARAM;

		if (pRenderPipeline->GetRenderer() == 0 || pRenderPipeline->GetRenderer()->GetType() != S_DIRECTX9)
			return pEngine->LogReport(S_NOTINIT, "Could not initialize Lighting Render Section: Renderer invalid or not initialized!");

		// -----------------------------------------------------------------------------------

		m_pEngine = pEngine;
		m_pDX9RenderPipeline = (SDirectX9RenderPipeline*)pRenderPipeline;

		// Prepare Lighting buffer
		SSettings settings = pEngine->GetSettings();
		if (Failure(m_LightingBuffer.Initialize(m_pEngine, settings.nXResolution, settings.nYResolution)))
			return m_pEngine->LogE("Cannot initialize lighting render section: failed to initialize lighting output buffer");

		return S_SUCCESS;
	}

	// ********************************************************************************************

	S_API SResult SDirectX9LightingRenderSection::Clear(void)
	{
		FreeLightSources();

		m_LightingBuffer.Clear();
		m_LightingShader.Clear();		
		m_pEngine = 0;
		m_pDX9RenderPipeline = 0;		

		return S_SUCCESS;
	}

	// ********************************************************************************************

	S_API SResult SDirectX9LightingRenderSection::PrepareSection()
	{
		if (m_pEngine == 0) return S_NOTINIT;

		if (m_pDX9RenderPipeline == 0 || !m_pDX9RenderPipeline->IsInitialized())
			return m_pEngine->LogReport(S_NOTINIT, "Cannot prepare lighting render section: Render pipeline not initialized!");

		// Get the DX Renderer
		SDirectX9Renderer* pDXRenderer = (SDirectX9Renderer*)m_pDX9RenderPipeline->GetRenderer();
		if (pDXRenderer == 0)
			return m_pEngine->LogReport(S_NOTINIT, "Cannot prepare lighting render section: Renderer not initialized!");

		// -----------------------------------------------------------------------------------
		// Check ptrs in advance

		if (m_LightingBuffer.pSurface == 0)
			return m_pEngine->LogReport(S_NOTINIT, "Could not prepare lighting render section: Lighting buffer not initialized!");

		if (m_LightingShader.GetEffect() == 0 || !m_LightingShader.IsInitialized())
			return m_pEngine->LogReport(S_NOTINIT, "Could not prepare lighting render section: Lighting shader not initialized!");

		// -----------------------------------------------------------------------------------
		// Set the render target
		// We assume, that the previous render sections properly cleared all render targets with index >0

		if (FAILED(pDXRenderer->pd3dDevice->SetRenderTarget(0, m_LightingBuffer.pSurface)))
			return m_pEngine->LogE("Could not prepare Lighting render section: Failed to Set Lighting Buffer as render target 0!");		

		// -----------------------------------------------------------------------------------
		// Clear the frame buffer object
		
		if (FAILED(pDXRenderer->pd3dDevice->Clear(0, NULL, D3DCLEAR_TARGET, D3DCOLOR_XRGB(0, 0, 0), 1.0f, 0)))
			return m_pEngine->LogE("Could not prepare Lighting render section: Cannot clear lighting frame buffer object!");
		
		return S_SUCCESS;
	}

	// ********************************************************************************************

	S_API SResult SDirectX9LightingRenderSection::EndSection()
	{
		if (m_pEngine == 0) return S_NOTINIT;

		if (m_pDX9RenderPipeline == 0 || !m_pDX9RenderPipeline->IsInitialized())
			return m_pEngine->LogReport(S_NOTINIT, "Cannot end lighting render section: Render pipeline not initialized!");

		// Get the DX Renderer
		SDirectX9Renderer* pDXRenderer = (SDirectX9Renderer*)m_pDX9RenderPipeline->GetRenderer();
		if (pDXRenderer == 0)
			return m_pEngine->LogReport(S_NOTINIT, "Cannot end lighting render section: Renderer not initialized!");

		// -----------------------------------------------------------------------------------

		// Reset the render target
		if (FAILED(pDXRenderer->pd3dDevice->SetRenderTarget(0, 0)))
			return m_pEngine->LogE("Could not end lighting render section: Failed to reset render target 0!");
		
		return S_SUCCESS;
	}

	// ********************************************************************************************

	S_API SResult SDirectX9LightingRenderSection::PrepareLightSources(SPool<SLight*>** pLightSources)
	{
		if (m_pEngine == 0) return S_NOTINIT;

		if (m_pDX9RenderPipeline == 0 || !m_pDX9RenderPipeline->IsInitialized())
			return m_pEngine->LogReport(S_NOTINIT, "Cannot prepare light sources in lighting render section: Render pipeline not initialized!");		

//~~~~~~~~~~~~~
// TODO: Increase performance by checking if light source buffer has to be updated at all
		if (m_pLightSources != 0)
			return m_pEngine->LogReport(S_ABORTED, "Cannot prepare light sources in lighting render section: Last light source buffer not cleared!");
//~~~~~~~~~~~~~

		// -----------------------------------------------------------------------------------
		// Load all default lights if full custom lighting is not requested

		if (!m_pEngine->GetSettings().bCustomLighting)
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

	S_API SResult SDirectX9LightingRenderSection::FreeLightSources()
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

	S_API SResult SDirectX9LightingRenderSection::RenderLighting(SDirectX9GeometryRenderSection* pDXGeometrySection, SDirectX9OutputPlane* pOutputPlane)
	{
		SResult res = S_SUCCESS;

		if (m_pEngine == 0) return S_NOTINIT;

		if (m_pDX9RenderPipeline == 0 || !m_pDX9RenderPipeline->IsInitialized())
			return m_pEngine->LogReport(S_NOTINIT, "Cannot render lighting in lighting render section: Render pipeline not initialized!");

		if (m_pLightSources == 0)
			return m_pEngine->LogReport(S_NOTINIT, "Cannot render lighting in lighting render section: light source buffer is zero!");

		if (m_LightingBuffer.pSurface == 0)
			return m_pEngine->LogReport(S_NOTINIT, "Cannot render lighting in lighting render section: lighting buffer is not initialized!");
		
		if (pOutputPlane == 0)
			return m_pEngine->LogReport(S_INVALIDPARAM, "Cannot render lighting in lighting render section: Given output plane is invalid!");

		if (pDXGeometrySection == 0)
			return m_pEngine->LogReport(S_INVALIDPARAM, "Cannot render lighting in lighting render section: Given Geometry section is invalid!");

		// check if nothing has to be rendered
		if (m_pLightSources->GetUsageSize() == 0)
			return S_SUCCESS;

		// Get the DirectX9 Renderer
		SDirectX9Renderer* pDXRenderer = (SDirectX9Renderer*)m_pDX9RenderPipeline->GetRenderer();
		if (pDXRenderer == 0)
			return m_pEngine->LogReport(S_NOTINIT, "Cannot render lighting in lighting render section: Renderer is not initialized");

		// -----------------------------------------------------------------------------------
		// Setup output plane geometry streams

		if (!pOutputPlane->vertexBuffer.IsInited() || !pOutputPlane->indexBuffer.IsInited())
			return m_pEngine->LogReport(S_NOTINIT, "Cannot render lighting in lighting render section: Given output plane is not initialized!");

		if (FAILED(pDXRenderer->pd3dDevice->SetStreamSource(0, pOutputPlane->vertexBuffer.pHWVertexBuffer, 0, sizeof(SVertex))))
			return m_pEngine->LogE("Cannot render lighting in lighting render section: Failed to set Stream source!");

		if (FAILED(pDXRenderer->pd3dDevice->SetIndices(pOutputPlane->indexBuffer.pHWIndexBuffer)))
			return m_pEngine->LogE("Cannot render lighting in lighting render section: Failed to set Stream source!");

		// -----------------------------------------------------------------------------------
		// Begin the effect

		LPD3DXEFFECT pLightingEffect = m_LightingShader.GetEffect();
		
		if (FAILED(pLightingEffect->SetTechnique("LightingTechnique")))
			return m_pEngine->LogE("Failed to render lighting in lighting render section: Failed to set technique");

		usint32 nPasses; 
		if (FAILED(pLightingEffect->Begin(&nPasses, 0)))
			return m_pEngine->LogE("Failed to render lighting in lighting render section: Failed to Begin effect!");

		// -----------------------------------------------------------------------------------		
		// Matrices

		D3DXMATRIX mtxProjection = SMatrixToDXMatrix(pOutputPlane->mProjection);
		if (FAILED(pLightingEffect->SetMatrix("mtxProjection", &mtxProjection)))		
			res = m_pEngine->LogE("Failed to set projection matrix for lighting shader!");					

		D3DXMATRIX mtxView = SMatrixToDXMatrix(pOutputPlane->mView);
		if (FAILED(pLightingEffect->SetMatrix("mtxView", &mtxView)))
			res = m_pEngine->LogE("Failed to set view matrix for lighting shader!");

		// World matrix is obsolete.

		if (Failure(res))
		{
			pLightingEffect->End();
			return res;
		}

		// -----------------------------------------------------------------------------------		
		// Texture
		
		if (FAILED(pDXRenderer->pd3dDevice->SetTexture(0, pDXGeometrySection->GetAlbedoFBO()->pTexture)))
			res = m_pEngine->LogE("Failed to set Albedo FBO as texture 0 for lighting shader!");

		if (FAILED(pDXRenderer->pd3dDevice->SetTexture(1, pDXGeometrySection->GetPositionsFBO()->pTexture)))
			res = m_pEngine->LogE("Failed to set Positions FBO as texture 1 for lighting shader!");

		if (FAILED(pDXRenderer->pd3dDevice->SetTexture(2, pDXGeometrySection->GetNormalsFBO()->pTexture)))
			res = m_pEngine->LogE("Failed to set Normals FBO as texture 2 for lighting shader!");

		if (FAILED(pDXRenderer->pd3dDevice->SetTexture(3, pDXGeometrySection->GetTangentsFBO()->pTexture)))
			res = m_pEngine->LogE("Failed to set Normals FBO as texture 3 for lighting shader!");

		if (Failure(res))
		{
			pLightingEffect->End();
			return res;
		}

		// -----------------------------------------------------------------------------------		
		// Now draw the polygons of the output plane
		
		for (usint32 iPass = 0; iPass < nPasses; ++iPass)
		{
			if (FAILED(pLightingEffect->BeginPass(iPass)))			
				res = m_pEngine->LogE("Failed to BeginPass of lighting effect in lighting render section!");		

			if (Success(res) && FAILED(pDXRenderer->pd3dDevice->DrawIndexedPrimitive(D3DPT_TRIANGLELIST, 0, 0, 11 * 11, 0, 10 * 10 * 3)))
				res = m_pEngine->LogE("Failed to draw output plane in lighting render section!");

			if (FAILED(pLightingEffect->EndPass()))
				res = m_pEngine->LogE("Failed to end pass of lighting effect in lighting render section!");

			if (Failure(res)) break;
		}

		// -----------------------------------------------------------------------------------		
		// End the lighting shader

		if (FAILED(pLightingEffect->End()))
			res = m_pEngine->LogE("Failed to end lighting effect in lighting render section!");

		// -----------------------------------------------------------------------------------		
		// Reset textures

		if (FAILED(pDXRenderer->pd3dDevice->SetTexture(0, 0)))
			res = m_pEngine->LogE("Failed to set Albedo FBO as texture 0 for lighting shader!");

		if (FAILED(pDXRenderer->pd3dDevice->SetTexture(1, 0)))
			res = m_pEngine->LogE("Failed to set Positions FBO as texture 1 for lighting shader!");

		if (FAILED(pDXRenderer->pd3dDevice->SetTexture(2, 0)))
			res = m_pEngine->LogE("Failed to set Normals FBO as texture 2 for lighting shader!");

		if (FAILED(pDXRenderer->pd3dDevice->SetTexture(3, 0)))
			res = m_pEngine->LogE("Failed to set Normals FBO as texture 3 for lighting shader!");

		return res;
	}

}