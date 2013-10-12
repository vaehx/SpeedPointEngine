// SpeedPoint DirectX9 OutputPlane

#include <Implementation\DirectX9\SDirectX9Renderer.h>
#include <Implementation\DirectX9\SDirectX9OutputPlane.h>
#include <Implementation\DirectX9\SDirectX9FrameBuffer.h>
#include <SVertex.h>
#include <SpeedPoint.h>

namespace SpeedPoint
{
	// **********************************************************************************

	S_API SResult SDirectX9OutputPlane::Initialize( SpeedPointEngine* eng, SRenderer* renderer, int nW, int nH )
	{
		if( eng == NULL ) return S_ABORTED;
		
		if( renderer == NULL || nW < 600 || nH < 400 )
			return eng->LogReport( S_ABORTED, "Tried Initialize Output Plane with invalid parameters" );

		if( Failure( Clear() ) )
			return S_ERROR;

		pEngine = eng;
		
		pDXRenderer = (SDirectX9Renderer*)renderer;

		// Initialize the shader
		if( Failure( mergeShader.Initialize( pEngine, "Effects\\merge.fx" ) ) )
		{
			return pEngine->LogReport( S_ERROR, "Failed to load Output merge shader!" );
		}

		if( Failure( mergeShader.SetTechnique( "MergeTechnique" ) ) )
		{
			return pEngine->LogReport( S_ERROR, "Failed Set Merge technique when creating OutputPlane" );
		}

		// Initialize the matrices
		float fWidth = (float)nW * 0.1f;
		float fHeight = (float)nH * 0.1f;
		
		D3DXMATRIX mProj;
		D3DXMatrixOrthoRH( &mProj, fWidth, fHeight, 2.0f, 200.0f );		
		mProjection = SMatrix( mProj );

		D3DXMATRIX mV;
		D3DXMatrixLookAtRH( &mV, new D3DXVECTOR3( 0, 0, -10.0f ), new D3DXVECTOR3( 0, 0, 0.0f ), new D3DXVECTOR3( 0, 1.0f, 0 ) );
		mView = SMatrix( mV );

		// Create the geometry with plane of 10 * 10 fields
		if( Failure( vertexBuffer.Initialize( 11 * 11, false, pEngine, renderer ) ) )
		{
			return pEngine->LogReport( S_ERROR, "Failed initialize vertex Buffer of OutputPlane!" );
		}

		if( Failure( indexBuffer.Initialize( 10 * 10 * 6, false, pEngine, renderer, S_INDEXBUFFER_32 ) ) )
		{
			return pEngine->LogReport( S_ERROR,"Failed initialize index buffer of OutputPlane!" );
		}

		SVertex* pVertices = (SVertex*)( malloc( sizeof( SVertex ) * 11 * 11 ) );
		DWORD* pIndices = (DWORD*)( malloc( sizeof( DWORD ) * 10 * 10 * 6 ) );

		UINT vPos = 0, iPos = 0;
				
		float fXDiff = fWidth * 0.1f;
		float fXHalf = fXDiff * ( fWidth * 0.5f );
		
		float fYDiff = fHeight * 0.1f;
		float fYHalf = fYDiff * ( fHeight * 0.5f );
		
		for( int yy = 0; yy < 11; yy++ )
		{
			for( int xx = 0; xx < 11; xx++ )
			{
				pVertices[vPos] = SVertex( (float)( xx - 5 ) * fXDiff, (float)( yy - 5 ) * fYDiff, 2.0f, 0, 0, -1.0f, (float)xx * 0.1f, (float)yy * 0.1f );
				
				if( yy < 10 && xx < 10 )
				{
					// Add a facette / 2 triangles
					pIndices[iPos  ] = vPos;
					pIndices[iPos+1] = vPos + 11;
					pIndices[iPos+2] = vPos + 1;
					pIndices[iPos+3] = vPos + 11;
					pIndices[iPos+4] = vPos + 12;
					pIndices[iPos+5] = vPos + 1;
					iPos += 6;
				}

				++vPos;
			}
		}

		if( Failure( vertexBuffer.Fill( pVertices, 11 * 11, true ) ) )
		{
			return pEngine->LogReport( S_ERROR, "Failed Fill Vertex Buffer of output plane!" );
		}

		if( Failure( indexBuffer.Fill( pIndices, 10 * 10 * 6, true ) ) )
		{
			return pEngine->LogReport( S_ERROR, "Failed fill index buffer of output plane!" );
		}

		free( pVertices );
		free( pIndices );

		return S_SUCCESS;
	}

	// **********************************************************************************

