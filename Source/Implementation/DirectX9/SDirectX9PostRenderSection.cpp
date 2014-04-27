// ********************************************************************************************

//	This file is part of the SpeedPoint Game Engine

//	(c) 2011-2014 Pascal R. aka iSmokiieZz
//	All rights reserved.

// ********************************************************************************************

#include <Implementation\DirectX9\SDirectX9PostRenderSection.h>
#include <Implementation\DirectX9\SDirectX9LightingRenderSection.h>
#include <Implementation\DirectX9\SDirectX9Renderer.h>
#include <Implementation\DirectX9\SDirectX9Utilities.h>
#include <Abstract\Pipelines\SRenderPipeline.h>
#include <Abstract\SViewport.h>
#include <Abstract\SRenderer.h>
#include <SSpeedPointEngine.h>
#include <Util\SVertex.h>

namespace SpeedPoint
{
	// ********************************************************************************************

	S_API SDirectX9PostRenderSection::~SDirectX9PostRenderSection()
	{
		Clear();
	}

	// ********************************************************************************************

	S_API SResult SDirectX9PostRenderSection::Initialize(SpeedPointEngine* pEngine, SRenderPipeline* pRenderPipeline)
	{
		if ((m_pEngine = pEngine) == 0) return S_INVALIDPARAM;		

		if (pRenderPipeline == 0 || !pRenderPipeline->IsInitialized())
			return m_pEngine->LogReport(S_INVALIDPARAM, "Cannot initialize post render section: given render pipeline is zero or not initialized!");

		if (pRenderPipeline->GetRenderer() == 0 || pRenderPipeline->GetRenderer()->GetType() != S_DIRECTX9)
			return m_pEngine->LogReport(S_NOTINIT, "Cannot initialize post render section: renderer is not initialized or invalid");

		m_pDX9RenderPipeline = (SDirectX9RenderPipeline*)pRenderPipeline;

		// -----------------------------------------------------------------------------------
		// Initialize the effect

		if (Failure(m_PostShader.Initialize(m_pEngine, "Effects\\post.fx")))
		{
			return m_pEngine->LogE("Failed initialize post render section: Could not initialize post shader!");
		}

		// -----------------------------------------------------------------------------------
		// Initialize output plane
		
		SIZE szVPSize = pRenderPipeline->GetTargetViewport()->GetSize();		
		if (Failure(m_OutputPlane.Initialize(pEngine, pRenderPipeline->GetRenderer(), szVPSize.cx, szVPSize.cy)))
		{
			return pEngine->LogReport(S_ERROR, "Failed initialize Output plane");
		}
	
		return S_SUCCESS;
	}

	// ********************************************************************************************

	S_API SResult SDirectX9PostRenderSection::Clear()
	{
		m_OutputPlane.Clear();

		m_pEngine = 0;
		m_pDX9RenderPipeline = 0;

		return S_SUCCESS;
	}

	// ********************************************************************************************

