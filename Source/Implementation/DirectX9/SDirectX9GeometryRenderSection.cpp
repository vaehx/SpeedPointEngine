// SpeedPoint DirectX9 Geometry Render Section of the Render Pipeline

#include <Implementation\DirectX9\SDirectX9GeometryRenderSection.h>
#include <Implementation\DirectX9\SDirectX9Renderer.h>
#include <Implementation\DirectX9\SDirectX9RenderPipeline.h>
#include <Implementation\DirectX9\SDirectX9FrameBuffer.h>
#include <Implementation\DirectX9\SDirectX9VertexBuffer.h>
#include <Implementation\DirectX9\SDirectX9IndexBuffer.h>
#include <Implementation\DirectX9\SDirectX9Texture.h>
#include <Abstract\SSolid.h>
#include <SMaterial.h>
#include <SVertex.h>
#include <SPrimitive.h>
#include <SpeedPoint.h>

namespace SpeedPoint
{

	// **************************************************************************************************************************

	S_API SResult SDirectX9GeometryRenderSection::Initialize( SpeedPointEngine* eng, SRenderer* pRenderer )
	{
		if( eng == NULL )
			return S_ABORTED;		

		if( pRenderer == NULL )
			return eng->LogReport( S_ABORTED, "Cannot initialize Geometry Render Section with invalid parameters!" );		

		pEngine		= eng;
		pDXRenderer	= (SDirectX9Renderer*)pRenderer;

		return S_SUCCESS;
	}

	// **************************************************************************************************************************

	S_API SResult SDirectX9GeometryRenderSection::Clear( void )
	{
		pEngine		= NULL;
		pDXRenderer	= NULL;
		pSolid		= NULL;

		return S_SUCCESS;
	}

	// **************************************************************************************************************************

	S_API SResult SDirectX9GeometryRenderSection::PrepareSection( SSolid* sol )
	{
		if( pEngine == NULL ) return S_ABORTED;

		if( pDXRenderer == NULL || !pDXRenderer->IsInited() )
			return pEngine->LogReport( S_ABORTED, "Cannot prepare geometry section: renderer not initialized!" );

		// --------------------------------------------------------------------------------------------
		// Setup the solid to be rendered
		
		if( ( pSolid = sol ) == NULL )
			return pEngine->LogReport( S_ABORTED, "Cannot prepare Geometry Section of render pipeline: given solid is not initialized!" );
		
		// --------------------------------------------------------------------------------------------
		// Check the render pipeline

		SDirectX9RenderPipeline* pDXRenderPipeline = (SDirectX9RenderPipeline*)pDXRenderer->GetRenderPipeline();

		if( pDXRenderPipeline->pGBufferAlbedo == NULL )
			return pEngine->LogReport( S_ABORTED, "Cannot prepare Geometry Section: Albedo component of GBuffer not initialized!" );					

		if( pDXRenderPipeline->pGBufferNormals == NULL )
			return pEngine->LogReport( S_ABORTED, "Cannot prepare Geometry Section: Normal component of GBuffer not initialized!" );			

		if( pDXRenderPipeline->pGBufferTangents == NULL )
			return pEngine->LogReport( S_ABORTED, "Cannot prepare Geometry Section: Tangent component of GBuffer not initialized!" );		

		if( pDXRenderPipeline->pGBufferPosition == NULL )
			return pEngine->LogReport( S_ABORTED, "Cannot prepare Geometry Section: Position component of GBuffer not initialized!" );		

		if( pDXRenderPipeline->gBufferShader.pEffect == NULL )
			return pEngine->LogReport( S_ABORTED, "Cannot prepare Geometry Section: GBuffer creation shader not initilized!" );

		// Convert FrameBuffers
		SDirectX9FrameBuffer* pDXAlbedo = (SDirectX9FrameBuffer*)pDXRenderPipeline->pGBufferAlbedo;		
		SDirectX9FrameBuffer* pDXNormals = (SDirectX9FrameBuffer*)pDXRenderPipeline->pGBufferNormals;			
		SDirectX9FrameBuffer* pDXTangents = (SDirectX9FrameBuffer*)pDXRenderPipeline->pGBufferTangents;
		SDirectX9FrameBuffer* pDXPosition = (SDirectX9FrameBuffer*)pDXRenderPipeline->pGBufferPosition;

		// --------------------------------------------------------------------------------------------
		// Set the render targets		

		// Convert the renderer		

		// First, set the proper render targets
		if( FAILED( pDXRenderer->pd3dDevice->SetRenderTarget( 0, pDXAlbedo->pSurface ) ) )
			return pEngine->LogReport( S_ERROR, "Failed to set Albedo Frame Buffer Object of the GBuffer as Render Target!" );

		if( FAILED( pDXRenderer->pd3dDevice->SetRenderTarget( 1, pDXPosition->pSurface ) ) )
			return pEngine->LogReport( S_ERROR, "Failed to set Position Frame Buffer Object of the GBuffer as Render Target!" );

		if( FAILED( pDXRenderer->pd3dDevice->SetRenderTarget( 2, pDXNormals->pSurface ) ) )
			return pEngine->LogReport( S_ERROR, "Failed to set Normals Frame Buffer Object of the GBuffer as Render Target!" );

		if( FAILED( pDXRenderer->pd3dDevice->SetRenderTarget( 3, pDXTangents->pSurface ) ) )
			return pEngine->LogReport( S_ERROR, "Failed to set Tangents Frame Buffer Object of the GBuffer as Render Target!" );		

		// --------------------------------------------------------------------------------------------
		// Clear the frame buffer objects

		if( FAILED( pDXRenderer->pd3dDevice->Clear( 0, NULL, D3DCLEAR_TARGET, D3DCOLOR_XRGB( 0, 0, 0 ), 1.0f, 0 ) ) )
			return pEngine->LogReport( S_ERROR, "Could not clear GBuffer components properly!" );

		
		// --------------------------------------------------------------------------------------------
		// Setup the data sources

		// Get the vertex buffer
		SDirectX9VertexBuffer* pDXVertexBuffer =
			(SDirectX9VertexBuffer*)pEngine->GetResourcePool()->GetVertexBuffer( pSolid->GetVertexBuffer() );

		if( pDXVertexBuffer == NULL ) return S_ERROR;

		// Get the index buffer
		SDirectX9IndexBuffer* pDXIndexBuffer = 
			(SDirectX9IndexBuffer*)pEngine->GetResourcePool()->GetIndexBuffer( pSolid->GetIndexBuffer() );

		if( pDXIndexBuffer == NULL ) return S_ERROR;

		// Setup data streams
		if( FAILED( pDXRenderer->pd3dDevice->SetStreamSource( 0, pDXVertexBuffer->pHWVertexBuffer, 0, sizeof( SVertex ) ) ) )
			return pEngine->LogReport( S_ERROR, "Failed to set DX9 Vertex Buffer Resource as stream source!" );

		if( FAILED( pDXRenderer->pd3dDevice->SetIndices( pDXIndexBuffer->pHWIndexBuffer ) ) )
			return pEngine->LogReport( S_ERROR, "Failed to set DX9 Index Buffer Resource as indices stream source!" );

		return S_SUCCESS;
	}

