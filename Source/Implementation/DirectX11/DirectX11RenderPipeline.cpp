//////////////////////////////////////////////////////////////////////////////////////////////////
//
//	This file is part of the SpeedPoint Game Engine
//
//	written by Pascal R. aka iSmokiieZz
//	(c) 2011-2014, All rights reserved.
//
//////////////////////////////////////////////////////////////////////////////////////////////////

#include <Implementation\DirectX11\DirectX11RenderPipeline.h>


#include <Implementation\DirectX11\DirectX11Renderer.h>
#include <Implementation\DirectX11\DirectX11VertexBuffer.h>
#include <Implementation\DirectX11\DirectX11IndexBuffer.h>
#include <Implementation\DirectX11\DirectX11Texture.h>
#include <Implementation\DirectX11\DirectX11OutputPlane.h>
#include <Implementation\DirectX11\DirectX11GeometryRenderSection.h>
#include <Implementation\DirectX11\DirectX11Utilities.h>
#include <Implementation\DirectX11\DirectX11RenderPipeline.h>
#include <Implementation\DirectX11\DirectX11FBO.h>
#include <Abstract\ISolid.h>
#include <Util\SVertex.h>
#include <Util\SPrimitive.h>
#include <Util\SMaterial.h>
#include <Util\STransformable.h>
#include <SpeedPointEngine.h>
#include <Pipelines\FramePipeline.h>

namespace SpeedPoint
{

	S_API DirectX11RenderPipeline::DirectX11RenderPipeline()
		: m_pEngine(0),
		m_pDXRenderer(0),
		m_pTargetViewport(0)		
	{
	}

	// **********************************************************************************

	S_API DirectX11RenderPipeline::~DirectX11RenderPipeline()
	{
		Clear();
	}

	// **********************************************************************************

	S_API SResult DirectX11RenderPipeline::Initialize(SpeedPointEngine* eng, IRenderer* renderer)
	{
		m_pEngine = eng;
		m_pDXRenderer = dynamic_cast<DirectX11Renderer*>(renderer);

		SP_ASSERTR(m_pEngine && m_pDXRenderer, S_ERROR);

		// Initialize backbuffer from default viewport of the renderer
		m_pTargetViewport = m_pDXRenderer->GetTargetViewport();

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

		// Initialize post render section
		if (Failure(m_PostRenderSection.Initialize(m_pEngine, (IRenderPipeline*)this)))
		{
			return m_pEngine->LogE("Failed initilaize post render section of render pipeline!");
		}

		// Initialize the render queue
		// we are currently using default initial approximated queue size
		if (Failure(m_Queue.Init()))
		{
			return m_pEngine->LogE("Failed initialize render command queue!");
		}

		return S_SUCCESS; // seems like everything went well :)
	}

	// **********************************************************************************

	S_API bool DirectX11RenderPipeline::IsInitialized()
	{
		return (m_pEngine && m_pDXRenderer && m_pTargetViewport);
	}

	// **********************************************************************************

	S_API SResult DirectX11RenderPipeline::SetFramePipeline(FramePipeline* pFramePipeline)
	{
		SP_ASSERTR(pFramePipeline, S_INVALIDPARAM);

		m_pFramePipeline = pFramePipeline;
		return S_SUCCESS;
	}

	// **********************************************************************************

	S_API SResult DirectX11RenderPipeline::Clear(void)
	{
		if (m_pEngine) m_pEngine->LogReport(S_INFO, "Clearing DX9 Render Pipeline...");

		m_Queue.Clear();

		m_GeometryRenderSection.Clear();
		m_LightingRenderSection.Clear();
		m_PostRenderSection.Clear();

		//m_pVertexDeclaration = 0;
		m_pTargetViewport = 0;
		m_pDXRenderer = 0;
		m_pEngine = 0;
		m_pFramePipeline = 0;

		return S_SUCCESS;
	}

	// **********************************************************************************

	S_API S_FRAMEPIPELINE_STAGE DirectX11RenderPipeline::GetCurrentStage()
	{
		if (m_pFramePipeline == 0)
			return S_IDLE;

		return m_pFramePipeline->GetCurrentStage();
	}

	// **********************************************************************************

	S_API IRenderer* DirectX11RenderPipeline::GetRenderer()
	{
		return (IRenderer*)m_pDXRenderer;
	}

	// **********************************************************************************

	S_API IViewport* DirectX11RenderPipeline::GetTargetViewport(void)
	{
		return m_pTargetViewport;
	}

	// **********************************************************************************