	S_API SResult SDirectX9PostRenderSection::RenderOutputPlane(SDirectX9LightingRenderSection* pLightingSection)
	{
		SP_ASSERTR(!m_pEngine, S_NOTINIT);
		SP_ASSERTXR(!pLightingSection, S_INVALIDPARAM, m_pEngine, "Given lighting section is invalid!");							
		SP_ASSERTXR(!m_pDX9RenderPipeline, S_NOTINIT, m_pEngine);

		if (!m_PostShader.IsInitialized())
			return m_pEngine->LogReport(S_NOTINIT, "Cannot render post output plane: post shader is not initialized!");

		SDirectX9FrameBuffer* pLightBuffer = pLightingSection->GetLightingBuffer();
		SP_ASSERTXR((pLightBuffer == 0
			|| pLightBuffer->pTexture == 0
			|| !m_OutputPlane.vertexBuffer.IsInited()
			|| !m_OutputPlane.indexBuffer.IsInited()),
			S_NOTINIT, m_pEngine, "Light buffer not initialized!");

		SDirectX9Renderer* pDXRenderer = (SDirectX9Renderer*)((SRenderPipeline*)m_pDX9RenderPipeline)->GetRenderer();
		SP_ASSERTXR(!pDXRenderer, S_NOTINIT, m_pEngine, "Renderer is zero!");

		// -----------------------------------------------------------------------------------

		// Set the backbuffer or swap chain as render target
		SViewport* pViewport = pDXRenderer->GetTargetViewport();		
		IDirect3DSurface9* pSurface;
		bool bIsAddition;
		if ((bIsAddition = pViewport->IsAddition()))
			((SDirectX9Viewport*)pViewport)->pSwapChain->GetBackBuffer(0, D3DBACKBUFFER_TYPE_MONO, &pSurface);					
		else
			pSurface = ((SDirectX9FrameBuffer*)pDXRenderer->GetTargetViewport()->GetBackBuffer())->pSurface;

		SP_ASSERTDXR(!pSurface, S_ERROR, m_pEngine, "Failed get Surface! IsAddition=%d", (int)bIsAddition);

		if (FAILED(pDXRenderer->pd3dDevice->SetRenderTarget(0, pSurface)))
			return m_pEngine->LogE("Cannot render post output plane: Failed to set backbuffer as render target 0!");

		// -
		// Do NOT clear the backbuffer here! This is done in the RenderPipeline in DoBeginRendering()
		// The reason for that is, that the latency between Clear-backbuffer and draw calls should be as large as possible.
		// -

		// Setup output plane geometry stream		
		if (FAILED(pDXRenderer->pd3dDevice->SetStreamSource(0, m_OutputPlane.vertexBuffer.pHWVertexBuffer, 0, sizeof(SVertex)))
			|| FAILED(pDXRenderer->pd3dDevice->SetIndices(m_OutputPlane.indexBuffer.pHWIndexBuffer)))
		{
			return m_pEngine->LogE("Cannot render post output plane: Failed to set geometry streams!");
		}

		// Begin the effect
		usint32 nPasses;
		LPD3DXEFFECT pPostEffect = m_PostShader.GetEffect();

		if (FAILED(pPostEffect->SetTechnique("PostTechnique"))
			|| FAILED(pPostEffect->Begin(&nPasses, 0)))			// Begins the effect here
		{
			return m_pEngine->LogE("Cannot render post output plane: Failed to set technique or begin effect!");
		}

		// Matrices
		SResult res = S_SUCCESS;

		D3DXMATRIX mtxProjection = SMatrixToDXMatrix(m_OutputPlane.mProjection);
		if (FAILED(pPostEffect->SetMatrix("mtxProjection", &mtxProjection)))
			res = m_pEngine->LogE("Failed to set projection matrix for post shader!");

		D3DXMATRIX mtxView = SMatrixToDXMatrix(m_OutputPlane.mView);
		if (FAILED(pPostEffect->SetMatrix("mtxView", &mtxView)))		
			res = m_pEngine->LogE("Failed to set view matrix for post shader!");

		// World matrix is obsolete

		if (Failure(res))
		{
			pPostEffect->End();
			return res;
		}

		// Set the texture
		if (FAILED(pDXRenderer->pd3dDevice->SetTexture(0, pLightingSection->GetLightingBuffer()->pTexture)))
			res = m_pEngine->LogE("Failed to set lighting buffer as texture in post render section!");

		if (Failure(res))
		{
			pPostEffect->End();
			return res;
		}

		// Now draw the polygons of the output plane
		for (usint32 iPass = 0; iPass < nPasses; ++iPass)
		{
			if (FAILED(pPostEffect->BeginPass(iPass)))
				res = m_pEngine->LogE("Failed to begin pass in post render section!");

			if (Success(res) && FAILED(pDXRenderer->pd3dDevice->DrawIndexedPrimitive(D3DPT_TRIANGLELIST, 0, 0, 11 * 11, 0, 10 * 10 * 3)))
				res = m_pEngine->LogE("Failed to Draw indexed primitive in post render section!");

			if (FAILED(pPostEffect->EndPass()))
				res = m_pEngine->LogE("Failed to end pass in post render section!");

			if (Failure(res)) break;
		}

		// End the post effect
		if (FAILED(pPostEffect->End()))
			res = m_pEngine->LogE("Failed to end post effect in post render section!");

		// Reset texture
		if (FAILED(pDXRenderer->pd3dDevice->SetTexture(0, 0)))
			res = m_pEngine->LogE("Failed to reset texture in post render section!");

		return res;
	}
}