	// **************************************************************************************************************************

	S_API SResult SDirectX9GeometryRenderSection::PrepareGBufferCreationShader( bool bTextured )
	{
		if( pEngine == NULL ) return S_ABORTED;
		
		if( pDXRenderer == NULL || !pDXRenderer->IsInited() )
			return pEngine->LogReport( S_ABORTED, "Cannot Prepare GBuffer Creation shader: Renderer not initialized!" );

		if( pSolid == NULL )
			return pEngine->LogReport( S_ABORTED, "Cannot Prepare GBuffer Creation shader: Section not initialized" );

		SDirectX9RenderPipeline* pDXRenderPipeline = (SDirectX9RenderPipeline*)pDXRenderer->GetRenderPipeline();

		
		// --------------------------------------------------------------------------------------------
		// Set the proper technique

		if( bTextured )
		{
			if( FAILED( pDXRenderPipeline->gBufferShader.pEffect->SetTechnique( "GBufferTechniqueTex" ) ) )
				return pEngine->LogReport( S_ERROR, "Failed to set GBufferTechniqueTex as current GBuffer creation technique!" );
		}
		else
		{
			if( FAILED( pDXRenderPipeline->gBufferShader.pEffect->SetTechnique( "GBufferTechniqueDiffuse" ) ) )
				return pEngine->LogReport( S_ERROR, "Failed to set GBufferTechniqueDiffuse as current GBuffer creation technique!" );
		}

		
		// --------------------------------------------------------------------------------------------
		// Begin the shader

		if( FAILED( pDXRenderPipeline->gBufferShader.pEffect->Begin( &nCurrentPasses, 0 ) ) )
			return pEngine->LogReport( S_ERROR, "Failed to start GBuffer creation shader!" );


		// --------------------------------------------------------------------------------------------
		// Setup shader parameters

		// Convert the viewport pointer
		SDirectX9Viewport* pDXTargetViewport = (SDirectX9Viewport*)pDXRenderPipeline->pTargetViewport;				

		// Recalculate the world transformation matrix
		if( Failure( pDXRenderPipeline->CalcRenderingTransformations( (STransformable*)pSolid, (STransformable*)pDXTargetViewport->pCamera ) ) )
			return S_ERROR;

//////// DEBUG BEGIN
		D3DXMATRIX mProj;
		//D3DXMatrixOrthoRH( &mProj, 10.0f, 10.0f, 2.0f, 200.0f );
		D3DXMatrixPerspectiveFovRH( &mProj, 100.0f, (float)pDXTargetViewport->nXResolution / (float)pDXTargetViewport->nYResolution, 1.0f, 200.0f );

		D3DXMATRIX mWrld = (D3DXMATRIX)pDXRenderPipeline->mWorld;		

		D3DXMATRIX mV;
		D3DXMatrixLookAtRH( &mV, new D3DXVECTOR3( 0, 0, -10.0f ), new D3DXVECTOR3( 0, 0, 1.0f ), new D3DXVECTOR3( 0, 1.0f, 0 ) );

		// Set transformation matrices
		if( FAILED( pDXRenderPipeline->gBufferShader.pEffect->SetMatrix( "mtxProjection", &mProj ) ) )
			return pEngine->LogReport( S_ERROR, "Failed to pass projection matrix to GBuffer creation shader!" );

		if( FAILED( pDXRenderPipeline->gBufferShader.pEffect->SetMatrix( "mtxWorld", &mWrld ) ) )
			return pEngine->LogReport( S_ERROR, "Failed to pass world matrix to GBuffer creation shader!" );

		if( FAILED( pDXRenderPipeline->gBufferShader.pEffect->SetMatrix( "mtxView", &mV ) ) )
			return pEngine->LogReport( S_ERROR, "Failed to pass view matrix to GBuffer creation shader!" );

//////// DEbUG END


		// Setup the material
		// Note: we only need the diffuse color for gbuffer creation
		// We also need this only if we are not texture mapping the solid
		if( !bTextured )
		{
			SColor* pDiffuseColor = &pSolid->GetMaterial()->colDiffuse;

			D3DXVECTOR4 vDiffuse( pDiffuseColor->r, pDiffuseColor->g, pDiffuseColor->b, pSolid->GetMaterial()->fShininess );				
			
			if( FAILED( pDXRenderPipeline->gBufferShader.pEffect->SetVector( "SolidDiffuseColor", &vDiffuse ) ) )
				return pEngine->LogReport( S_ERROR, "Failed to pass Solid diffuse color to the shader!" );
		}

		return S_SUCCESS;
	}