	S_API SResult DirectX11RenderPipeline::SetTargetViewport(IViewport* pVP)
	{
		SP_ASSERTR(pVP, S_INVALIDPARAM);

		m_pTargetViewport = pVP;
		return S_SUCCESS;
	}


	// **********************************************************************************
	//				R E N D E R     S E C T I O N S
	// **********************************************************************************

	// **********************************************************************************

	S_API SResult DirectX11RenderPipeline::DoBeginRendering(S_GEOMETRY_RENDER_INTERACTION_STRATEGY geomRenderStrategy)
	{
		SResult res;

		SP_ASSERTR(IsInitialized(), S_ERROR);

		// Clear the current backbuffer		
		m_pDXRenderer->ClearRenderTargets();				

		// fire event				
		if (Failure(m_pFramePipeline->CallEvent(S_E_RENDER_GEOMETRY_BEGIN, 0)))
			return S_ERROR;



		//~~~~~~~~
		// TODO: Maybe we don't need this anymore in DX11

		// Begin DX Scene
		if (Failure(m_pDXRenderer->BeginScene()))
			return S_ERROR;

		//~~~~~~~~


		// store Geometry Render strategy for interaction with the application
		m_GeometryRenderStrategy = geomRenderStrategy;

		return S_SUCCESS;
	}

	// **********************************************************************************

	S_API SResult DirectX11RenderPipeline::DoGeometrySection()
	{
		SP_ASSERTR(IsInitialized(), S_NOTINIT);
		SP_ASSERTXR(GetCurrentStage() == S_RENDER_GEOMETRY, S_INVALIDSTAGE, m_pEngine);

		// fire event			
		if (Failure(m_pFramePipeline->CallEvent(S_E_RENDER_GEOMETRY_BEGIN, 0)))
			return S_ERROR;

		// Setup Geometry render section
		if (Failure(m_GeometryRenderSection.PrepareSection()))
			return S_ERROR;

		// fire calls event or examine Render command queue
		// in order to access m_GeometryRenderStrategy you HAVE to call DoBeginRendering first!
		if (m_GeometryRenderStrategy == eGEOMRENDER_STRATEGY_EVENTS)
		{
			// using same params like above, as they do not change
			if (Failure(m_pFramePipeline->CallEvent(S_E_RENDER_GEOMETRY_CALLS, 0)))
				return S_ERROR;
		}
		else
		{
			// go through queue
			// DirectX11RenderPipeline::ExamineRenderCommandQueue()
			if (Failure(ExamineRenderCommandQueue()))
				return S_ERROR;
		}

		return S_SUCCESS;
	}

	// **********************************************************************************

	S_API inline SResult DirectX11RenderPipeline::ExamineRenderCommand(SCommandDescription* pDesc)
	{
		switch (pDesc->commandType)
		{
		case eSRCMD_DRAWPRIMITIVE:




			//SDrawPrimitiveData* pData = (SDrawPrimitiveData*)pDesc->pData;
			/// TODO






			break;

		case eSRCMD_DRAWSOLID:






			//SDrawSolidData* pData = (SDrawSolidData*)pDesc->pData;
			/// TODO





			break;

		default:
			return S_INVALIDPARAM;
		}

		return S_SUCCESS;
	}

	// **********************************************************************************

	S_API inline SResult DirectX11RenderPipeline::ExamineRenderCommandHybrid(SCommandDescription* pDesc)
	{
		SEventParameters params;
		if (pDesc->commandType == eSRCMD_DRAWPRIMITIVE || pDesc->commandType == eSRCMD_DRAWSOLID)
		{
			params.AddSenderParam(this);
			params.Add(ePARAM_DRAW_DESC, S_PARAMTYPE_PTR, &pDesc);

			// the custom event data is set by the CallEvent function of the Framepipe
			//params.Add(ePARAM_CUSTOM_DATA, S_PARAMTYPE_PTR, m_pFramePipeline->GetCustomEventParameterData());						

			// At this point, the concept of the hybrid geom input strategy is, that the pipeline
			// does not handle the draw calls, but the application, handling the following event will.
			m_pFramePipeline->CallEvent((pDesc->commandType == eSRCMD_DRAWPRIMITIVE) ? S_E_RENDER_GEOMETRY_DRAWPRIMITIVE : S_E_RENDER_GEOMETRY_DRAWSOLID, &params);

			// ... so we're done here.
			params.Clear();
		}

		return S_SUCCESS;
	}

	// **********************************************************************************

