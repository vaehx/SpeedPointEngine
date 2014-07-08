//////////////////////////////////////////////////////////////////////////////////////////////////
//
//	This file is part of the SpeedPoint Game Engine
//
//	written by Pascal R. aka iSmokiieZz
//	(c) 2011-2014, All rights reserved.
//
//////////////////////////////////////////////////////////////////////////////////////////////////

#include <Implementation\DirectX11\DirectX11GeometryRenderSection.h>
#include <Implementation\DirectX11\DirectX11Renderer.h>
#include <Implementation\DirectX11\DirectX11RenderPipeline.h>
#include <Implementation\DirectX11\DirectX11FBO.h>
#include <Implementation\DirectX11\DirectX11VertexBuffer.h>
#include <Implementation\DirectX11\DirectX11IndexBuffer.h>
#include <Implementation\DirectX11\DirectX11Texture.h>
#include <Implementation\DirectX11\DirectX11Utilities.h>
#include <Abstract\ISolid.h>
#include <Abstract\ITexture.h>
#include <Util\SMaterial.h>
#include <Util\SVertex.h>
#include <Util\SPrimitive.h>
#include <SSpeedPointEngine.h>

SP_NMSPACE_BEG

// -------------------------------------------------------------------
S_API DirectX11GeometryRenderSection::DirectX11GeometryRenderSection()
	: m_pEngine(0),
	m_pDXRenderPipeline(0)
{
}

// -------------------------------------------------------------------
S_API DirectX11GeometryRenderSection::~DirectX11GeometryRenderSection()
{
	Clear();
}

// -------------------------------------------------------------------
S_API SResult DirectX11GeometryRenderSection::Initialize(SpeedPointEngine* eng, IRenderPipeline* pRenderPipeline)
{	
	SP_ASSERTR(eng, S_INVALIDPARAM);
	SP_ASSERTR((pRenderPipeline == 0
		|| pRenderPipeline->GetRenderer() == 0
		|| pRenderPipeline->GetRenderer()->GetType() != S_DIRECTX11), S_INVALIDPARAM);

	// --------------------------------------------------------------------------------------------
	// Assign pointers
	IRenderer* pRenderer;
	DirectX11Renderer* pDXRenderer;

	m_pEngine = eng;
	m_pDXRenderPipeline = (DirectX11RenderPipeline*)pRenderPipeline;
	pRenderer = m_pDXRenderPipeline->GetRenderer();
	pDXRenderer = (DirectX11Renderer*)pRenderer;

	// --------------------------------------------------------------------------------------------
	// Initialize the Frame-Buffers (GBuffer components)
	SSettings settings = m_pEngine->GetSettings();
	SResult res = S_SUCCESS, tempRes;

	usint32 nXRes = settings.app.nXResolution, nYRes = settings.app.nYResolution;

	SP_ASSERTR(m_GBufferAlbedo.Initialize(eFBO_GBUFFER_ALBEDO, m_pEngine, pRenderer, nXRes, nYRes), S_ERROR);
	SP_ASSERTR(m_GBufferPosition.Initialize(eFBO_GBUFFER_POSITION, m_pEngine, pRenderer, nXRes, nYRes), S_ERROR);	
	SP_ASSERTR(m_GBufferNormals.Initialize(eFBO_GBUFFER_NORMALS, m_pEngine, pRenderer, nXRes, nYRes), S_ERROR);	
	SP_ASSERTR(m_GBufferTangents.Initialize(eFBO_GBUFFER_TANGENTS, m_pEngine, pRenderer, nXRes, nYRes), S_ERROR);

	// Setup the render target collection
	SP_ASSERTR(pDXRenderer->AddBindedFBO(0, (IFBO*)&m_GBufferAlbedo), S_ERROR);
	SP_ASSERTR(pDXRenderer->AddBindedFBO(1, (IFBO*)&m_GBufferPosition), S_ERROR);
	SP_ASSERTR(pDXRenderer->AddBindedFBO(2, (IFBO*)&m_GBufferNormals), S_ERROR);
	SP_ASSERTR(pDXRenderer->AddBindedFBO(3, (IFBO*)&m_GBufferTangents), S_ERROR);	
	SP_ASSERTR(pDXRenderer->BindFBOs(eRENDERTARGETS_GBUFFER, true), S_ERROR);

	// --------------------------------------------------------------------------------------------
	// Initialize the GBuffer effect

	char* pGBufferFXFile = 0;
#ifdef _DEBUG
	pGBufferFXFile = new char[500];
	sprintf_s(pGBufferFXFile, 500, "%s..\\Effects\\gbuffer.fx", SOL_DIR);
#else
	pGBufferFXFile = "Effects\\gbuffer.fx";
#endif

	if (Failure(m_gBufferShader.Initialize(m_pEngine, pGBufferFXFile)))
		return m_pEngine->LogE("Failed to load GBuffer creation effect");


	// --------------------------------------------------------------------------------------------
	// Initialize the constants buffer

	if (Failure(pDXRenderer->D3D11_CreateConstantsBuffer(&m_pConstantsBuffer, sizeof(SDefMtxCB))))
		return S_ERROR;

	return res;
}

