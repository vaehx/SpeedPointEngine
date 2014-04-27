// ********************************************************************************************

//	This file is part of the SpeedPoint Game Engine

// ********************************************************************************************

#include <Implementation\DirectX9\SDirectX9GeometryRenderSection.h>
#include <Implementation\DirectX9\SDirectX9Renderer.h>
#include <Implementation\DirectX9\SDirectX9RenderPipeline.h>
#include <Implementation\DirectX9\SDirectX9FrameBuffer.h>
#include <Implementation\DirectX9\SDirectX9VertexBuffer.h>
#include <Implementation\DirectX9\SDirectX9IndexBuffer.h>
#include <Implementation\DirectX9\SDirectX9Texture.h>
#include <Implementation\DirectX9\SDirectX9Utilities.h>
#include <Abstract\SSolid.h>
#include <Util\SMaterial.h>
#include <Util\SVertex.h>
#include <Util\SPrimitive.h>
#include <SSpeedPointEngine.h>

namespace SpeedPoint
{
	// **************************************************************************************************************************

	S_API SDirectX9GeometryRenderSection::SDirectX9GeometryRenderSection()
		: m_pEngine(0),
		m_pDX9RenderPipeline(0)		
	{
	}

	// **************************************************************************************************************************

	S_API SDirectX9GeometryRenderSection::~SDirectX9GeometryRenderSection()
	{
		Clear();
	}

	// **************************************************************************************************************************

	S_API SResult SDirectX9GeometryRenderSection::Initialize( SpeedPointEngine* eng, SRenderPipeline* pRenderPipeline )
	{
		if(eng == 0) return S_INVALIDPARAM;

		if (pRenderPipeline == 0
			|| pRenderPipeline->GetRenderer() == 0
			|| pRenderPipeline->GetRenderer()->GetType() != S_DIRECTX9)
		{
			return eng->LogReport(S_INVALIDPARAM, "Cannot initialize Geometry Render Section: Given Renderer or RenderPipeline is invalid!");
		}

		// --------------------------------------------------------------------------------------------
		// Assign pointers
		m_pEngine		= eng;
		m_pDX9RenderPipeline	= (SDirectX9RenderPipeline*)pRenderPipeline;

		// --------------------------------------------------------------------------------------------
		// Initialize the Frame-Buffers (GBuffer components)
		SSettings settings = m_pEngine->GetSettings();
		SResult res = S_SUCCESS, tempRes;

		if (Failure(tempRes = m_GBufferAlbedo.Initialize(m_pEngine, settings.nXResolution, settings.nYResolution)))
		{
			m_pEngine->LogE("Cannot initialize Geometry Render Section: Failed to Initialize Albedo GBuffer component!");
			if (res == S_SUCCESS) res = tempRes;
		}

		if (Failure(tempRes = m_GBufferPosition.Initialize(m_pEngine, settings.nXResolution, settings.nYResolution)))
		{
			m_pEngine->LogE("Cannot intialize Geometry Render Section: Failed to Initialize Position GBuffer component!");
			if (res == S_SUCCESS) res = tempRes;
		}

		if (Failure(tempRes = m_GBufferNormals.Initialize(m_pEngine, settings.nXResolution, settings.nYResolution)))
		{
			m_pEngine->LogE("Cannot initialize Geometry Render Section: Failed to Initialize normals GBuffer component!");
			if (res == S_SUCCESS) res = tempRes;
		}

		if (Failure(tempRes = m_GBufferTangents.Initialize(m_pEngine, settings.nXResolution, settings.nYResolution)))
		{
			m_pEngine->LogE("Cannot initialize Geometry Render Section: Failed to Initialize tangents GBuffer component!");
			if (res == S_SUCCESS) res = tempRes;
		}

		// --------------------------------------------------------------------------------------------
		// Initialize the GBuffer shader

		if (Failure(m_gBufferShader.Initialize(m_pEngine, "Effects\\gbuffer.fx")))		
			return m_pEngine->LogE("Failed to load GBuffer creation effect");		

		return res;
	}

	// **************************************************************************************************************************

	S_API SResult SDirectX9GeometryRenderSection::Clear( void )
	{
		m_GBufferAlbedo.Clear();
		m_GBufferPosition.Clear();
		m_GBufferNormals.Clear();
		m_GBufferTangents.Clear();

		m_pEngine		= 0;
		m_pDX9RenderPipeline	= 0;

		return S_SUCCESS;
	}