	S_API inline SResult DirectX11RenderPipeline::ExamineRenderCommandQueue()
	{
		SP_ASSERTR(m_pEngine, S_NOTINIT);
		SP_ASSERTR(m_GeometryRenderStrategy != eGEOMRENDER_STRATEGY_EVENTS, S_INVALIDGEOMSTRATEGY, "geomStrategy=%d", m_GeometryRenderStrategy);

		SResult res = S_SUCCESS;

		if (Failure(m_Queue.Lock(false)))
		{
			return m_pEngine->LogE("Failed lock render command queue!");
		}

		SCommandDescription* pDesc;
		while (!m_Queue.ReachedEnd())
		{
			m_Queue.Pop(&pDesc);
			if (pDesc)
			{
				if (m_GeometryRenderStrategy = eGEOMRENDER_STRATEGY_HYBRID)
				{
					// DirectX11RenderPipeline::ExamineRenderCommandHybrid()
					if (Failure(ExamineRenderCommandHybrid(pDesc)))
						res = m_pEngine->LogE("ExamineRenderCommandHybrid failed!");
				}
				else // eGEOMRENDER_STRATEGY_COMMANDS
				{
					// DirectX11RenderPipeline::ExamineRenderCommand()
					if (Failure(ExamineRenderCommand(pDesc)))
						res = m_pEngine->LogE("ExamineRenderCommand failed!");
				}
			}
		}

		m_Queue.Unlock();

		return res;
	}

	// **********************************************************************************

	S_API SResult DirectX11RenderPipeline::RenderSolidGeometry(ISolid* pSolid, bool bTextured)
	{
		SResult res;
		SP_ASSERTR(IsInitialized(), S_NOTINIT);
		SP_ASSERTXR(GetCurrentStage() == S_RENDER_GEOMETRY, S_INVALIDSTAGE, m_pEngine, "curstage: %d", (int)GetCurrentStage());

		// Render the solid geometry
		if (Failure(m_GeometryRenderSection.RenderSolidGeometry(pSolid, false)))
			return S_ERROR;

		return S_SUCCESS;
	}

	// **********************************************************************************

	S_API SResult DirectX11RenderPipeline::ExitGeometrySection()
	{
		SP_ASSERTR(IsInitialized(), S_NOTINIT);
		SP_ASSERTDXR(GetCurrentStage() == S_RENDER_GEOMETRY, S_INVALIDSTAGE, m_pEngine, "curstage: %d", (int)GetCurrentStage());

		// Fire event
		SEventParameters params;
		params.AddSenderParam(this);
		if (Failure(m_pFramePipeline->CallEvent(S_E_RENDER_GEOMETRY_EXIT, &params)))
			return S_ERROR;

		// Exit the Geometry section
		if (Failure(m_GeometryRenderSection.EndSection()))
			return S_ERROR;

		return S_SUCCESS;
	}


	// **********************************************************************************

	S_API SResult DirectX11RenderPipeline::DoLightingSection()
	{
		SP_ASSERTR(IsInitialized(), S_NOTINIT);
		SP_ASSERTXR(GetCurrentStage() == S_RENDER_LIGHTING, S_INVALIDSTAGE, m_pEngine);

		// Setup the lighting section
		if (Failure(m_LightingRenderSection.PrepareSection()))
			return S_ERROR;

		// Setup light source buffer
		SPool<SLight*>* pLights = 0;

		// fire preparation event
		SEventParameters params;
		params.AddSenderParam(this);
		params.Add(ePARAM_LIGHTSOURCES, S_PARAMTYPE_PTR, (void*)&pLights);
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

		params.Clear();
		return S_SUCCESS;
	}

	// **********************************************************************************

	S_API SResult DirectX11RenderPipeline::DoPost()
	{
		SP_ASSERTR(IsInitialized(), S_NOTINIT);
		SP_ASSERTXR(GetCurrentStage() == S_RENDER_POST, S_INVALIDSTAGE, m_pEngine);

		// Render the output plane to the backbuffer
		if (Failure(m_PostRenderSection.RenderOutputPlane(&m_LightingRenderSection)))
			return S_ERROR;

		return S_SUCCESS;
	}

	// **********************************************************************************

	S_API SResult DirectX11RenderPipeline::DoEndRendering()
	{
		SP_ASSERTR(IsInitialized(), S_NOTINIT);
		SP_ASSERTXR(GetCurrentStage() == S_RENDER_POST, S_INVALIDSTAGE, m_pEngine);

		// exit the DX Scene
		if (Failure(m_pDXRenderer->EndScene()))
			return S_ERROR;



		// present the backbuffer
		if (Failure(m_pDXRenderer->PresentTargetViewport()))
			return S_ERROR;




		return S_SUCCESS; // okay, done.
	}
}