	// **************************************************************************************************************************

	S_API SResult SDirectX9GeometryRenderSection::RenderTexturedPrimitive( UINT iPrimitive )
	{
		if( pEngine == NULL ) return S_ABORTED;

		// --------------------------------------------------------------------------------------------

		if( pDXRenderer == NULL || !pDXRenderer->IsInited() )
			return pEngine->LogReport( S_ABORTED, "Cannot render textured primitive: Renderer not initilized!" );

		if( pSolid == NULL )
			return pEngine->LogReport( S_ABORTED, "Cannot render textured primitive: Current Solid not initialized!" );

		if( iPrimitive > pSolid->GetPrimitiveCount() )
			return pEngine->LogReport( S_ABORTED, "Cannot render textured primitive: given id exceeds primitive count of solid!" );

		// --------------------------------------------------------------------------------------------
		// Get the primitive
		
		SPrimitive* pPrimitive = pSolid->GetPrimitive( iPrimitive );
		
		if( !pPrimitive->bDraw ) return S_SUCCESS;


		// --------------------------------------------------------------------------------------------
		// Setup the texture

		STexture* pTexture;

		if( NULL == ( pTexture = pEngine->GetResourcePool()->GetTexture( pPrimitive->iTexture ) ) )
			return pEngine->LogReport( S_ERROR, "Failed to retrieve texture of textured primitive!" );

		SDirectX9Texture* pDXTexture = (SDirectX9Texture*)pTexture;
		if( FAILED( pDXRenderer->pd3dDevice->SetTexture( 0, pDXTexture->pTexture ) ) )
			return pEngine->LogReport( S_ERROR, "Failed to set the texture of textured primitive!" );

		 
		// --------------------------------------------------------------------------------------------
		// Draw the actual geometry

		return RenderPrimitiveGeometry( pPrimitive );
	}

	// **************************************************************************************************************************