	// **************************************************************************************************************************

	S_API SResult SDirectX9GeometryRenderSection::PrepareSection()
	{
		if (m_pEngine == 0) return S_ABORTED;

		if (m_pDX9RenderPipeline == 0 || !m_pDX9RenderPipeline->IsInitialized())
			return m_pEngine->LogReport(S_ABORTED, "Cannot prepare geometry section: Render pipeline or renderer not initialized!");
												
		// --------------------------------------------------------------------------------------------
		// Check ptrs in advance

		if (m_GBufferAlbedo.pSurface == 0) return m_pEngine->LogReport(S_NOTINIT, "Could not prepare Geometry Render Section: Albedo Framebuffer not initialized!");
		if (m_GBufferPosition.pSurface == 0) return m_pEngine->LogReport(S_NOTINIT, "Could not prepare Geometry Render Section: Position Framebuffer not initialized!");
		if (m_GBufferNormals.pSurface == 0) return m_pEngine->LogReport(S_NOTINIT, "Could not prepare Geometry Render Section: Normals Framebuffer not initialized!");
		if (m_GBufferTangents.pSurface == 0) return m_pEngine->LogReport(S_NOTINIT, "Could not prepare Geometry Render Section: Tangents Framebuffer not initialized!");
		if (m_gBufferShader.GetEffect() == 0) return m_pEngine->LogReport(S_ABORTED, "Cannot prepare Geometry Section: GBuffer creation shader not initilized!");

		// --------------------------------------------------------------------------------------------
		// Set the render targets

		// Convert the Renderer
		SDirectX9Renderer* pDXRenderer = (SDirectX9Renderer*)m_pDX9RenderPipeline->GetRenderer();
		
		if (FAILED(pDXRenderer->pd3dDevice->SetRenderTarget(0, m_GBufferAlbedo.pSurface)))
			return m_pEngine->LogReport(S_ERROR, "Failed to set Albedo Frame Buffer Object of the GBuffer as Render Target!");

		if (FAILED(pDXRenderer->pd3dDevice->SetRenderTarget(1, m_GBufferPosition.pSurface)))
			return m_pEngine->LogReport(S_ERROR, "Failed to set Position Frame Buffer Object of the GBuffer as Render Target!");

		if (FAILED(pDXRenderer->pd3dDevice->SetRenderTarget(2, m_GBufferNormals.pSurface)))
			return m_pEngine->LogReport(S_ERROR, "Failed to set Normals Frame Buffer Object of the GBuffer as Render Target!");

		if (FAILED(pDXRenderer->pd3dDevice->SetRenderTarget(3, m_GBufferTangents.pSurface)))
			return m_pEngine->LogReport(S_ERROR, "Failed to set Tangents Frame Buffer Object of the GBuffer as Render Target!");

		// --------------------------------------------------------------------------------------------
		// Clear the frame buffer objects

		if( FAILED( pDXRenderer->pd3dDevice->Clear( 0, NULL, D3DCLEAR_TARGET, D3DCOLOR_XRGB( 0, 0, 0 ), 1.0f, 0 ) ) )
			return m_pEngine->LogReport( S_ERROR, "Could not clear GBuffer components properly!" );				

		return S_SUCCESS;
	}

	// **************************************************************************************************************************

	S_API SResult SDirectX9GeometryRenderSection::EndSection()
	{
		if (m_pEngine == 0) return S_ABORTED;

		if (m_pDX9RenderPipeline == 0 || !m_pDX9RenderPipeline->IsInitialized())
			return m_pEngine->LogReport(S_NOTINIT, "Cannot End Geometry Section: Render Pipeline not initialized");

		// Get the DirectX9 Renderer
		SDirectX9Renderer* pDXRenderer;
		if (0 == (pDXRenderer = (SDirectX9Renderer*)m_pDX9RenderPipeline->GetRenderer()))
			return m_pEngine->LogReport(S_NOTINIT, "Cannot End Geometry Section: Renderer of RenderPipeline is not initialized!");

		// --------------------------------------------------------------------------------------------

		// Reset the render targets
		if (FAILED(pDXRenderer->pd3dDevice->SetRenderTarget(0, 0)))
			return m_pEngine->LogReport(S_ERROR, "Failed to reset Render Target with index 0");

		if (FAILED(pDXRenderer->pd3dDevice->SetRenderTarget(1, 0)))
			return m_pEngine->LogReport(S_ERROR, "Failed to reset Render Target with index 1");

		if (FAILED(pDXRenderer->pd3dDevice->SetRenderTarget(2, 0)))
			return m_pEngine->LogReport(S_ERROR, "Failed to reset Render Target with index 2");

		if (FAILED(pDXRenderer->pd3dDevice->SetRenderTarget(3, 0)))
			return m_pEngine->LogReport(S_ERROR, "Failed to reset Render Target with index 3");

		return S_SUCCESS;
	}

