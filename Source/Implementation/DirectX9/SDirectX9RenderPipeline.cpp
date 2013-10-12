// SpeedPoint DirectX9 Render Pipeline

#include <Implementation\DirectX9\SDirectX9RenderPipeline.h>
#include <Implementation\DirectX9\SDirectX9FrameBuffer.h>
#include <Implementation\DirectX9\SDirectX9Renderer.h>
#include <Implementation\DirectX9\SDirectX9VertexBuffer.h>
#include <Implementation\DirectX9\SDirectX9IndexBuffer.h>
#include <Implementation\DirectX9\SDirectX9Texture.h>
#include <Implementation\DirectX9\SDirectX9OutputPlane.h>
#include <Implementation\DirectX9\SDirectX9GeometryRenderSection.h>
#include <Abstract\SSolid.h>
#include <SVertex.h>
#include <SPrimitive.h>
#include <SMaterial.h>
#include <SpeedPoint.h>
#include <STransformable.h>

namespace SpeedPoint
{

	// **********************************************************************************

	S_API SResult SDirectX9RenderPipeline::Initialize( SpeedPointEngine* eng, SRenderer* renderer )
	{
		pEngine = eng;
		pRenderer = (SDirectX9Renderer*)renderer;

		if( pEngine == NULL || pRenderer == NULL ) return S_ERROR;

		// Initialize backbuffer from default viewport of the renderer
		pTargetViewport = &pRenderer->vpViewport;

		// Now initialize the GBuffer and lighting buffer
		pGBufferAlbedo = (SFrameBuffer*)new SDirectX9FrameBuffer();
		if( Failure( pGBufferAlbedo->Initialize( pEngine, pRenderer->vpViewport.nXResolution, pRenderer->vpViewport.nYResolution ) ) )
		{
			return pEngine->LogReport( S_ERROR, "Failed initialize Albdeo GBuffer component" );
		}

		pGBufferDepth = (SFrameBuffer*)new SDirectX9FrameBuffer();
		if( Failure( pGBufferDepth->Initialize( pEngine, pRenderer->vpViewport.nXResolution, pRenderer->vpViewport.nYResolution ) ) )
		{
			return pEngine->LogReport( S_ERROR, "Failed initialize Depth GBuffer component" );
		}

		pGBufferNormals = (SFrameBuffer*)new SDirectX9FrameBuffer();
		if( Failure( pGBufferNormals->Initialize( pEngine, pRenderer->vpViewport.nXResolution, pRenderer->vpViewport.nYResolution ) ) )
		{
			return pEngine->LogReport( S_ERROR, "Failed initialize Normal GBuffer component" );
		}

		pLightingBuffer = (SFrameBuffer*)new SDirectX9FrameBuffer();
		if( Failure( pLightingBuffer->Initialize( pEngine, pRenderer->vpViewport.nXResolution, pRenderer->vpViewport.nYResolution ) ) )
		{
			return pEngine->LogReport( S_ERROR, "Failed initialize Light buffer component" );
		}

		// Initialize the gbuffer shader
		if( Failure( gBufferShader.Initialize( pEngine, "Effects\\gbuffer.fx" ) ) )
		{
			return pEngine->LogReport( S_ERROR, "Failed to load GBuffer creation effect" );
		}

		// Initialize the output plane
		pOutputPlane = (SOutputPlane*)new SDirectX9OutputPlane();
		SIZE szVPSize = pTargetViewport->GetSize();
		if( Failure( pOutputPlane->Initialize( pEngine, pRenderer, szVPSize.cx, szVPSize.cy ) ) )
		{
			return pEngine->LogReport( S_ERROR, "Failed initialize Output plane" );		
		}

		// Initialize the Geometry Render Section
		pGeometryRenderSection = new SDirectX9GeometryRenderSection();
		if( Failure( pGeometryRenderSection->Initialize( pEngine, pRenderer ) ) )
		{
			return pEngine->LogReport( S_ERROR, "Failed initialize Geometry render section of Render pipeline!" );
		}

		return S_SUCCESS;
	}

