// SpeedPoint DirectX9 Render Pipeline

#include <Implementation\DirectX9\SDirectX9RenderPipeline.h>
#include <Implementation\DirectX9\SDirectX9FrameBuffer.h>
#include <Implementation\DirectX9\SDirectX9Renderer.h>
#include <Implementation\DirectX9\SDirectX9VertexBuffer.h>
#include <Implementation\DirectX9\SDirectX9IndexBuffer.h>
#include <Implementation\DirectX9\SDirectX9Texture.h>
#include <Implementation\DirectX9\SDirectX9OutputPlane.h>
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
			return S_ERROR;
		}

		pGBufferDepth = (SFrameBuffer*)new SDirectX9FrameBuffer();
		if( Failure( pGBufferDepth->Initialize( pEngine, pRenderer->vpViewport.nXResolution, pRenderer->vpViewport.nYResolution ) ) )
		{
			return S_ERROR;
		}

		pGBufferNormals = (SFrameBuffer*)new SDirectX9FrameBuffer();
		if( Failure( pGBufferNormals->Initialize( pEngine, pRenderer->vpViewport.nXResolution, pRenderer->vpViewport.nYResolution ) ) )
		{
			return S_ERROR;
		}

		pLightingBuffer = (SFrameBuffer*)new SDirectX9FrameBuffer();
		if( Failure( pLightingBuffer->Initialize( pEngine, pRenderer->vpViewport.nXResolution, pRenderer->vpViewport.nYResolution ) ) )
		{
			return S_ERROR;
		}

		// Initialize the gbuffer shader
		if( Failure( gBufferShader.Initialize( pEngine, "gbuffer.fx" ) ) )
		{
			return S_ERROR;
		}

		// Initialize the output plane
		pOutputPlane = (SOutputPlane*)new SDirectX9OutputPlane();
		SIZE szVPSize = pTargetViewport->GetSize();
		if( Failure( pOutputPlane->Initialize( pEngine, pRenderer, szVPSize.cx, szVPSize.cy ) ) )
		{
			return S_ERROR;
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
		if( pVP == NULL ) return S_ABORTED;
		pTargetViewport = pVP;
		return S_SUCCESS;
	}

	// **********************************************************************************

	S_API SResult SDirectX9RenderPipeline::Clear( void )
	{
		pTargetViewport = NULL;

		if( pOutputPlane ) pOutputPlane->Clear(); delete pOutputPlane; pOutputPlane = NULL;
		
		if( pGBufferAlbedo ) pGBufferAlbedo->Clear(); delete pGBufferAlbedo; pGBufferAlbedo = NULL;
		
		if( pGBufferDepth ) pGBufferDepth->Clear(); delete pGBufferDepth; pGBufferDepth = NULL;
		
		if( pGBufferNormals ) pGBufferNormals->Clear(); delete pGBufferNormals; pGBufferNormals = NULL;
		
		if( pLightingBuffer ) pLightingBuffer->Clear(); delete pLightingBuffer; pLightingBuffer = NULL;
		
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
		
		D3DXMatrixLookAtRH( &(D3DXMATRIX)mView, &vEyePt, &vLookAt, &vUpVec );

		return S_SUCCESS;
	}

	// **********************************************************************************

	S_API SResult SDirectX9RenderPipeline::CalcWorldTransformation( const STransformable* form )
	{
		if( form == NULL ) return S_ABORTED;

		// World / Form matrix
		D3DXMATRIX mTrans, mRot, mScale, mOrig;					
	
		D3DXMatrixIdentity( &mOrig );	
		D3DXMatrixRotationYawPitchRoll	( &mRot,   form->vRotation.y,  form->vRotation.x,  form->vRotation.z  );		
		D3DXMatrixTranslation		( &mTrans, form->vPosition.x,  form->vPosition.y,  form->vPosition.z  );			
		D3DXMatrixScaling		( &mScale, form->vSize.x, form->vSize.y, form->vSize.z );

		mWorld = mOrig * mScale * mRot * mTrans;

		return S_SUCCESS;
	}

	// **********************************************************************************

	S_API SResult SDirectX9RenderPipeline::CalcRenderingTransformations( const STransformable* form, const STransformable* camera )
	{
		if( form == NULL || camera == NULL )
		{
			return S_ABORTED;
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

		if( FAILED( pDXRenderer->pd3dDevice->Clear( 0, NULL, dwFlags, D3DCOLOR_XRGB( 0, 255, 0 ), 1.0f, 0 ) ) )
		{
			return S_ERROR;
		}

		// Begin DX Scene
		if( FAILED( pDXRenderer->pd3dDevice->BeginScene() ) )
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
			iState != S_RENDER_GEOMETRY )
		{
			return S_ABORTED;
		}

		// ----------------------------------------------------------------
		// Convert FrameBuffers
		SDirectX9FrameBuffer* pDXAlbedo = (SDirectX9FrameBuffer*)pGBufferAlbedo;
		SDirectX9FrameBuffer* pDXDepth = (SDirectX9FrameBuffer*)pGBufferDepth;
		SDirectX9FrameBuffer* pDXNormals = (SDirectX9FrameBuffer*)pGBufferNormals;

		// First, set the proper render targets
		SDirectX9Renderer* pDXRenderer = (SDirectX9Renderer*)pRenderer;
		if( FAILED( pDXRenderer->pd3dDevice->SetRenderTarget( 0, pDXAlbedo->pSurface ) ) )
			return S_ERROR;

		if( FAILED( pDXRenderer->pd3dDevice->SetRenderTarget( 1, pDXNormals->pSurface ) ) )
			return S_ERROR;

		if( FAILED( pDXRenderer->pd3dDevice->SetRenderTarget( 2, pDXDepth->pSurface ) ) )
			return S_ERROR;

		// ----------------------------------------------------------------
		// Set shader for GBuffer creation
		UINT nGBufferShaderPasses;
		if( FAILED( gBufferShader.pEffect->Begin( &nGBufferShaderPasses, 0 ) ) )
		{
			return S_ERROR;
		}

		// Set transformation matrices
		SDirectX9Viewport* pDXTargetViewport = (SDirectX9Viewport*)pTargetViewport;		
		gBufferShader.pEffect->SetMatrix( "Proj", &(D3DXMATRIX)pDXTargetViewport->mProjection );


		CalcWorldTransformation( (STransformable*)pSolid );
		gBufferShader.pEffect->SetMatrix( "World", &(D3DXMATRIX)mWorld );

		gBufferShader.pEffect->SetMatrix( "View", &(D3DXMATRIX)mView );		

		// Get the vertex buffer
		SDirectX9VertexBuffer* pDXVertexBuffer =
			(SDirectX9VertexBuffer*)pEngine->GetResourcePool()->GetVertexBuffer( pSolid->GetVertexBuffer() );

		if( pDXVertexBuffer == NULL ) return S_ERROR;

		// Get the index buffer
		SDirectX9IndexBuffer* pDXIndexBuffer = 
			(SDirectX9IndexBuffer*)pEngine->GetResourcePool()->GetIndexBuffer( pSolid->GetIndexBuffer() );

		if( pDXIndexBuffer == NULL ) return S_ERROR;

		// Setup data streams
		pDXRenderer->pd3dDevice->SetStreamSource( 0, pDXVertexBuffer->pHWVertexBuffer, 0, sizeof( SVertex ) );

		pDXRenderer->pd3dDevice->SetIndices( pDXIndexBuffer->pHWIndexBuffer );

		// Setup the material
		SColor* pDiffuseColor = &pSolid->GetMaterial()->colDiffuse;
		D3DXVECTOR4 vDiffuse( pDiffuseColor->r, pDiffuseColor->g, pDiffuseColor->b, pSolid->GetMaterial()->fShininess );				
		gBufferShader.pEffect->SetVector( "SolidDiffuseColor", &vDiffuse );

		// Now simply go through all passes and draw all primitives of the solid
		// that need to be rendered

		// As we are going to improve speed we save the last texture index
		SP_ID iLastTexture;
		iLastTexture.iIndex = -1;

		for( UINT iPass = 0; iPass < nGBufferShaderPasses; ++iPass )
		{			

			if( FAILED( gBufferShader.pEffect->BeginPass( iPass ) ) )
			{
/////// TODO: Throw error, that pass iPass could not be started
				return S_ERROR;
			}

			for( UINT iPrimitive = 0; iPrimitive < (UINT)pSolid->GetPrimitiveCount(); ++iPrimitive )
			{

//////////////////////////////////
///////// TODO: Check if the primitive is inside the view volume, otherwise do not render it
/////////	This will speed up rendering vastly!
/////////////////////////////////

				SPrimitive* pPrimitive = pSolid->GetPrimitive( iPrimitive );
				if( !pPrimitive->bDraw ) continue;

				// Setup the texture
				if( iLastTexture != pPrimitive->iTexture )
				{
					STexture* pTexture = pEngine->GetResourcePool()->GetTexture( pPrimitive->iTexture );			
					if( pTexture != NULL )
					{
						SDirectX9Texture* pDXTexture = (SDirectX9Texture*)pTexture;
						if( FAILED( pDXRenderer->pd3dDevice->SetTexture( 0, pDXTexture->pTexture ) ) )
						{
///// TODO: Give a warning, that the texture could not be properly activated
							continue;
						}
					}
					else
					{
						pDXRenderer->pd3dDevice->SetTexture( 0, NULL );
					}

					iLastTexture = SP_ID( pPrimitive->iTexture );
				}

				// Setup the proper Cullmode
				DWORD dwCullMode = D3DCULL_CCW;
				if( pPrimitive->tType == S_PRIM_PARTICLE || pPrimitive->tType == S_PRIM_COMPLEX_PLANE )
				{
					dwCullMode = D3DCULL_NONE;
				}

				if( FAILED( pDXRenderer->pd3dDevice->SetRenderState( D3DRS_CULLMODE, dwCullMode ) ) )
				{
///////// TODO: Give a warning that the cullmode could not be changed
					return S_ERROR;
				}

				// Convert the polygon type
				D3DPRIMITIVETYPE ptType = D3DPT_TRIANGLELIST;
				switch( pPrimitive->tRenderType )
				{
				case S_PRIM_RENDER_TRIANGLELIST: ptType = D3DPT_TRIANGLELIST; break;
				case S_PRIM_RENDER_TRIANGLESTRIP: ptType = D3DPT_TRIANGLESTRIP; break;
				case S_PRIM_RENDER_LINELIST: ptType = D3DPT_LINELIST; break;
				case S_PRIM_RENDER_POINTLIST: ptType = D3DPT_POINTLIST; break;
				}

				// Now draw the polygons
				if( FAILED( pDXRenderer->pd3dDevice->DrawIndexedPrimitive( ptType,
					pPrimitive->iFirstVertex, 0, (pPrimitive->iLastVertex - pPrimitive->iFirstVertex),
					pPrimitive->iFirstIndex, (UINT)pPrimitive->nPolygons ) ) )
				{
///////// TODO: Throw error that drawing failed
					return S_ERROR;
				}

				// and go on with the next primitive
				continue;

			}

			if( FAILED( gBufferShader.pEffect->EndPass() ) )
			{
//////// TODO: throw error that the current pass could not be ended properly
				return S_ERROR;
			}

			// and go on with the next pass
			continue;

		}

		// Stop using the GBuffer creation shader
		if( FAILED( gBufferShader.pEffect->End() ) )
		{
			return S_ERROR;
		}

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
			return S_ERROR;
		}

		pDXRenderer->pd3dDevice->SetRenderTarget( 1, NULL );
		pDXRenderer->pd3dDevice->SetRenderTarget( 2, NULL );
		if( FAILED( pDXRenderer->pd3dDevice->SetRenderTarget( 0, pDXTargetBackBuffer->pSurface ) ) )
		{
			return S_ERROR;
		}

		if( Failure( pOutputPlane->Render( pGBufferAlbedo, pLightingBuffer ) ) )
		{
			return S_ERROR;
		}

		// And finally we'll end up the DirectX Scene
		if( FAILED( pDXRenderer->EndScene() ) )
		{
			return S_ERROR;
		}

		NextState();

		return S_SUCCESS;
	}

	// **********************************************************************************

	S_API SResult SDirectX9RenderPipeline::Present( void )
	{
		if( pTargetViewport == NULL || pTargetViewport->GetBackBuffer() == NULL || iState != S_RENDER_PRESENT ) return S_ABORTED;		

		// Now present
		SDirectX9Viewport* pDXViewport = (SDirectX9Viewport*)pTargetViewport;
		if( FAILED( pDXViewport->pSwapChain->Present( NULL, NULL, NULL, NULL, 0 ) ) )
			return S_ERROR;

		NextState();

		return S_SUCCESS;
	}

}