	// **************************************************************************************************************************

	S_API SResult SDirectX9GeometryRenderSection::PrepareShaderInput(SSolid* pSolid, bool bTextured)
	{
		HRESULT hRes;		
		SDirectX9Renderer* pDXRenderer;
		SDirectX9VertexBuffer* pDXVertexBuffer;
		SDirectX9IndexBuffer* pDXIndexBuffer;		

		SP_ASSERTR(!m_pEngine, S_NOTINIT);		
		SP_ASSERTXR(m_pDX9RenderPipeline == 0 || !m_pDX9RenderPipeline->IsInitialized(), S_NOTINIT, m_pEngine, "Renderer not initialized!");
		SP_ASSERTXR(!pSolid, S_INVALIDPARAM, m_pEngine, "Cannot Prepare GBuffer Creation shader: The given solid ptr is zero.");		

		// Assign the current solid for which the shader inputs are going to be configured
		m_pCurrentSolid = pSolid;

		// Get the DirectX9 Renderer
		pDXRenderer = (SDirectX9Renderer*)m_pDX9RenderPipeline->GetRenderer();

		// -----------------------------------------------------------------------------------------------------------------
		// Setup the data sources

		// Get the vertex buffer of the solid
		pDXVertexBuffer = (SDirectX9VertexBuffer*)m_pEngine->GetResourcePool()->GetVertexBuffer(pSolid->GetVertexBuffer());
		if (!pDXVertexBuffer) return S_ERROR;

		// Get the index buffer of the solid
		pDXIndexBuffer = (SDirectX9IndexBuffer*)m_pEngine->GetResourcePool()->GetIndexBuffer(pSolid->GetIndexBuffer());
		if (!pDXIndexBuffer) return S_ERROR;

		// Setup data streams
		SP_ASSERTXR(Failure(pDXRenderer->pd3dDevice->SetStreamSource(0, pDXVertexBuffer->pHWVertexBuffer, 0, sizeof(SVertex))), S_ERROR, m_pEngine, "Failed SetStreamSource of DX!");
		SP_ASSERTXR(Failure(pDXRenderer->pd3dDevice->SetIndices(pDXIndexBuffer->pHWIndexBuffer)), S_ERROR, m_pEngine, "Failed SetIndices of DX!");

		// -----------------------------------------------------------------------------------------------------------------
		// Retrieve pointer to the effect

		LPD3DXEFFECT pGBufferEffect = m_gBufferShader.GetEffect();

		// -----------------------------------------------------------------------------------------------------------------
		// Set the proper technique matching bTextured flag

		hRes = pGBufferEffect->SetTechnique((bTextured) ? "GBufferTechniqueTex" : "GBufferTechniqueDiffuse");
		SP_ASSERTXR(Failure(hRes), S_ERROR, m_pEngine, "Failed set Technique!");
		
		// --------------------------------------------------------------------------------------------
		// Begin the shader

		hRes = pGBufferEffect->Begin(&m_nCurrentPasses, 0);
		SP_ASSERTXR(Failure(hRes), S_ERROR, m_pEngine, "Failed Begin Shader!");

		// --------------------------------------------------------------------------------------------
		// Setup shader input variables

		// Convert the viewport pointer and get matrices
		SDirectX9Viewport* pDXTargetViewport = (SDirectX9Viewport*)m_pDX9RenderPipeline->GetTargetViewport();
		D3DXMATRIX mtxProjection = SMatrixToDXMatrix(pDXTargetViewport->GetProjectionMatrix());
		D3DXMATRIX mtxWorld = SMatrixToDXMatrix(pSolid->GetWorldMatrix());
		D3DXMATRIX mtxView = SMatrixToDXMatrix(pDXTargetViewport->GetCameraViewMatrix());

		// Set transformation matrices
		if (Failure(pGBufferEffect->SetMatrix("mtxProjection", &mtxProjection)) ||
			Failure(pGBufferEffect->SetMatrix("mtxWorld", &mtxWorld)) ||
			Failure(pGBufferEffect->SetMatrix("mtxview", &mtxView)))
		{
			m_pEngine->LogW("Matrices couldnt be set for GBuffer Effect!");
		}


		// Setup the material				
		SMaterial* pMaterial = pSolid->GetMaterial();			
		SColor* pDiffuseColor = &pMaterial->colDiffuse;			
		D3DXVECTOR4 vDiffuse(pDiffuseColor->r, pDiffuseColor->g, pDiffuseColor->b, pSolid->GetMaterial()->fShininess);
		if (Failure(pGBufferEffect->SetVector("SolidDiffuseColor", &vDiffuse))
			|| Failure(pGBufferEffect->SetFloat("SolidShininess", pMaterial->fShininess))
			|| Failure(pGBufferEffect->SetFloat("SolidEmissive", pMaterial->fEmissivePower)))
		{
			m_pEngine->LogW("Material factory couldnt be set for GBuffer Effect!");
		}	

		// Now its time to throw the render calls!
		return S_SUCCESS;
	}