	// **********************************************************************************

	S_API S_RENDER_STATE SDirectX9RenderPipeline::GetState( void )
	{
		return iState;
	}

	// **********************************************************************************

	S_API S_RENDER_STATE SDirectX9RenderPipeline::NextState( void )
	{
		switch( iState )
		{
		case S_RENDER_BEGINFRAME: iState = S_RENDER_GEOMETRY; break;
		case S_RENDER_GEOMETRY: iState = S_RENDER_LIGHTING; break;
		case S_RENDER_LIGHTING: iState = S_RENDER_POST; break;
		case S_RENDER_POST: iState = S_RENDER_ENDFRAME; break;
		case S_RENDER_ENDFRAME: iState = S_RENDER_PRESENT; break;		
		default:
			iState = S_RENDER_NONE;
		}

		return iState;
	}

	// **********************************************************************************

	S_API SViewport* SDirectX9RenderPipeline::GetTargetViewport( void )
	{
		return pTargetViewport;
	}

	// **********************************************************************************

	S_API SResult SDirectX9RenderPipeline::SetTargetViewport( SViewport* pVP )
	{
		if( pVP == NULL )		
			return pEngine->LogReport( S_ABORTED, "Tried to set target Viewport to NULL" );		

		pTargetViewport = pVP;
		return S_SUCCESS;
	}

	// **********************************************************************************

	S_API SResult SDirectX9RenderPipeline::Clear( void )
	{
		if( pEngine )
			pEngine->LogReport( S_INFO, "Clearing DX9 Render Pipeline" );

		pTargetViewport = NULL;

		if( pOutputPlane ) pOutputPlane->Clear(); delete pOutputPlane; pOutputPlane = NULL;
		
		if( pGBufferAlbedo ) pGBufferAlbedo->Clear(); delete pGBufferAlbedo; pGBufferAlbedo = NULL;
		
		if( pGBufferDepth ) pGBufferDepth->Clear(); delete pGBufferDepth; pGBufferDepth = NULL;
		
		if( pGBufferNormals ) pGBufferNormals->Clear(); delete pGBufferNormals; pGBufferNormals = NULL;
		
		if( pLightingBuffer ) pLightingBuffer->Clear(); delete pLightingBuffer; pLightingBuffer = NULL;

		if( pGeometryRenderSection ) pGeometryRenderSection->Clear(); delete pGeometryRenderSection; pGeometryRenderSection = NULL;
		
		pEngine = NULL;
		pRenderer = NULL;

		return S_SUCCESS;
	}

	// **********************************************************************************

	S_API SResult SDirectX9RenderPipeline::CalcViewTransformation( const STransformable* camera )
	{
		if( camera == NULL ) return S_ABORTED;

		// Camera / view matrix
		D3DXVECTOR3 vEyePt( camera->vPosition.x, camera->vPosition.y, camera->vPosition.z );
		
		D3DXVECTOR3 vLookAt(
			camera->vPosition.x + sin( camera->vRotation.y )*cos( camera->vRotation.x ),
			camera->vPosition.y + sin( camera->vRotation.x ),
			camera->vPosition.z + cos( camera->vRotation.y ) * cos( camera->vRotation.x )
		);
		
		D3DXVECTOR3 vUpVec( 0.0f, 1.0f, 0.0f );
		
		D3DXMATRIX mV;
		D3DXMatrixLookAtRH( &mV, &vEyePt, &vLookAt, &vUpVec );
		mView = SMatrix( mV );

		return S_SUCCESS;
	}

	// **********************************************************************************

