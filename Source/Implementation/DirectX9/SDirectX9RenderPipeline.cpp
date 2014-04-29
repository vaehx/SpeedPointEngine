// ********************************************************************************************

//	This file is part of the SpeedPoint Game Engine

//	(c) 2011-2014 Pascal R. aka iSmokiieZz
//	All rights reserved.

// ********************************************************************************************

#include <Implementation\DirectX9\SDirectX9RenderPipeline.h>
#include <Implementation\DirectX9\SDirectX9FrameBuffer.h>
#include <Implementation\DirectX9\SDirectX9Renderer.h>
#include <Implementation\DirectX9\SDirectX9VertexBuffer.h>
#include <Implementation\DirectX9\SDirectX9IndexBuffer.h>
#include <Implementation\DirectX9\SDirectX9Texture.h>
#include <Implementation\DirectX9\SDirectX9OutputPlane.h>
#include <Implementation\DirectX9\SDirectX9GeometryRenderSection.h>
#include <Implementation\DirectX9\SDirectX9Utilities.h>
#include <Abstract\ISolid.h>
#include <Util\SVertex.h>
#include <Util\SPrimitive.h>
#include <Util\SMaterial.h>
#include <Util\STransformable.h>
#include <SSpeedPointEngine.h>

namespace SpeedPoint
{

	S_API SDirectX9RenderPipeline::SDirectX9RenderPipeline()
		: m_pEngine(0),
		m_pDXRenderer(0),
		m_pTargetViewport(0),
		m_pVertexDeclaration(0)
	{
	}

	// **********************************************************************************

	S_API SDirectX9RenderPipeline::~SDirectX9RenderPipeline()
	{
		Clear();
	}

	// **********************************************************************************

	S_API SResult SDirectX9RenderPipeline::Initialize(SpeedPointEngine* eng, IRenderer* renderer)
	{
		m_pEngine = eng;
		m_pDXRenderer = (SDirectX9Renderer*)renderer;

		SP_ASSERTR((!m_pEngine || !m_pDXRenderer), S_ERROR);

		// Initialize backbuffer from default viewport of the renderer
		m_pTargetViewport = &m_pDXRenderer->vpViewport;

		// Initialize the Geometry Render Section		
		if (Failure(m_GeometryRenderSection.Initialize(m_pEngine, (IRenderPipeline*)this)))
		{
			return m_pEngine->LogE("Failed initialize Geometry render section of Render pipeline!");
		}

		// Initialize Lighting render section
		if (Failure(m_LightingRenderSection.Initialize(m_pEngine, (IRenderPipeline*)this)))
		{
			return m_pEngine->LogE("Failed initialize lighting render section of render pipeline!");
		}

		// INitialize post render section
		if (Failure(m_PostRenderSection.Initialize(m_pEngine, (IRenderPipeline*)this)))
		{
			return m_pEngine->LogE("Failed initilaize post render section of render pipeline!");
		}

		return S_SUCCESS; // seems like everything went well :)
	}

	// **********************************************************************************

	S_API bool SDirectX9RenderPipeline::IsInitialized()
	{
		return (m_pEngine && m_pDXRenderer && m_pTargetViewport);
	}

	// **********************************************************************************

	S_API SResult SDirectX9RenderPipeline::SetFramePipeline(SFramePipeline* pFramePipeline)
	{		
		SP_ASSERTR(!pFramePipeline, S_INVALIDPARAM);
		
		m_pFramePipeline = pFramePipeline;
		return S_SUCCESS;
	}

	// **********************************************************************************

	S_API SResult SDirectX9RenderPipeline::Clear(void)
	{
		if (m_pEngine) m_pEngine->LogReport(S_INFO, "Clearing DX9 Render Pipeline...");

		m_GeometryRenderSection.Clear();
		m_LightingRenderSection.Clear();
		m_PostRenderSection.Clear();

		m_pVertexDeclaration = 0;
		m_pTargetViewport = 0;
		m_pDXRenderer = 0;
		m_pEngine = 0;
		m_pFramePipeline = 0;

		return S_SUCCESS;
	}

	// **********************************************************************************

	S_API S_FRAMEPIPELINE_STAGE SDirectX9RenderPipeline::GetCurrentStage()
	{
		if (m_pFramePipeline == 0)
			return S_IDLE;

		return m_pFramePipeline->GetCurrentStage();
	}

	// **********************************************************************************

	S_API IRenderer* SDirectX9RenderPipeline::GetRenderer()
	{
		return (IRenderer*)m_pDXRenderer;
	}	

	// **********************************************************************************

