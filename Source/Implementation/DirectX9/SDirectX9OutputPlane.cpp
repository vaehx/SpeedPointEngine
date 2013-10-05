// SpeedPoint DirectX9 OutputPlane

#include <Implementation\DirectX9\SDirectX9Renderer.h>
#include <Implementation\DirectX9\SDirectX9OutputPlane.h>
#include <Implementation\DirectX9\SDirectX9FrameBuffer.h>
#include <SVertex.h>

namespace SpeedPoint
{
	// **********************************************************************************

	S_API SResult SDirectX9OutputPlane::Initialize( SpeedPointEngine* eng, SRenderer* renderer, int nW, int nH )
	{
		if( eng == NULL || renderer == NULL || nW < 600 || nH < 400 ) return S_ABORTED;

		if( Failure( Clear() ) )
			return S_ERROR;

		pEngine = eng;
		pDXRenderer = (SDirectX9Renderer*)renderer;

		// Initialize the shader
		if( Failure( mergeShader.Initialize( pEngine, "merge.fx" ) ) )
		{
			return S_ERROR;
		}

		if( Failure( mergeShader.SetTechnique( "MergeTechnique" ) ) )
		{
			return S_ERROR;
		}

		// Initialize the matrices
		float fWidth = (float)nW * 0.1f;
		float fHeight = (float)nH * 0.1f;
		
		D3DXMatrixOrthoRH( &(D3DXMATRIX)mProjection, fWidth, fHeight, 1.0f, 200.0f );

		D3DXMatrixLookAtRH( &(D3DXMATRIX)mView, new D3DXVECTOR3( 0, 0, -10.0f ), new D3DXVECTOR3( 0, 0, 0 ), new D3DXVECTOR3( 0, 1.0f, 0 ) );

		// Create the geometry with plane of 10 * 10 fields
		if( Failure( vertexBuffer.Initialize( 11 * 11, false, pEngine, renderer ) ) )
		{
			return S_ERROR;
		}

		if( Failure( indexBuffer.Initialize( 10 * 10 * 6, false, pEngine, renderer, S_INDEXBUFFER_32 ) ) )
		{
			return S_ERROR;
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
				pVertices[vPos] = SVertex( xx * fXDiff - fXHalf, yy * fYDiff - fYHalf, 0, 0, 0, -1.0f, (float)xx * 0.1f, (float)yy * 0.1f );
				++vPos;

				if( yy < 10 && xx < 10 )
				{
					// Add a facette / 2 triangles
					pIndices[iPos  ] = vPos;
					pIndices[iPos+1] = vPos + 1;
					pIndices[iPos+2] = vPos + 11;
					pIndices[iPos+3] = vPos + 11;
					pIndices[iPos+4] = vPos + 1;
					pIndices[iPos+5] = vPos + 12;
					iPos += 6;
				}
			}
		}

		if( Failure( vertexBuffer.Fill( pVertices, 11 * 11, false ) ) )
		{
			return S_ERROR;
		}

		if( Failure( indexBuffer.Fill( pIndices, 10 * 10 * 6, false ) ) )
		{
			return S_ERROR;
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

		// first, when calling this method, the target buffers should be set properly already

		// so we can immediately setup vertex- and indexbuffer data and start drawing the plane
		if( FAILED( pDXRenderer->pd3dDevice->SetStreamSource( 0, vertexBuffer.pHWVertexBuffer, 0, sizeof( SVertex ) ) ) )
		{
//////// TODO: Throw error
			return S_ERROR;
		}

		if( FAILED( pDXRenderer->pd3dDevice->SetIndices( indexBuffer.pHWIndexBuffer ) ) )
		{
//////// TODO: Throw error
			return S_ERROR;
		}

		SDirectX9FrameBuffer* pDXGAlbedoBuffer = (SDirectX9FrameBuffer*)pGBufferAlbedo;
		if( FAILED( pDXRenderer->pd3dDevice->SetTexture( 0, pDXGAlbedoBuffer->pTexture ) ) )
		{
//////// TODO: Throw error
			return S_ERROR;
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
//////// TODO: Throw error
			return S_ERROR;
		}

		// Set the shader parameters
		D3DXMATRIX mWorld; D3DXMatrixIdentity( &mWorld );

		mergeShader.pEffect->SetMatrix( "World", &mWorld );
		mergeShader.pEffect->SetMatrix( "View", &(D3DXMATRIX)mView );
		mergeShader.pEffect->SetMatrix( "Proj", &(D3DXMATRIX)mProjection );

		for( UINT iPass = 0; iPass < nPasses; ++iPass )
		{

			if( FAILED( mergeShader.pEffect->BeginPass( iPass ) ) )
			{
//////// TODO: Throw error
				return S_ERROR;
			}

			if( FAILED( pDXRenderer->pd3dDevice->DrawIndexedPrimitive(
				D3DPT_TRIANGLELIST, 0, 0, 11 * 11, 0, 10 * 10 * 3 ) ) )
			{
//////// TODO: Throw error
				return S_ERROR;
			}

			if( FAILED( mergeShader.pEffect->EndPass() ) )
			{
//////// TODO: Throw error
				return S_ERROR;
			}

		}

		if( FAILED( mergeShader.pEffect->End() ) )
		{
//////// TODO: Throw error
			return S_ERROR;
		}

		return S_SUCCESS;
	}

	// **********************************************************************************

	S_API SResult SDirectX9OutputPlane::Clear( void )
	{	
		pEngine = NULL;

		pDXRenderer = NULL;

		if( Failure( indexBuffer.Clear() ) )
		{
			return S_ERROR;
		}

		if( Failure( vertexBuffer.Clear() ) )
		{
			return S_ERROR;
		}

		if( Failure( mergeShader.Clear() ) )
		{
			return S_ERROR;
		}	

		return S_SUCCESS;
	}
}