	S_API SResult SDirectX9RenderPipeline::CalcWorldTransformation( const STransformable* form )
	{
		if( pEngine == NULL ) return S_ABORTED;

		if( form == NULL )
			return pEngine->LogReport( S_ABORTED, "Tried calculate World transformation of not given form" );

		// World / Form matrix
		D3DXMATRIX mTrans, mRot, mScale, mOrig;					
	
		D3DXMatrixIdentity( &mOrig );	
		D3DXMatrixRotationYawPitchRoll	( &mRot,   form->vRotation.y,  form->vRotation.x,  form->vRotation.z  );		
		D3DXMatrixTranslation		( &mTrans, form->vPosition.x,  form->vPosition.y,  form->vPosition.z  );			
		D3DXMatrixScaling		( &mScale, form->vSize.x, form->vSize.y, form->vSize.z );
		
		D3DXMATRIX mW = mOrig * mScale * mRot * mTrans;

		mWorld = SMatrix( mW );

		return S_SUCCESS;
	}

	// **********************************************************************************

	S_API SResult SDirectX9RenderPipeline::CalcRenderingTransformations( const STransformable* form, const STransformable* camera )
	{
		if( pEngine == NULL ) return S_ABORTED;

		if( form == NULL || camera == NULL )
		{
			return pEngine->LogReport( S_ABORTED, "Tried to calculate Rendering transformations without given form and camera" );
		}

		if( Aborted( CalcViewTransformation( camera ) ) )
		{
			return S_ABORTED;
		}

		if( Aborted( CalcWorldTransformation( form ) ) )
		{
			return S_ABORTED;
		}

		return S_SUCCESS;
	}


	// **********************************************************************************
	//				R E N D E R     S T A T E S
	// **********************************************************************************

	// **********************************************************************************

	S_API SResult SDirectX9RenderPipeline::BeginFrameSection( void )
	{
		if( iState != S_RENDER_BEGINFRAME && iState != S_RENDER_NONE ) return S_ABORTED;

		if( pEngine == NULL || pRenderer == NULL ) return S_ABORTED;

		SDirectX9Renderer* pDXRenderer = (SDirectX9Renderer*)pRenderer;

		// Clear the current buffers
		DWORD dwFlags = D3DCLEAR_TARGET;
		if( pDXRenderer->setSettings.bAutoDepthStencil )
			dwFlags |= D3DCLEAR_ZBUFFER;

		if( FAILED( pDXRenderer->pd3dDevice->Clear( 0, NULL, dwFlags, D3DCOLOR_XRGB( 0, 0, 0 ), 1.0f, 0 ) ) )
		{
			return S_ERROR;
		}

		// Begin DX Scene
		if( FAILED( pDXRenderer->BeginScene() ) )
		{
			return S_ERROR;
		}

		if( iState == S_RENDER_NONE ) iState = S_RENDER_BEGINFRAME;

		NextState();

		return S_SUCCESS;
	}

	// **********************************************************************************