	S_API IViewport* SDirectX9RenderPipeline::GetTargetViewport(void)
	{
		return m_pTargetViewport;
	}

	// **********************************************************************************

	S_API SResult SDirectX9RenderPipeline::SetTargetViewport(IViewport* pVP)
	{
		SP_ASSERTR(!pVP, S_INVALIDPARAM);

		m_pTargetViewport = pVP;
		return S_SUCCESS;
	}	

	// **********************************************************************************

	/*
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
		mView = DXMatrixToDMatrix( mV );

		return S_SUCCESS;
	}

	// **********************************************************************************

	S_API SResult SDirectX9RenderPipeline::CalcWorldTransformation( const STransformable* form )
	{
		if( m_pEngine == NULL ) return S_ABORTED;

		if( form == NULL )
			return m_pEngine->LogReport( S_ABORTED, "Tried calculate World transformation of not given form" );

		// World / Form matrix
		D3DXMATRIX mTrans, mRot, mScale, mOrig;					
	
		D3DXMatrixIdentity( &mOrig );	
		D3DXMatrixRotationYawPitchRoll	( &mRot,   form->vRotation.y,  form->vRotation.x,  form->vRotation.z  );		
		D3DXMatrixTranslation		( &mTrans, form->vPosition.x,  form->vPosition.y,  form->vPosition.z  );			
		D3DXMatrixScaling		( &mScale, form->vSize.x, form->vSize.y, form->vSize.z );
		
		D3DXMATRIX mW = mOrig * mScale * mRot * mTrans;

		mWorld = DXMatrixToDMatrix( mW );

		return S_SUCCESS;
	}

	// **********************************************************************************

	S_API SResult SDirectX9RenderPipeline::CalcRenderingTransformations( const STransformable* form, const STransformable* camera )
	{
		if( m_pEngine == NULL ) return S_ABORTED;

		if( form == NULL || camera == NULL )
		{
			return m_pEngine->LogReport( S_ABORTED, "Tried to calculate Rendering transformations without given form and camera" );
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
	*/


	// **********************************************************************************
	//				R E N D E R     S E C T I O N S
	// **********************************************************************************

	// **********************************************************************************

	S_API SResult SDirectX9RenderPipeline::DoBeginRendering()
	{		
		SResult res;

		SP_ASSERTR(!IsInitialized(), S_ERROR);

		// Clear the current backbuffer
		// We assume that the RenderTarget is still the Backbuffer!
		DWORD dwFlags = D3DCLEAR_TARGET;
		if (m_pDXRenderer->setSettings.bAutoDepthStencil)
			dwFlags |= D3DCLEAR_ZBUFFER;
		
//~~~~~~~~
// TODO: eliminate direct call to pd3dDevice
		if (Failure(m_pDXRenderer->pd3dDevice->Clear(0, NULL, dwFlags, D3DCOLOR_XRGB(0, 0, 0), 1.0f, 0)))
			return m_pEngine->LogE("Cannot DXDEVICE::Clear (backbuffer)!");
//~~~~~~~~~

		// fire event
		SEventParameters params;
		params.Add("sender", S_PARAMTYPE_PTR, this);
		if (Failure(m_pFramePipeline->CallEvent(S_E_RENDER_GEOMETRY_BEGIN, &params)))
			return S_ERROR;

		// Begin DX Scene
		if (Failure(m_pDXRenderer->BeginScene()))
			return S_ERROR;

		// fire event
		if (Failure(m_pFramePipeline->CallEvent(S_E_RENDER_GEOMETRY_CALLS, &params)))
			return S_ERROR;

		return S_SUCCESS;
	}

	// **********************************************************************************

	S_API SResult SDirectX9RenderPipeline::DoGeometrySection()
	{
		SP_ASSERTR(!IsInitialized(), S_NOTINIT);
		SP_ASSERTXR(GetCurrentStage() != S_RENDER_GEOMETRY, S_INVALIDSTAGE, m_pEngine);

		// fire event
		SEventParameters params;
		params.Add("sender", S_PARAMTYPE_PTR, this);
		if (Failure(m_pFramePipeline->CallEvent(S_E_RENDER_GEOMETRY_BEGIN, &params)))
			return S_ERROR;

		// Setup Geometry render section
		if (Failure(m_GeometryRenderSection.PrepareSection()))
			return S_ERROR;	

		// fire event
		m_pFramePipeline->CallEvent(S_E_RENDER_GEOMETRY_CALLS, &params);

		params.Clear();
		return S_SUCCESS;
	}

	// **********************************************************************************