	// **************************************************************************************************************************

	S_API SResult SDirectX9GeometryRenderSection::RenderSolidGeometry(SSolid* pSolid, bool bTextured)
	{		
		SPrimitive* pPrimitive;

		SP_ASSERTR(!m_pEngine, S_NOTINIT);
		SP_ASSERTXR(m_pDX9RenderPipeline == 0 || !m_pDX9RenderPipeline->IsInitialized(), S_NOTINIT, m_pEngine, "Renderer not initialized!");			
		SP_ASSERTXR(!(m_pCurrentSolid = pSolid), S_NOTINIT, m_pEngine, "Current solid is empty!");

		// -----------------------------------------------------------------------------------------------------------------------
		// Check if solid has to be rendered

		SCamera* pCamera = m_pDX9RenderPipeline->GetTargetViewport()->GetCamera();
		float fMinDist = pCamera->GetViewRadius();

		if (SVector3(pSolid->GetPosition() - pCamera->GetPosition()).Square() > fMinDist)
		{
			return S_SUCCESS; // Does not need to be drawn, so break as early as possible
		}

		// -----------------------------------------------------------------------------------------------------------------------
		// Render textured primitives
		// To be performant we'll select untextured primitives simultaneously during this render process

		if (Failure(PrepareShaderInput(pSolid, true))) return S_ERROR;

		// -----------------------------------------------------------------------------------------------------------------------
		// temporary buffer for untextured primitives

		SP_ASSERTXR(m_pCurrentSolid->GetPrimitiveCount() <= 0, S_INVALIDPARAM, m_pEngine, "Primitive count of solid equals 0!");

		usint32* pUntexturedPrimitives = new UINT[m_pCurrentSolid->GetPrimitiveCount()];
		usint32 nUntexturedPrimitives = 0;

		// -----------------------------------------------------------------------------------------------------------------------

		for (usint32 iPass = 0; iPass < GetCurrentPassCount(); ++iPass)
		{
			if (Failure(m_gBufferShader.GetEffect()->BeginPass(iPass)))
				return m_pEngine->LogReport(S_ERROR, "Failed Begin Pass of GBuffer Creation effect");

			for (usint32 iPrimitive = 0; iPrimitive < (usint32)m_pCurrentSolid->GetPrimitiveCount(); ++iPrimitive)
			{

				//////////////////////////////////
				///////// TODO: Minimize Triangles to be drawn, if they are not visible by camera!
				/////////////////////////////////

				pPrimitive = m_pCurrentSolid->GetPrimitive(iPrimitive);
				SP_ASSERTXR(!pPrimitive, S_ERROR, "Got Nullpointer as primitive to be drawn!");
				
				if (!pPrimitive->bDraw) continue;				

				// Check if this primitive is textured or not
				if (pPrimitive->iTexture.iIndex == SP_TRIVIAL)
				{
					pUntexturedPrimitives[nUntexturedPrimitives] = iPrimitive;
					nUntexturedPrimitives++;
					continue;
				}

				// Render the primitive
				if (Failure(RenderTexturedPrimitive(iPrimitive)))
					return S_ERROR;

			}

			if (Failure(m_gBufferShader.GetEffect()->EndPass()))
				return m_pEngine->LogReport(S_ERROR, "Could not properly End GBuffer creation shader pass");

			// and go on with the next pass
			continue;

		}

		// we need to free the shader input, before we can switch the technique
		if (Failure(FreeShaderInput())) return S_ERROR;

		// -----------------------------------------------------------------------------------------------------------------------
		// Now render untextured primitives

		if (nUntexturedPrimitives > 0)
		{
			// Prepare the untextured shader technique
			if (Failure(PrepareShaderInput(pSolid, false))) return S_ERROR;

			for (usint32 iPass = 0; iPass < m_nCurrentPasses; ++iPass)
			{
				if (Failure(m_gBufferShader.GetEffect()->BeginPass(iPass)))
					return m_pEngine->LogReport(S_ERROR, "Failed Begin Pass of GBuffer Creation effect for untextured primitives");

				for (usint32 iPrimitive = 0; iPrimitive < nUntexturedPrimitives; ++iPrimitive)
				{

					//////////////////////////////////
					///////// TODO: Check if the primitive is inside the view volume, otherwise do not render it
					/////////	This will speed up rendering vastly!
					/////////////////////////////////

					SPrimitive* pPrimitive = pSolid->GetPrimitive(pUntexturedPrimitives[iPrimitive]);
					if (!pPrimitive->bDraw) continue;

					// Render the primitive
					if (Failure(RenderUntexturedPrimitive(iPrimitive)))
						return S_ERROR;

				}

				if (Failure(m_gBufferShader.GetEffect()->EndPass()))
					return m_pEngine->LogReport(S_ERROR, "Could not properly End GBuffer creation shader pass for untextured primitives");

				// and go on with the next pass
				continue;

			}

			if (Failure(FreeShaderInput())) return S_ERROR;
		}

		// -----------------------------------------------------------------------------------------------------------------------
		// Clearup temporary stuff

		delete[] pUntexturedPrimitives;

		return S_SUCCESS;
	}