// -------------------------------------------------------------------
S_API SResult DirectX11GeometryRenderSection::Clear(void)
{
	m_GBufferAlbedo.Clear();
	m_GBufferPosition.Clear();
	m_GBufferNormals.Clear();
	m_GBufferTangents.Clear();

	m_pEngine = 0;
	m_pDXRenderPipeline = 0;

	return S_SUCCESS;
}

// -------------------------------------------------------------------
S_API SResult DirectX11GeometryRenderSection::PrepareSection()
{
	SP_ASSERTR(m_pEngine && m_pDXRenderPipeline && m_pDXRenderPipeline->IsInitialized(), S_NOTINIT);
	
	DirectX11Renderer* pDXRenderer = (DirectX11Renderer*)m_pDXRenderPipeline->GetRenderer();

	
	// Check ptrs in advance
	if (!m_GBufferAlbedo.IsInitialized()) return m_pEngine->LogReport(S_NOTINIT, "Could not prepare Geometry Render Section: Albedo Framebuffer not initialized!");
	if (!m_GBufferPosition.IsInitialized()) return m_pEngine->LogReport(S_NOTINIT, "Could not prepare Geometry Render Section: Position Framebuffer not initialized!");
	if (!m_GBufferNormals.IsInitialized()) return m_pEngine->LogReport(S_NOTINIT, "Could not prepare Geometry Render Section: Normals Framebuffer not initialized!");
	if (!m_GBufferTangents.IsInitialized()) return m_pEngine->LogReport(S_NOTINIT, "Could not prepare Geometry Render Section: Tangents Framebuffer not initialized!");
	if (!m_gBufferShader.IsInitialized()) return m_pEngine->LogReport(S_ABORTED, "Cannot prepare Geometry Section: GBuffer creation shader not initilized!");
	
	// Set the render targets
	if (Failure(pDXRenderer->BindFBOs(eRENDERTARGETS_GBUFFER)))
	{
		return S_ERROR;
	}
	
	// Clear the frame buffer objects	
	if (!pDXRenderer->ClearRenderTargets())
	{
		return m_pEngine->LogReport(S_ERROR, "Could not clear GBuffer components properly!");
	}

	return S_SUCCESS;
}

// **************************************************************************************************************************

S_API SResult DirectX11GeometryRenderSection::EndSection()
{
	SP_ASSERTR(m_pEngine && m_pDXRenderPipeline && m_pDXRenderPipeline->IsInitialized(), S_NOTINIT);


	// In DX11 we don't have to worry about resetting the RTs as we can only call ID3D11DeviceContext::OMSetRenderTargets()
	// which does not take any argument to reset an RT...


	return S_SUCCESS;
}

// -------------------------------------------------------------------
S_API SResult DirectX11GeometryRenderSection::PrepareShaderInput(ISolid* pSolid, bool bTextured)
{
	HRESULT hRes;
	DirectX11Renderer* pDXRenderer;
	DirectX11VertexBuffer* pDXVertexBuffer;
	DirectX11IndexBuffer* pDXIndexBuffer;

	SP_ASSERTR(m_pEngine, S_NOTINIT);
	SP_ASSERTXR(m_pDXRenderPipeline && m_pDXRenderPipeline->IsInitialized(), S_NOTINIT, m_pEngine, "Renderer not initialized!");
	SP_ASSERTXR(pSolid, S_INVALIDPARAM, m_pEngine, "Cannot Prepare GBuffer Creation shader: The given solid ptr is zero.");

	pDXRenderer = (DirectX11Renderer*)m_pDXRenderPipeline->GetRenderer();


	// Assign the current solid for which the shader inputs are going to be configured
	m_pCurrentSolid = pSolid;	

	// -----------------------------------------------------------------------------------------------------------------
	// Setup the data sources
	
	if (Failure(pDXRenderer->SetVBStream(m_pEngine->GetResourcePool()->GetVertexBuffer(pSolid->GetVertexBuffer()))))
		return S_ERROR;

	if (Failure(pDXRenderer->SetIBStream(m_pEngine->GetResourcePool()->GetIndexBuffer(pSolid->GetIndexBuffer()))))
		return S_ERROR;	


	// -----------------------------------------------------------------------------------------------------------------
	// Enable the proper shaders
	
	if (Failure(m_gBufferShader.Enable()))	// Will set the shaders and the input layout
		return S_ERROR;

	// --------------------------------------------------------------------------------------------
	// Update constants buffer

	if (Failure(pDXRenderer->SetViewportMatrices(m_pDXRenderPipeline->GetTargetViewport())))
		return S_ERROR;

	if (Failure(pDXRenderer->SetWorldMatrix((STransformable*)pSolid)))
		return S_ERROR;

	if (Failure(pDXRenderer->UpdateMatrixCB()))
		return S_ERROR;



	// TODO: Material Constants Buffer?
	


	// Now its time to throw the render calls!
	return S_SUCCESS;
}

