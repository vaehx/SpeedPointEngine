/* ========================================================================== */
/*                                                                            */
/*   Filename.c                                                               */
/*   (c) 2012 Author                                                          */
/*                                                                            */
/*   Description                                                              */
/*                                                                            */
/* ========================================================================== */



/*

	TODO:

	- Implement IRenderer::Draw() in DirectX11Renderer
	- Go on with algorithm in Solid::Render()

	- Implement IRenderer::Begin() and IRenderer::End() in DirectX11Renderer
	- Go back to Frame Pipeline and call IRenderer functions to draw all visible objects
	- Consider using a IRenderProcess to define the object-type-specific pipeline to render


*/


// ==============================================================================================================================================
// Util.h

#define SP_SAFE_DELETE_ARR(buf, len) if((buf)) { if((len) > 1) delete[] buf; else delete buf; }

// ==============================================================================================================================================
// IGame.h

struct IGame
{
	virtual void OnUpdate(float fLastFrameTime) = 0;
};

struct IGameEngine
{
	virtual void RegisterGame(IGame* pGame) = 0;
	virtual void Tick() = 0;

	virtual void ExecuteFramePipeline() = 0;
};


// ==============================================================================================================================================
// FramePipeline.h

enum EFramePipelineSectionType
{
	eFPSEC_DYNAMICS,
	eFPSEC_RENDERING
};

struct IFramePipelineSection
{
	virtual void Run() = 0;
};

struct IFramePipeline
{
	virtual void RegisterSection() = 0;

	virtual void ExecuteSections() = 0;
};

class FramePipeline : public IFramePipeline
{
private:
	std::vector<IFramePipelineSection*> m_Sections;

public:
	virtual void ExecuteSections()
	{
		for (m_Sections::iterator itSection = m_Sections.begin(); itSection != m_Sections.end(); itSection++)
		{
			itSection->Run();
		}
	}
};



// ==============================================================================================================================================
// Dynamics.h

struct IFramePipelineSection;

class DynamicsPipeline : public IFramePipelineSection
{
	virtual void Run()
	{
		// Physics
		for (uint32 iPhysicalBody; iPhysicalBody < m_pPhysics->GetNumRegisteredBodies(); ++iPhysicalBody)
		{
			IPhysicalBody* pBody = m_pPhysics->GetBodyByIndex(iPhysicalBody);
			if (!pBody)
				continue;

			pBody->OnPreUpdate();

			SIntersectionInformation* pIntersectionInformations = 0;
			uint32 nIntersections = m_pPhysics->TestIntersection(pBody, &pIntersectionInformations);
			if (nIntersections == 0)
				continue;

			pBody->OnIntersections(pIntersectionInformations, nIntersections);

			SP_SAFE_DELETE_ARR(pIntersectionInformations, nIntersections);

                        pBody->OnPostUpdate();
		}

		// AI
		for (uint32 iBot; iBot < m_pAISystem->GetNumRegisteredBots(); ++iBot)
		{
			IBot* pBot = m_pAISystem->GetBotByIndex(iBot);
			if (!pBot)
				continue;

			pBot->OnUpdate();
		}

		// Script
		for (uint32 iScript; iScript < m_pScriptSystem->GetNumActiveScripts(); ++iScript)
		{
			IScript* pScript = m_pScriptSystem->GetScriptByIndex(iScript);
			if (!pScript)
				continue;

			pScript->OnUpdate();
		}
	}
};

// ==============================================================================================================================================
// Rendering.h


struct IRenderer;
struct IShader;
enum EShaderType;
struct SShaderBindDesc;
struct IVertexBuffer;
struct IIndexBuffer;

enum EVisibleObjectType
{
	eVOBJ_SOLID,
	eVOBJ_TERRAIN,
	eVOBJ_VEGETATION,
	eVOBJ_WATER
	// ...
};

struct IVisibleObject
{
	virtual IIndexBuffer* GetIndexBuffer() = 0;
	virtual IVertexBuffer* GetVertexBuffer() = 0;

	virtual IMaterial* GetMaterial() = 0;

	virtual EVisibleObjectType GetType() = 0;

	// Calls functions of the IRenderer in an order specific to the
	// implementation of this interface
	virtual void Render(IRenderer* pRenderer) = 0;
};

enum ERenderTarget
{
	eRT_UNKNOWN = 0,
	eRT_NORMALS,
	eRT_DEPTH,

	_eRT_LAST_ENUM_ID        // Warning: Do not remove this!
};

// RenderTarget Format
enum ERTFormat
{
	eRTFMT_A8R8G8B8;
	eRTFMT_D32;
};

struct IRenderTarget
{
	virtual void SetType(ERenderTarget t) = 0;
	virtual ERenderTarget GetType() = 0;
};

// Implementation
class DirectX11RenderTarget : public IRenderTarget
{
private:
	ERenderTarget m_Type;

public:
	virtual void SetType(ERenderTarget t) { m_Type = t; }
	virtual void GetType() { return m_Type; }
};
// ~Implementation