	S_API SResult SDirectX9RenderPipeline::RenderSolidGeometry( SSolid* pSolid )
	{
		if( pSolid == NULL ||
			pGBufferAlbedo == NULL ||
			pGBufferDepth == NULL ||
			pGBufferNormals == NULL ||
			gBufferShader.pEffect == NULL ||
			pGeometryRenderSection == NULL ||
			iState != S_RENDER_GEOMETRY )
		{
			return S_ABORTED;
		}


		// -----------------------------------------------------------------------------------------------------------------------
		// Setup the geometry render section

		if( Failure( pGeometryRenderSection->PrepareSection( pSolid ) ) )
			return pEngine->LogReport( S_ERROR, "Cannot prepare geometry render section" );


		// -----------------------------------------------------------------------------------------------------------------------
		// temporary buffer for untextured primitives
		
		UINT* pUntexturedPrimitives = new UINT[pSolid->GetPrimitiveCount()];
		UINT nUntexturedPrimitives = 0;


		// -----------------------------------------------------------------------------------------------------------------------
		// Render textured primitives and select untextured primitives

		if( Failure( pGeometryRenderSection->PrepareGBufferCreationShader( true ) ) )
			return pEngine->LogReport( S_ERROR, "Could not prepare GBuffer creation shader for textured primitives!" );

		for( UINT iPass = 0; iPass < pGeometryRenderSection->nCurrentPasses; ++iPass )
		{			

			if( FAILED( gBufferShader.pEffect->BeginPass( iPass ) ) )			
				return pEngine->LogReport( S_ERROR, "Failed Begin Pass of GBuffer Creation effect" );			

			for( UINT iPrimitive = 0; iPrimitive < (UINT)pSolid->GetPrimitiveCount(); ++iPrimitive )
			{

//////////////////////////////////
///////// TODO: Check if the primitive is inside the view volume, otherwise do not render it
/////////	This will speed up rendering vastly!
/////////////////////////////////

				SPrimitive* pPrimitive = pSolid->GetPrimitive( iPrimitive );
				if( !pPrimitive->bDraw ) continue;

				// Check if this primitive is textured or not
				if( pPrimitive->iTexture.iIndex == SP_TRIVIAL )
				{
					pUntexturedPrimitives[nUntexturedPrimitives] = iPrimitive;
					nUntexturedPrimitives++;
					continue;
				}

				// Render the primitive
				if( Failure( pGeometryRenderSection->RenderTexturedPrimitive( iPrimitive ) ) )
					return S_ERROR;

			}

			if( FAILED( gBufferShader.pEffect->EndPass() ) )			
				return pEngine->LogReport( S_ERROR, "Could not properly End GBuffer creation shader pass" );			

			// and go on with the next pass
			continue;

		}

		if( Failure( pGeometryRenderSection->ExitGBufferCreationShader() ) )
			return S_ERROR;

		// -----------------------------------------------------------------------------------------------------------------------
		// Now render untextured primitives
		
		if( nUntexturedPrimitives > 0 )
		{
			if( Failure( pGeometryRenderSection->PrepareGBufferCreationShader( false ) ) )
				return pEngine->LogReport( S_ERROR, "Could not prepare GBuffer creation shader for not-textured primitives!" );

			for( UINT iPass = 0; iPass < pGeometryRenderSection->nCurrentPasses; ++iPass )
			{			

				if( FAILED( gBufferShader.pEffect->BeginPass( iPass ) ) )			
					return pEngine->LogReport( S_ERROR, "Failed Begin Pass of GBuffer Creation effect for untextured primitives" );			

				for( UINT iPrimitive = 0; iPrimitive < nUntexturedPrimitives; ++iPrimitive )
				{

//////////////////////////////////
///////// TODO: Check if the primitive is inside the view volume, otherwise do not render it
/////////	This will speed up rendering vastly!
/////////////////////////////////

					SPrimitive* pPrimitive = pSolid->GetPrimitive( pUntexturedPrimitives[iPrimitive] );
					if( !pPrimitive->bDraw ) continue;					

					// Render the primitive
					if( Failure( pGeometryRenderSection->RenderUntexturedPrimitive( iPrimitive ) ) )
						return S_ERROR;

				}

				if( FAILED( gBufferShader.pEffect->EndPass() ) )			
					return pEngine->LogReport( S_ERROR, "Could not properly End GBuffer creation shader pass for untextured primitives" );			

				// and go on with the next pass
				continue;

			}

			if( Failure( pGeometryRenderSection->ExitGBufferCreationShader() ) )
				return S_ERROR;
		}

		// -----------------------------------------------------------------------------------------------------------------------
		// Clearup temporary stuff

		delete[] pUntexturedPrimitives;

		
		// -----------------------------------------------------------------------------------------------------------------------

		return S_SUCCESS;
	}

	// **********************************************************************************

	S_API SResult SDirectX9RenderPipeline::StopGeometryRendering( void )
	{
		if( pEngine == NULL || pRenderer == NULL || iState != S_RENDER_GEOMETRY )
			return S_ABORTED;

		NextState();

		return S_SUCCESS;
	}

	// **********************************************************************************
	