	// **************************************************************************************************************************

	S_API SResult SDirectX9GeometryRenderSection::RenderTexturedPrimitive( UINT iPrimitive )
	{
		if( m_pEngine == 0 ) return S_NOTINIT;

		// --------------------------------------------------------------------------------------------

		if (m_pDX9RenderPipeline == 0 || !m_pDX9RenderPipeline->IsInitialized())
			return m_pEngine->LogReport(S_ABORTED, "Cannot render textured primitive: Renderer not initilized!");

		if (m_pCurrentSolid == 0)
			return m_pEngine->LogReport(S_ABORTED, "Cannot render textured primitive: Current Solid not initialized!");

		if (iPrimitive > m_pCurrentSolid->GetPrimitiveCount())
			return m_pEngine->LogReport(S_INVALIDPARAM, "Cannot render textured primitive: given id exceeds primitive count of solid!");

		// Get the DirectX9 Renderer
		SDirectX9Renderer* pDXRenderer = (SDirectX9Renderer*)m_pDX9RenderPipeline->GetRenderer();

		// --------------------------------------------------------------------------------------------
		// Get the primitive
		
		SPrimitive* pPrimitive = m_pCurrentSolid->GetPrimitive( iPrimitive );
		
		if( !pPrimitive->bDraw ) return S_SUCCESS;

		// --------------------------------------------------------------------------------------------
		// Setup the texture

		STexture* pTexture;

		if (0 == (pTexture = m_pEngine->GetResourcePool()->GetTexture(pPrimitive->iTexture)))
			return m_pEngine->LogE("Failed to retrieve texture of textured primitive!");

		SDirectX9Texture* pDXTexture = (SDirectX9Texture*)pTexture;
		if (FAILED(pDXRenderer->pd3dDevice->SetTexture(0, pDXTexture->pTexture)))
			return m_pEngine->LogE("Failed to set the texture of textured primitive!");
		 
		// --------------------------------------------------------------------------------------------
		// Draw the actual geometry

		return RenderPrimitiveGeometry(pPrimitive);
	}

	// **************************************************************************************************************************