struct SDrawCallDescription
{
	IVertexBuffer* pVB;
	IIndexBuffer* pIB;

	uint32 iStartIBIndex;
	uint32 iEndIBIndex;
	uint32 iStartVBIndex;
	uint32 iEndVBIndex;

	SDrawCallDescription()
		: pVB(0), pIB(0)
	{
	}
};

/*
- to change the format of a specific RT use
pRenderer->InitRenderTarget(eRT_NORMALS, eRTFMT_A8R8G8B8);
*/
struct IRenderer
{
	virtual IRenderTarget* GetRenderTarget(ERenderTarget rt) = 0;
	virtual void InitRenderTarget(IRenderTarget* pRT, ERTFormat format) = 0;
	virtual void InitRenderTarget(ERenderTarget rt, ERTFormat format) = 0;
	virtual void InitRTs() = 0;
	virtual void ClearBindedRenderTargets() = 0;
	virtual void BindRenderTarget(ERenderTarget rt) = 0;

	virtual void RegisterShaderImplementation(IShader* pShaderInstance, const EShaderType& t) = 0;
	virtual void SetActiveShader(const EShaderType& t, const SShaderBindDesc& bindDesc) = 0;

	virtual void Begin() = 0;

	virtual void RenderObject(IVisibleObject* pObj) = 0;
	virtual void Draw(SDrawCallDescription* pDrawCallDesc) = 0;

	virtual void End() = 0;
};


// Implementation
class DirectX11Renderer : public IRenderer
{
private:
	IRenderTarget* m_pRenderTargets;	// array, enum IDs used to index

public:
	DirectX11Renderer()
	{
		m_pRenderTargets = new IRenderTarget[_eRT_LAST_ENUM_ID];
	}

	~DirectX11Renderer()
	{
		if (m_pRenderTargets) delete[] m_pRenderTargets;
	}

	virtual IRenderTarget* GetRenderTarget(ERenderTarget rt)
	{
		return m_pRenderTargets[(uint32)rt];
	}

	virtual void InitRenderTarget(IRenderTarget* pRT, ERTFormat format)
	{
		if (!pRT)
			pRT = new DirectX11RenderTarget(this);

		pRT->Clear();
		pRT->Init(format);
	}

	virtual void InitRenderTarget(ERenderTarget rt, ERTFormat format)
	{
		return InitRenderTarget(GetRenderTarget(rt), format);
	}

	virtual void InitRTs()
	{
		InitRenderTarget(eRT_NORMALS, 	eRTFMT_A8R8G8B8);
		InitRenderTarget(eRT_DEPTH, 	eRTFMT_D32);
	}

	virtual void RenderObject(IVisibleObject* pObj)
	{
		if (!pObj)
			return;

		// Definetly bind buffers
		BindVertexBuffer(pObj->GetVertexBuffer());
		BindIndexBuffer(pObj->GetIndexBuffer());

		return pObj->Render();
	}

        virtual void RegisterShaderImplementation(IShader* pShaderInstance, const EShaderType& t);
        virtual void SetActiveShader(const EShaderType& t)
	{
		GetShader(t)->SetActive();
	}
};
// ~Implementation



// ==============================================================================================================================================
// Shader.h

#include "Rendering.h"  // for binding render targets

enum EShaderType
{
	eSHADER_DL_GBUFFER     // Deferred Lighting GBuffer creation shader
};

struct IShader
{
	// Arguments:
	//      pEffect - Valid ptr to the renderer-specific effect
	virtual void Init(IRendererEffect* pEffect) = 0;

	virtual void SetActive() = 0;
};

// Implementation
class DeferredLightingGBufferShader : public IShader
{
private:
	IRenderer* m_pRenderer;
	IRendererEffect* m_pEffect;

public:
	DeferredLightingGBufferShader()
		: m_pRenderer(0),
		m_pEffect(0)
	{
	}

	virtual void Init(IRendererEffect* pEffect)
	{
		if (!pEffect || !pEffect->GetRenderer())
			return;

		m_pRenderer = pEffect->GetRenderer();
		m_pEffect = pEffect;
		return m_pRenderer->RegisterShaderImplementation(static_cast<IShader*>(this), eSHADER_DL_GBUFFER);
	}

	virtual void SetActive()
	{
		// bind necessary RTs here
		m_pRenderer->ClearBindedRenderTargets();
		m_pRenderer->BindRenderTarget(eRT_NORMALS);
		m_pRenderer->BindRenderTarget(eRT_DEPTH);

		m_pEffect->SetActive();
	}
};
// ~Implementation


// ==============================================================================================================================================
// Solid.h

class Solid : public IVisibleObject
{
public:
	virtual void Render(IRenderer* pRenderer)
	{
		pRenderer->SetActiveShader(eSHADER_DL_GBUFFER);

		pRenderer->Draw()







		// ??




	}
};