	S_API SResult SDirectX9OutputPlane::Render( SFrameBuffer* pGBufferAlbedo, SFrameBuffer* pLightingBuffer )
	{
		if( pEngine == NULL || pDXRenderer == NULL ) return S_ABORTED;

		if( pGBufferAlbedo == NULL || pLightingBuffer == NULL ) return S_ABORTED;

		pDXRenderer->pd3dDevice->SetRenderState( D3DRS_CULLMODE, D3DCULL_NONE );

		// first, when calling this method, the target buffers should be set properly already

		// so we can immediately setup vertex- and indexbuffer data and start drawing the plane
		if( FAILED( pDXRenderer->pd3dDevice->SetStreamSource( 0, vertexBuffer.pHWVertexBuffer, 0, sizeof( SVertex ) ) ) )
		{
			return pEngine->LogReport( S_ERROR, "Failed set vertex stream source for output plane!" );
		}

		if( FAILED( pDXRenderer->pd3dDevice->SetIndices( indexBuffer.pHWIndexBuffer ) ) )
		{
			return pEngine->LogReport( S_ERROR, "Failed set index stream source for output plane!" );
		}

		SDirectX9FrameBuffer* pDXGAlbedoBuffer = (SDirectX9FrameBuffer*)pGBufferAlbedo;		
		if( FAILED( pDXRenderer->pd3dDevice->SetTexture( 0, pDXGAlbedoBuffer->pTexture ) ) )
		{
			return pEngine->LogReport( S_ERROR, "Failed set albedo buffer as temporary texture of output plane!" );
		}

//////// TODO: Implement Lighting buffer creation and proper merge shader
		/*SDirectX9FrameBuffer* pDXLightingBuffer = (SDirectX9FrameBuffer*)pLightingBuffer;
		if( FAILED( pDXRenderer->pd3dDevice->SetTexture( 1, pDXLightingBuffer->pTexture ) ) )
		{
			return S_ERROR;
		}*/

		UINT nPasses = 0;
		if( FAILED( mergeShader.pEffect->Begin( &nPasses, 0 ) ) )
		{
			return pEngine->LogReport( S_ERROR, "Failed Begin merge shader rendering for output plane!" );
		}

		// Set the shader parameters
		D3DXMATRIX mWorld; D3DXMatrixIdentity( &mWorld );
		
		D3DXMATRIX pmView = (D3DXMATRIX)mView;
		D3DXMATRIX pmProjection = (D3DXMATRIX)mProjection;					

		mergeShader.pEffect->SetMatrix( "World", &mWorld );
		mergeShader.pEffect->SetMatrix( "View", &pmView );
		mergeShader.pEffect->SetMatrix( "Proj", &pmProjection );

		for( UINT iPass = 0; iPass < nPasses; ++iPass )
		{

			if( FAILED( mergeShader.pEffect->BeginPass( iPass ) ) )
			{
				mergeShader.pEffect->End();
				return pEngine->LogReport( S_ERROR, "Failed Begin Merge shader pass for output plane!" );
			}

			if( FAILED( pDXRenderer->pd3dDevice->DrawIndexedPrimitive(
				D3DPT_TRIANGLELIST, 0, 0, 11 * 11, 0, 10 * 10 * 3 ) ) )
			{
				mergeShader.pEffect->EndPass();
				mergeShader.pEffect->End();
				return pEngine->LogReport( S_ERROR, "Failed Draw polygons of output Plane!" );
			}

			if( FAILED( mergeShader.pEffect->EndPass() ) )
			{
				mergeShader.pEffect->End();
				return pEngine->LogReport( S_ERROR, "Failed end merge shader pass for output plane!" );
			}

		}

		if( FAILED( mergeShader.pEffect->End() ) )
		{
			return pEngine->LogReport( S_ERROR, "Failed End merge shader renderering for output plane!" );
		}

		return S_SUCCESS;
	}

	// **********************************************************************************

	S_API SResult SDirectX9OutputPlane::Clear( void )
	{			
		SResult res = S_SUCCESS;

		pDXRenderer = NULL;

		if( Failure( indexBuffer.Clear() ) )
		{
			if( pEngine )
				return pEngine->LogReport( S_ERROR, "Failed Clear index Buffer of output plane!" );
			else
				res = S_ERROR;
		}

		if( Failure( vertexBuffer.Clear() ) )
		{
			if( pEngine )
				return pEngine->LogReport( S_ERROR, "Failed Clear vertex buffer of output plane!" );
			else
				res = S_ERROR;
		}

		if( Failure( mergeShader.Clear() ) )
		{
			if( pEngine )
				return pEngine->LogReport( S_ERROR, "Failed clear merge shader of output plane!" );
			else
				res = S_ERROR;
		}	

		pEngine = NULL;

		return res;
	}
}