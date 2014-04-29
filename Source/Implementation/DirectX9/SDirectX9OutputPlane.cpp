// SpeedPoint DirectX9 OutputPlane

#include <Implementation\DirectX9\SDirectX9Renderer.h>
#include <Implementation\DirectX9\SDirectX9OutputPlane.h>
#include <Implementation\DirectX9\SDirectX9FrameBuffer.h>
#include <Implementation\DirectX9\SDirectX9Utilities.h>
#include <Util\SVertex.h>
#include <SpeedPoint.h>
#include <SSpeedPointEngine.h>

namespace SpeedPoint
{
	// **********************************************************************************

	S_API SResult SDirectX9OutputPlane::Initialize( SpeedPointEngine* eng, IRenderer* renderer, int nW, int nH )
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
		mProjection = DXMatrixToSMatrix( mProj );

		D3DXMATRIX mV;
		D3DXMatrixLookAtRH( &mV, new D3DXVECTOR3( 0, 0, -10.0f ), new D3DXVECTOR3( 0, 0, 0.0f ), new D3DXVECTOR3( 0, 1.0f, 0 ) );
		mView = DXMatrixToSMatrix( mV );

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
				pVertices[vPos] = SVertex(
					(float)( xx - 5 ) * fXDiff, (float)( yy - 5 ) * fYDiff, 2.0f,	// position
					0.0f, 0.0f, -1.0f,						// normal
					1.0f, 0.0f, 0.0f,						// tangent
					(float)xx * 0.1f, (float)yy * 0.1f );				// texture coords
				
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

	// This function should only be called by the Render Pipeline!
	// The target buffers should be set properly already!
	S_API SResult SDirectX9OutputPlane::Render( IFrameBuffer* pGBufferAlbedo, IFrameBuffer* pLightingBuffer )
	{
		return S_NOTIMPLEMENTED;
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