// -------------------------------------------------------------------
S_API SResult DirectX11GeometryRenderSection::RenderSolidGeometry(ISolid* pSolid, bool bTextured)
{
	SPrimitive* pPrimitive;

	SP_ASSERTR(m_pEngine && m_pDXRenderPipeline && m_pDXRenderPipeline->IsInitialized(), S_NOTINIT);	
	SP_ASSERTXR((m_pCurrentSolid = pSolid), S_NOTINIT, m_pEngine, "Current solid is empty!");

	// -----------------------------------------------------------------------------------------------------------------------
	// Check if solid has to be rendered

	SCamera* pCamera = m_pDXRenderPipeline->GetTargetViewport()->GetCamera();
	float fMinDist = pCamera->GetViewRadius();

	if (SVector3(pSolid->GetPosition() - pCamera->GetPosition()).Square() > fMinDist)
	{
		return S_SUCCESS; // Does not need to be drawn, so break as early as possible
	}

	// -----------------------------------------------------------------------------------------------------------------------
	// Render textured primitives	

	if (Failure(PrepareShaderInput(pSolid, true))) return S_ERROR;

	// -----------------------------------------------------------------------------------------------------------------------	

	SP_ASSERTXR(m_pCurrentSolid->GetPrimitiveCount() > 0, S_INVALIDPARAM, m_pEngine, "Primitive count of solid equals 0!");	

	// -----------------------------------------------------------------------------------------------------------------------
	// now draw all objects per shader pass	

	for (usint32 iPrimitive = 0; iPrimitive < (usint32)m_pCurrentSolid->GetPrimitiveCount(); ++iPrimitive)
	{

		//////////////////////////////////
		///////// TODO: Minimize primitives to be drawn, if they are not visible by camera!
		/////////////////////////////////

		pPrimitive = m_pCurrentSolid->GetPrimitive(iPrimitive);
		SP_ASSERTXR(pPrimitive, S_ERROR, "Got Nullpointer as primitive to be drawn!");

		if (!pPrimitive->bDraw) continue;	

		// Render the primitive
		if (Failure(RenderTexturedPrimitive(iPrimitive)))
			return S_ERROR;

	}

	// -----------------------------------------------------------------------------------------------------------------------
	// we need to free the shader input, before we can switch the technique

	if (Failure(FreeShaderInput())) return S_ERROR;

	// -----------------------------------------------------------------------------------------------------------------------		

	return S_SUCCESS;
}

// -------------------------------------------------------------------
S_API SResult DirectX11GeometryRenderSection::RenderTexturedPrimitive(UINT iPrimitive)
{
	if (m_pEngine == 0) return S_NOTINIT;

	// --------------------------------------------------------------------------------------------

	if (m_pDXRenderPipeline == 0 || !m_pDXRenderPipeline->IsInitialized())
		return m_pEngine->LogReport(S_ABORTED, "Cannot render textured primitive: Renderer not initilized!");

	if (m_pCurrentSolid == 0)
		return m_pEngine->LogReport(S_ABORTED, "Cannot render textured primitive: Current Solid not initialized!");

	if (iPrimitive > m_pCurrentSolid->GetPrimitiveCount())
		return m_pEngine->LogReport(S_INVALIDPARAM, "Cannot render textured primitive: given id exceeds primitive count of solid!");

	// Get the DirectX9 Renderer
	DirectX11Renderer* pDXRenderer = (DirectX11Renderer*)m_pDXRenderPipeline->GetRenderer();

	// --------------------------------------------------------------------------------------------
	// Get the primitive

	SPrimitive* pPrimitive = m_pCurrentSolid->GetPrimitive(iPrimitive);

	if (!pPrimitive->bDraw) return S_SUCCESS;

	// --------------------------------------------------------------------------------------------
	// Setup the texture

	ITexture* pTexture;

	if (0 == (pTexture = m_pEngine->GetResourcePool()->GetTexture(pPrimitive->iTexture)))
		return m_pEngine->LogE("Failed to retrieve texture of textured primitive!");

	DirectX11Texture* pDXTexture = (DirectX11Texture*)pTexture;
	if (FAILED(pDXRenderer->pd3dDevice->SetTexture(0, pDXTexture->pTexture)))
		return m_pEngine->LogE("Failed to set the texture of textured primitive!");

	// --------------------------------------------------------------------------------------------
	// Draw the actual geometry

	return RenderPrimitiveGeometry(pPrimitive);
}