	S_API SResult SDirectX9RenderPipeline::RenderLighting( SPool<SLight>* pLightPool )
	{
//////////////////////////////
////// TODO
//////////////////////////////
		NextState();

		return S_SUCCESS;
	}

	// **********************************************************************************

	S_API SResult SDirectX9RenderPipeline::RenderLighting( SLight* pLightArray )
	{
//////////////////////////////
////// TODO
//////////////////////////////
		NextState();

		return S_SUCCESS;
	}

	// **********************************************************************************

	S_API SResult SDirectX9RenderPipeline::RenderPostEffects( SP_UNIQUE* pCustomPostShaders )
	{
/////////////////////////////
////// TODO
////////////////////////////
		NextState();

		return S_SUCCESS;
	}	

	// **********************************************************************************

	S_API SResult SDirectX9RenderPipeline::EndFrameSection( void )		
	{
/////// TODO: Add actually rendering all buffers together using a merge-shader
////	For now, we will simply render the Albedo GBuffer component to the post plane
		if( pEngine == NULL || pRenderer == NULL || pOutputPlane == NULL ) return S_ABORTED;

		SDirectX9Renderer* pDXRenderer = (SDirectX9Renderer*)pRenderer;

		// Setup proper render target		
		SDirectX9FrameBuffer* pDXTargetBackBuffer = (SDirectX9FrameBuffer*)pTargetViewport->GetBackBuffer();
		if( NULL == pDXTargetBackBuffer )
		{
			return pEngine->LogReport( S_ERROR, "Failed to Get BackBuffer of current targetviewport of render pipeline" );
		}

		pDXRenderer->pd3dDevice->SetRenderTarget( 1, NULL );
		pDXRenderer->pd3dDevice->SetRenderTarget( 2, NULL );
		if( FAILED( pDXRenderer->pd3dDevice->SetRenderTarget( 0, pDXTargetBackBuffer->pSurface ) ) )
		{
			return pEngine->LogReport( S_ERROR, "Failed set target backbuffer render target inside render pipeline" );
		}

		if( Failure( pOutputPlane->Render( pGBufferAlbedo, pLightingBuffer ) ) )
		{
			return pEngine->LogReport( S_ERROR, "Failed to render output plane inside render pipeline EndFrameSection()" );
		}

		// And finally we'll end up the DirectX Scene
		if( FAILED( pDXRenderer->EndScene() ) )
		{
			return pEngine->LogReport( S_ERROR, "Failed End DX Scene in render pipeline" );
		}

		NextState();

		return S_SUCCESS;
	}

	// **********************************************************************************

	S_API SResult SDirectX9RenderPipeline::Present( void )
	{
		if( pEngine == NULL ) return S_ABORTED;

		if( pTargetViewport == NULL || pTargetViewport->GetBackBuffer() == NULL || iState != S_RENDER_PRESENT )
		{
			return pEngine->LogReport( S_ABORTED, "Tried to present with not initialized target viewport or not in PRESENT RP State" );
		}

		// Now present
		SDirectX9Viewport* pDXViewport = (SDirectX9Viewport*)pTargetViewport;
		if( pDXViewport->pSwapChain != NULL && FAILED( pDXViewport->pSwapChain->Present( NULL, NULL, NULL, NULL, 0 ) ) )
		{
			return pEngine->LogReport( S_ERROR, "Failed to present current target viewport (additional swapchain) Backbuffer!" );
		}
		else
		{
			if( pDXViewport->pBackBuffer == NULL )
				return pEngine->LogReport( S_ERROR, "Cannot present default viewport that has a null pointer as back buffer!" );

			SDirectX9Renderer* pDXRenderer = (SDirectX9Renderer*)pEngine->GetRenderer();						
			if( FAILED( pDXRenderer->pd3dDevice->Present( NULL, NULL, NULL, NULL ) ) )
				return pEngine->LogReport( S_ERROR, "Failed to present default viewport!" );
		}

		NextState();

		return S_SUCCESS;
	}

}