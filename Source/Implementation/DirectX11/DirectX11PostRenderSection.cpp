//////////////////////////////////////////////////////////////////////////////////////////////////
//
//	This file is part of the SpeedPoint Game Engine
//
//	written by Pascal R. aka iSmokiieZz
//	(c) 2011-2014, All rights reserved.
//
//////////////////////////////////////////////////////////////////////////////////////////////////

#include <Implementation\DirectX11\DirectX11PostRenderSection.h>
#include <Implementation\DirectX11\DirectX11LightingRenderSection.h>
#include <Implementation\DirectX11\DirectX11Renderer.h>
#include <Implementation\DirectX11\DirectX11Utilities.h>
#include <Abstract\Pipelines\IRenderPipeline.h>
#include <Abstract\IViewport.h>
#include <Abstract\IRenderer.h>
#include <SpeedPointEngine.h>
#include <Util\SVertex.h>
#include <DxErr.h>

namespace SpeedPoint
{
	// ********************************************************************************************

	S_API DirectX11PostRenderSection::~DirectX11PostRenderSection()
	{
		Clear();
	}

	// ********************************************************************************************

	S_API SResult DirectX11PostRenderSection::Initialize(SpeedPointEngine* pEngine, IRenderPipeline* pRenderPipeline)
	{
		if ((m_pEngine = pEngine) == 0) return S_INVALIDPARAM;		

		if (pRenderPipeline == 0 || !pRenderPipeline->IsInitialized())
			return m_pEngine->LogReport(S_INVALIDPARAM, "Cannot initialize post render section: given render pipeline is zero or not initialized!");

		if (pRenderPipeline->GetRenderer() == 0 || pRenderPipeline->GetRenderer()->GetType() != S_DIRECTX9)
			return m_pEngine->LogReport(S_NOTINIT, "Cannot initialize post render section: renderer is not initialized or invalid");

		m_pDXRenderPipeline = (DirectX11RenderPipeline*)pRenderPipeline;

		// -----------------------------------------------------------------------------------
		// Initialize the effect

		char* pPostFXFile = 0;
#ifdef _DEBUG
		pPostFXFile = new char[500];
		sprintf_s(pPostFXFile, 500, "%s..\\Effects\\post.fx", SOL_DIR);
#else
		pPostFXFile = "Effects\\post.fx";
#endif

		if (Failure(m_PostShader.Initialize(m_pEngine, pPostFXFile)))
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

	S_API SResult DirectX11PostRenderSection::Clear()
	{
		m_OutputPlane.Clear();

		m_pEngine = 0;
		m_pDXRenderPipeline = 0;

		return S_SUCCESS;
	}

	// ********************************************************************************************

	S_API SResult DirectX11PostRenderSection::RenderOutputPlane(DirectX11LightingRenderSection* pLightingSection)
	{
		SResult res;
		SP_ASSERTR(m_pEngine, S_NOTINIT);
		SP_ASSERTXR(pLightingSection, S_INVALIDPARAM, m_pEngine, "Given lighting section is invalid!");							
		SP_ASSERTXR(m_pDXRenderPipeline, S_NOTINIT, m_pEngine);

		if (!m_PostShader.IsInitialized())
			return m_pEngine->LogReport(S_NOTINIT, "Cannot render post output plane: post shader is not initialized!");

		DirectX11FBO* pLightBuffer = pLightingSection->GetLightingBuffer();
		SP_ASSERTXR(pLightBuffer
			&& pLightBuffer->IsInitialized()
			&& m_OutputPlane.IsInitialized(),
			S_NOTINIT, m_pEngine, "Light buffer not initialized!");

		DirectX11Renderer* pDXRenderer = (DirectX11Renderer*)((IRenderPipeline*)m_pDXRenderPipeline)->GetRenderer();
		SP_ASSERTXR(pDXRenderer, S_NOTINIT, m_pEngine, "Renderer is zero!");

		// -----------------------------------------------------------------------------------
		
		pDXRenderer->SetRenderTarget(pDXRenderer->GetTargetViewport());



		// ??????



		// IS THIS CORRECT ?????

		// -
		// Do NOT clear the backbuffer here! This is done in the RenderPipeline in DoBeginRendering()
		// The reason for that is, that the latency between Clear-backbuffer and draw calls should be as large as possible.
		// -		




		// ?????????


		// Setup output plane geometry stream		
		pDXRenderer->SetVBStream(m_OutputPlane.GetVertexBuffer(), 0);
		pDXRenderer->SetIBStream(m_OutputPlane.GetIndexBuffer());		

		// Enable the effect
		m_PostShader.Enable();

		// Matrices		
		// Notice that we don't have to worry about the world matrix as this is a fixed one in the shader
		pDXRenderer->SetViewportMatrices(m_OutputPlane.GetViewMatrix(), m_OutputPlane.GetProjectionMatrix());		

		res = pDXRenderer->UpdateMatrixCB();
		if (Failure(res))
			return S_ERROR;


		// Set the texture
		res = pDXRenderer->BindTexture(pLightingSection->GetLightingBuffer(), 0);
		if (Failure(res))
			return S_ERROR;

		// Now draw the polygons of the output plane
		pDXRenderer->GetD3D11DeviceContext()->DrawIndexed(m_OutputPlane.GetIndexCount(), 0, 0);



		// We don't have to reset the texture as it is probably overridden in the next render section

		return res;
	}
}