	S_API SResult SDirectX9GeometryRenderSection::RenderUntexturedPrimitive( UINT iPrimitive )
	{
		if( pEngine == NULL ) return S_ABORTED;

		// --------------------------------------------------------------------------------------------

		if( pDXRenderer == NULL || !pDXRenderer->IsInited() )
			return pEngine->LogReport( S_ABORTED, "Cannot render untextured primitive: Renderer not initilized!" );

		if( pSolid == NULL )
			return pEngine->LogReport( S_ABORTED, "Cannot render untextured primitive: Current Solid not initialized!" );

		if( iPrimitive > pSolid->GetPrimitiveCount() )
			return pEngine->LogReport( S_ABORTED, "Cannot render untextured primitive: given id exceeds primitive count of solid!" );

		// --------------------------------------------------------------------------------------------
		// Get the primitive
		
		SPrimitive* pPrimitive = pSolid->GetPrimitive( iPrimitive );
		
		if( !pPrimitive->bDraw ) return S_SUCCESS;


		// --------------------------------------------------------------------------------------------
		// Setup the texture
		
		if( FAILED( pDXRenderer->pd3dDevice->SetTexture( 0, NULL ) ) )
			return pEngine->LogReport( S_ERROR, "Failed to reset the texture of not-textured primitive!" );

		 
		// --------------------------------------------------------------------------------------------
		// Draw the actual geometry

		return RenderPrimitiveGeometry( pPrimitive );
	}

	// **************************************************************************************************************************

	S_API SResult SDirectX9GeometryRenderSection::RenderPrimitiveGeometry( SPrimitive* pPrimitive )
	{
		if( pEngine == NULL ) return S_ABORTED;

		if( pDXRenderer == NULL || !pDXRenderer->IsInited() )
			return pEngine->LogReport( S_ABORTED, "Cannot render primitive geometry: renderer not initialized!" );

		if( pPrimitive == NULL )
			return pEngine->LogReport( S_ABORTED, "Cannot render primitive geometry: given primitive not initialized!" );

		// --------------------------------------------------------------------------------------------
		// Retrieve the render pipeline

		SDirectX9RenderPipeline* pDXRenderPipeline = (SDirectX9RenderPipeline*)pDXRenderer->GetRenderPipeline();

		// --------------------------------------------------------------------------------------------
		// Setup the proper Cullmode

		DWORD dwCullMode = D3DCULL_CW;
		if( pPrimitive->tType == S_PRIM_PARTICLE || pPrimitive->tType == S_PRIM_COMPLEX_PLANE )
		{
			dwCullMode = D3DCULL_NONE;
		}

		if( FAILED( pDXRenderer->pd3dDevice->SetRenderState( D3DRS_CULLMODE, dwCullMode ) ) )
		{
			pDXRenderPipeline->gBufferShader.pEffect->EndPass();
			pDXRenderPipeline->gBufferShader.pEffect->End();
			return pEngine->LogReport( S_ERROR, "Failed set CullMode render state while rendering a solid inside the pipeline" );
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

		if( FAILED( pDXRenderer->pd3dDevice->DrawIndexedPrimitive( ptType,
			pPrimitive->iFirstVertex, 0, (pPrimitive->iLastVertex - pPrimitive->iFirstVertex + 1),
			pPrimitive->iFirstIndex, (UINT)pPrimitive->nPolygons ) ) )
		{
			pDXRenderPipeline->gBufferShader.pEffect->EndPass();
			pDXRenderPipeline->gBufferShader.pEffect->End();
			return pEngine->LogReport( S_ERROR, "Failed draw Polygons inside render pipeline!" );
		}

		// --------------------------------------------------------------------------------------------

		return S_SUCCESS;
	}

	// **************************************************************************************************************************

	S_API SResult SDirectX9GeometryRenderSection::ExitGBufferCreationShader( void )
	{
		if( pEngine == NULL ) return S_ABORTED;

		if( pDXRenderer == NULL || !pDXRenderer->IsInited() )
			return pEngine->LogReport( S_ABORTED, "Cannot Exit GBuffer creation shader: Renderer not initialized!" );


		// --------------------------------------------------------------------------------------------
		// Retrieve the render pipeline

		SDirectX9RenderPipeline* pDXRenderPipeline = (SDirectX9RenderPipeline*)pDXRenderer->GetRenderPipeline();

		// --------------------------------------------------------------------------------------------
		// Exit the shader

		if( FAILED( pDXRenderPipeline->gBufferShader.pEffect->End() ) )
			return pEngine->LogReport( S_ERROR, "Failed to exit gBuffer creation shader!" );

		// --------------------------------------------------------------------------------------------

		return S_SUCCESS;
	}

}