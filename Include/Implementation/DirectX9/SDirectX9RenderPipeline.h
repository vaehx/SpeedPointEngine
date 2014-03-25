// ******************************************************************************************

//	SpeedPoint DirectX9 Render Pipeline

// ******************************************************************************************

#pragma once
#include <SPrerequisites.h>
#include <Abstract\SRenderPipeline.h>
#include <Abstract\SFrameBuffer.h>
#include "SDirectX9Shader.h"
#include <d3dx9.h>

namespace SpeedPoint
{
	class S_API SDirectX9RenderPipeline : public SRenderPipeline
	{
	public:
		SpeedPointEngine*		pEngine;
		SDirectX9Renderer*		pRenderer;
		SViewport*			pTargetViewport;

		// -- Rendering Matrices --		
		D3DMATRIX			mView;
		D3DMATRIX			mWorld;

		// -- Deferred Rendering --
		LPDIRECT3DVERTEXDECLARATION9	pVertexDeclaration;
		SDirectX9Shader			gBufferShader;
		SFrameBuffer*			pGBufferPosition;	// in world space
		SFrameBuffer*			pGBufferNormals;	// in world space
		SFrameBuffer*			pGBufferTangents;	// in world space		
		SFrameBuffer*			pGBufferAlbedo;
		SFrameBuffer*			pLightingBuffer;
		S_RENDER_STATE			iState;
		SOutputPlane*			pOutputPlane;
		SDirectX9GeometryRenderSection*	pGeometryRenderSection;

		// Default constructor
		SDirectX9RenderPipeline()
			: pTargetViewport( NULL ),
			pGBufferPosition( NULL ),
			pGBufferNormals( NULL ),
			pGBufferTangents( NULL ),			
			pGBufferAlbedo( NULL ),
			pLightingBuffer( NULL ),
			pOutputPlane( NULL ),
			iState( S_RENDER_NONE ) {};

		// --

		SResult Initialize( SpeedPointEngine* pEngine, SRenderer* pRenderer );
		
		S_RENDER_STATE GetState( void );
		
		S_RENDER_STATE NextState( void );
		
		SViewport* GetTargetViewport( void );
		
		SResult SetTargetViewport( SViewport* pVP );
		
		SResult Clear( void );


		SResult CalcRenderingTransformations( const STransformable* form, const STransformable* camera );

		SResult CalcViewTransformation( const STransformable* camera );

		SResult CalcWorldTransformation( const STransformable* form );
		

		SResult BeginFrameSection( void );


		SResult RenderSolidGeometry( SSolid* pSolid );		

		SResult StopGeometryRendering( void );

		
		SResult RenderLighting( SPool<SLight>* pLightPool );

		SResult RenderLighting( SLight* pLightArray );


		SResult RenderPostEffects( SP_UNIQUE* pCustomPostShaders );

		
		SResult EndFrameSection( void );



		SResult Present( void );		
	};
}