	S_API SResult SDirectX9GeometryRenderSection::RenderUntexturedPrimitive( UINT iPrimitive )
	{
		if( m_pEngine == 0 ) return S_NOTINIT;

		// --------------------------------------------------------------------------------------------

		if (m_pDX9RenderPipeline == 0 || !m_pDX9RenderPipeline->IsInitialized())
			return m_pEngine->LogReport( S_ABORTED, "Cannot render untextured primitive: Renderer not initilized!" );

		if( m_pCurrentSolid == 0 )
			return m_pEngine->LogReport( S_ABORTED, "Cannot render untextured primitive: Current Solid not initialized!" );

		if( iPrimitive > m_pCurrentSolid->GetPrimitiveCount() )
			return m_pEngine->LogReport( S_INVALIDPARAM, "Cannot render untextured primitive: given id exceeds primitive count of solid!" );

		// Get the DirectX9 Renderer
		SDirectX9Renderer* pDXRenderer = (SDirectX9Renderer*)m_pDX9RenderPipeline->GetRenderer();

		// --------------------------------------------------------------------------------------------
		// Get the primitive
		
		SPrimitive* pPrimitive = m_pCurrentSolid->GetPrimitive( iPrimitive );
		
		if( !pPrimitive->bDraw ) return S_SUCCESS;


		// --------------------------------------------------------------------------------------------
		// Setup the texture
		
		if( FAILED( pDXRenderer->pd3dDevice->SetTexture( 0, NULL ) ) )
			return m_pEngine->LogReport( S_ERROR, "Failed to reset the texture of not-textured primitive!" );

		 
		// --------------------------------------------------------------------------------------------
		// Draw the actual geometry

		return RenderPrimitiveGeometry( pPrimitive );
	}

	// **************************************************************************************************************************

	S_API SResult SDirectX9GeometryRenderSection::RenderPrimitiveGeometry( SPrimitive* pPrimitive )
	{
		if( m_pEngine == NULL ) return S_ABORTED;

		if( m_pDX9RenderPipeline == 0 || !m_pDX9RenderPipeline->IsInitialized() )
			return m_pEngine->LogReport( S_ABORTED, "Cannot render primitive geometry: renderer not initialized!" );

		if( pPrimitive == NULL )
			return m_pEngine->LogReport( S_ABORTED, "Cannot render primitive geometry: given primitive not initialized!" );				

		// Get the DirectX9 Renderer
		SDirectX9Renderer* pDXRenderer = (SDirectX9Renderer*)m_pDX9RenderPipeline->GetRenderer();

		// Get the effect
		LPD3DXEFFECT pGBufferEffect = m_gBufferShader.GetEffect();

		// --------------------------------------------------------------------------------------------
		// Setup the proper Cullmode

		DWORD dwCullMode = D3DCULL_CW;
		if( pPrimitive->tType == S_PRIM_PARTICLE || pPrimitive->tType == S_PRIM_COMPLEX_PLANE )
		{
			dwCullMode = D3DCULL_NONE;
		}

		if( FAILED( pDXRenderer->pd3dDevice->SetRenderState( D3DRS_CULLMODE, dwCullMode ) ) )
		{
			pGBufferEffect->EndPass(); // pass started by Renderpipeline
			pGBufferEffect->End();
			return m_pEngine->LogReport( S_ERROR, "Failed set CullMode render state while rendering a solid inside the pipeline" );
		}

		// --------------------------------------------------------------------------------------------
		// Convert the polygon type

		D3DPRIMITIVETYPE ptType = D3DPT_TRIANGLELIST;
		switch( pPrimitive->tRenderType )
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

	// **************************************************************************************************************************

	S_API SResult SDirectX9GeometryRenderSection::FreeShaderInput(void)
	{
		SP_ASSERTR(!m_pEngine, S_ERROR);
		SP_ASSERTXR(m_pDX9RenderPipeline == 0 || !m_pDX9RenderPipeline->IsInitialized(), S_NOTINIT, "Renderer not initialized!");						

		// --------------------------------------------------------------------------------------------
		// Exit the shader

		if (FAILED(m_gBufferShader.GetEffect()->End()))
			return m_pEngine->LogReport(S_ERROR, "Failed to exit gBuffer creation shader!");

		// --------------------------------------------------------------------------------------------

		return S_SUCCESS;
	}

}