// -------------------------------------------------------------------
S_API SResult DirectX11GeometryRenderSection::RenderUntexturedPrimitive(UINT iPrimitive)
{
	return S_NOTIMPLEMENTED;
}

// -------------------------------------------------------------------
S_API SResult DirectX11GeometryRenderSection::RenderPrimitiveGeometry(SPrimitive* pPrimitive)
{
	if (m_pEngine == NULL) return S_ABORTED;

	if (m_pDX9RenderPipeline == 0 || !m_pDX9RenderPipeline->IsInitialized())
		return m_pEngine->LogReport(S_ABORTED, "Cannot render primitive geometry: renderer not initialized!");

	if (pPrimitive == NULL)
		return m_pEngine->LogReport(S_ABORTED, "Cannot render primitive geometry: given primitive not initialized!");

	// Get the DirectX9 Renderer
	SDirectX9Renderer* pDXRenderer = (SDirectX9Renderer*)m_pDX9RenderPipeline->GetRenderer();

	// Get the effect
	LPD3DXEFFECT pGBufferEffect = m_gBufferShader.GetEffect();

	// --------------------------------------------------------------------------------------------
	// Setup the proper Cullmode

	DWORD dwCullMode = D3DCULL_CW;
	if (pPrimitive->tType == S_PRIM_PARTICLE || pPrimitive->tType == S_PRIM_COMPLEX_PLANE)
	{
		dwCullMode = D3DCULL_NONE;
	}

	if (FAILED(pDXRenderer->pd3dDevice->SetRenderState(D3DRS_CULLMODE, dwCullMode)))
	{
		pGBufferEffect->EndPass(); // pass started by Renderpipeline
		pGBufferEffect->End();
		return m_pEngine->LogReport(S_ERROR, "Failed set CullMode render state while rendering a solid inside the pipeline");
	}

	// --------------------------------------------------------------------------------------------
	// Convert the polygon type

	D3DPRIMITIVETYPE ptType = D3DPT_TRIANGLELIST;
	switch (pPrimitive->tRenderType)
	{
	case S_PRIM_RENDER_TRIANGLELIST: ptType = D3DPT_TRIANGLELIST; break;
	case S_PRIM_RENDER_TRIANGLESTRIP: ptType = D3DPT_TRIANGLESTRIP; break;
	case S_PRIM_RENDER_LINELIST: ptType = D3DPT_LINELIST; break;
	case S_PRIM_RENDER_POINTLIST: ptType = D3DPT_POINTLIST; break;
	}

	// --------------------------------------------------------------------------------------------
	// Now draw the polygons

	if (FAILED(pDXRenderer->pd3dDevice->DrawIndexedPrimitive(
		ptType,								// Polygon type
		pPrimitive->iFirstVertex,					// index of first vertex
		0,								// minimum index of the vertex buffer
		(pPrimitive->iLastVertex - pPrimitive->iFirstVertex + 1),	// Count of vertices available
		pPrimitive->iFirstIndex,					// the first index to be drawn
		(UINT)pPrimitive->nPolygons)))					// Count of polygons in the index buffer to be drawn
	{
		pGBufferEffect->EndPass(); // pass started by renderPipeline
		pGBufferEffect->End();

		return m_pEngine->LogReport(S_ERROR, "Failed draw Polygons inside render pipeline!");
	}

	// --------------------------------------------------------------------------------------------

	return S_SUCCESS;
}

// -------------------------------------------------------------------
S_API SResult DirectX11GeometryRenderSection::FreeShaderInput(void)
{
	SP_ASSERTR(m_pEngine, S_ERROR);
	SP_ASSERTXR(m_pDX9RenderPipeline && m_pDX9RenderPipeline->IsInitialized(), S_NOTINIT, "Renderer not initialized!");

	// --------------------------------------------------------------------------------------------
	// Exit the shader

	if (FAILED(m_gBufferShader.GetEffect()->End()))
		return m_pEngine->LogReport(S_ERROR, "Failed to exit gBuffer creation shader!");

	// --------------------------------------------------------------------------------------------

	return S_SUCCESS;
}

}