	S_API SResult SDirectX9RenderPipeline::RenderSolidGeometry(ISolid* pSolid, bool bTextured)
	{
		SResult res;
		SP_ASSERTR(!IsInitialized(), S_NOTINIT);		
		SP_ASSERTXR(GetCurrentStage() != S_RENDER_GEOMETRY, S_INVALIDSTAGE, m_pEngine);	

		// Render the solid geometry
		if (Failure(m_GeometryRenderSection.RenderSolidGeometry(pSolid, false)))
			return S_ERROR;

		return S_SUCCESS;
	}

	// **********************************************************************************

	S_API SResult SDirectX9RenderPipeline::ExitGeometrySection()
	{
		SP_ASSERTR(!IsInitialized(), S_NOTINIT);
		SP_ASSERTDXR(GetCurrentStage() != S_RENDER_GEOMETRY, S_INVALIDSTAGE, m_pEngine, "curstage: %d", (int)GetCurrentStage());

		// Fire event
		SEventParameters params;
		params.Add("sender", S_PARAMTYPE_PTR, this);
		if (Failure(m_pFramePipeline->CallEvent(S_E_RENDER_GEOMETRY_EXIT, &params)))
			return S_ERROR;

		// Exit the Geometry section
		if (Failure(m_GeometryRenderSection.EndSection()))
			return S_ERROR;

		return S_SUCCESS;
	}


	// **********************************************************************************
	
	S_API SResult SDirectX9RenderPipeline::DoLightingSection()
	{
		SP_ASSERTR(!IsInitialized(), S_NOTINIT);
		SP_ASSERTXR(GetCurrentStage() != S_RENDER_LIGHTING, S_INVALIDSTAGE, m_pEngine);

		// Setup the lighting section
		if (Failure(m_LightingRenderSection.PrepareSection()))
			return S_ERROR;

		// Setup light source buffer
		SPool<SLight*>* pLights = 0;

		// fire preparation event
		SEventParameters params;
		params.Add("sender", S_PARAMTYPE_PTR, this);
		params.Add("lights", S_PARAMTYPE_PTR, (void*)&pLights);
		if (Failure(m_pFramePipeline->CallEvent(S_E_RENDER_LIGHTING_PREPARE, &params)))
			return S_ERROR;

		// prepare light sources, or assign custom lights buffer if SSettings::bCustomLightSources is true
		if (Failure(m_LightingRenderSection.PrepareLightSources(&pLights)))
			return S_ERROR;

		// render the lighting
		if (Failure(m_LightingRenderSection.RenderLighting(&m_GeometryRenderSection, m_PostRenderSection.GetOutputPlane())))
			return S_ERROR;

		// ... and exit the lighting section
		if (Failure(m_LightingRenderSection.FreeLightSources()))
			return S_ERROR;

		if (Failure(m_LightingRenderSection.EndSection()))
			return S_ERROR;

		return S_SUCCESS;
	}

	// **********************************************************************************

	S_API SResult SDirectX9RenderPipeline::DoPost()
	{
		SP_ASSERTR(!IsInitialized(), S_NOTINIT);
		SP_ASSERTXR(GetCurrentStage() != S_RENDER_POST, S_INVALIDSTAGE, m_pEngine);

		// Render the output plane to the backbuffer
		if (Failure(m_PostRenderSection.RenderOutputPlane(&m_LightingRenderSection)))
			return S_ERROR;		

		return S_SUCCESS;
	}	

	// **********************************************************************************

	S_API SResult SDirectX9RenderPipeline::DoEndRendering()
	{
		SP_ASSERTR(!IsInitialized(), S_NOTINIT);
		SP_ASSERTXR(GetCurrentStage() != S_RENDER_POST, S_INVALIDSTAGE, m_pEngine);		

		// exit the DX Scene
		if (Failure(m_pDXRenderer->EndScene()))		
			return S_ERROR;		

		// present the backbuffer
		SDirectX9Viewport* pDXViewport = (SDirectX9Viewport*)m_pDXRenderer->GetTargetViewport();
		SP_ASSERTR(!pDXViewport, S_NOTINIT, "pDXViewport is 0");
		if (pDXViewport->pSwapChain)
		{
			if (Failure(pDXViewport->pSwapChain->Present(0, 0, 0, 0, /*flags*/0)))
				return m_pEngine->LogE("Failed to present current target viewport (additional swapchain) Backbuffer!");
		}
		else
		{
			SP_ASSERTXR(!pDXViewport->GetBackBuffer(), S_ERROR, m_pEngine);
						
			if (Failure(m_pDXRenderer->pd3dDevice->Present(0, 0, 0, 0)))
				return m_pEngine->LogE("Failed to present default viewport!");
		}		

		return S_SUCCESS; // okay, done.
	}	
}