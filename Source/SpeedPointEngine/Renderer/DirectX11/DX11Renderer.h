//////////////////////////////////////////////////////////////////////////////////////////////////
//
//	This file is part of the SpeedPoint Game Engine
//
//	written by Pascal R. aka iSmokiieZz
//	(c) 2011-2015, All rights reserved.
//
//////////////////////////////////////////////////////////////////////////////////////////////////

#pragma once

#include "DX11Viewport.h"
#include "DX11FBO.h"
#include "DX11Shader.h"
#include "DX11Texture.h"
#include "DX11ConstantsBuffer.h"
#include "..\IRenderer.h"
#include <Common\ChunkedObjectPool.h>
#include <Common\SPrerequisites.h>

// DirectX11 Specific headers
#include "DX11.h"

#include <map>

using std::vector;
using std::map;
using std::pair;

SP_NMSPACE_BEG


//////////////////////////////////////////////////////////////////////////////////////////////////
// Forward declarations

struct S_API IResourcePool;
class S_API DX11FontRenderer;


//////////////////////////////////////////////////////////////////////////////////////////////////
enum EDX11RenderBudgetTimer
{
	eDX11_BUDGET_DRAW_FORWARD_SUBSETS = 0,
	eDX11_BUDGET_UNLEASH_FONT_SCHEDULE,
	eDX11_BUDGET_DRAW_FORWARD
};

#define NUM_DIRECTX11_BUDGET_TIMERS 3


//////////////////////////////////////////////////////////////////////////////////////////////////

enum EDepthTestFunction
{
	eDEPTH_TEST_LESS,
	eDEPTH_TEST_GREATER
};

//////////////////////////////////////////////////////////////////////////////////////////////////
// SpeedPoint DirectX 11 Renderer Implementation

#define MAX_BOUND_SHADER_RESOURCES 8

class S_API DX11Renderer : public IRenderer
{
private:
	IResourcePool* m_pResourcePool;
	DX11FontRenderer* m_pFontRenderer;
	SRendererInitParams m_InitParams;

	ID3D11Device* m_pD3DDevice;
	ID3D11DeviceContext* m_pD3DDeviceContext;
	D3D_FEATURE_LEVEL m_D3DFeatureLevel;

	IDXGIFactory1* m_pDXGIFactory;
	vector<IDXGIAdapter1*> m_vAdapters;	// enumeration list of all possible adapters
	DXGI_MODE_DESC m_AutoSelectedDisplayModeDesc;	// automatically selected in AutoSelectAdapter()
	DXGI_ADAPTER_DESC m_AutoSelectedAdapterDesc;
	IDXGIAdapter1* m_pAutoSelectedAdapter;	// dont release this. it will be released in m_vAdapters! possibly use shared_ptr

	// Rasterizer state
	D3D11_RASTERIZER_DESC m_rsDesc;	
	ID3D11RasterizerState* m_pRSState;

	// Sampler states
	ID3D11SamplerState* m_pBilinearSamplerState;
	ID3D11SamplerState* m_pPointSamplerState;
	ID3D11SamplerState* m_pShadowSamplerState;	

	// Blend states
	D3D11_BLEND_DESC m_DefBlendDesc;
	ID3D11BlendState* m_pDefBlendState;

	D3D11_BLEND_DESC m_AlphaTestBlendDesc;
	ID3D11BlendState* m_pAlphaTestBlendState;

	D3D11_BLEND_DESC m_DeferredLightBlendDesc;
	ID3D11BlendState* m_pDeferredLightBlendState;

	ID3D11BlendState* m_pSetBlendState;

	// Depth Stencil states
	D3D11_DEPTH_STENCIL_DESC m_depthStencilDesc;
	ID3D11DepthStencilState* m_pDepthStencilState;

	D3D11_DEPTH_STENCIL_DESC m_terrainDepthDesc;
	ID3D11DepthStencilState* m_pTerrainDepthState;


	bool m_bFullscreen;

	DX11Viewport m_Viewport;
	IViewport* m_pTargetViewport;

	vector<DX11FBO*> m_BoundRenderTargets;
	ID3D11DepthStencilView* m_pBoundDSV;
	bool m_bBoundDSVReadonly;
	




	ConstantsBufferHelper<SSceneConstants> m_SceneConstants;
	ConstantsBufferHelper<STerrainConstants> m_TerrainConstants;	
	
	ID3D11Buffer* m_pBoundCB; // Bound per-object CB, scene cb is assumed to be always bound


	// States:	
	EPrimitiveType m_SetPrimitiveType;
	EShaderPassType m_EnabledShaderPass;

	ID3D11ShaderResourceView* m_BoundVSResources[MAX_BOUND_SHADER_RESOURCES];
	ID3D11ShaderResourceView* m_BoundPSResources[MAX_BOUND_SHADER_RESOURCES];
	unsigned int m_iMaxBoundVSResource;
	unsigned int m_iMaxBoundPSResource;

	ITexture *m_pDummyTexture;
	ITexture *m_pDummyNormalMap;	// contains pure (128,128,0) color.

	IShaderPass* m_Passes[NUM_SHADERPASS_TYPES];
	EShaderPassType m_CurrentPass;

	bool m_bInScene;

	SRenderDesc m_FullscreenPlane;

	STerrainRenderDesc m_TerrainRenderDesc;
	D3D11_BLEND_DESC m_TerrainBlendDesc;	
	ID3D11BlendState* m_pTerrainBlendState;

	ChunkedObjectPool<SFontRenderSlot, 20> m_FontRenderSchedule;


	// Budgeting:
	SRenderBudgetTimer m_BudgetTimers[NUM_DIRECTX11_BUDGET_TIMERS]; // enumerated by EDirectX11RenderBudgetTimer	



	// TODO: Do this framedump stuff way better (e.g. by using a macro)

	bool m_bDumpFrame;	




	void InitShaderPasses();
	void InitBlendStates();
	void InitFullscreenQuad();

	SResult UpdateRasterizerState();
	SResult UpdateDepthStencilState();
		
	void BindSceneCB(const IConstantsBuffer* cb);

	// Returns the lvl which the srv is bound on or -1 if it is not bound
	int IsBoundAsTexture(ID3D11ShaderResourceView* srv);

	// Does not apply blend state etc., called from public Render() methods
	void RenderGeometry(const SRenderDesc& renderDesc, bool overrideBlendState = true);

	SResult DrawTerrainSubset(const STerrainDrawCallDesc& dcd);
	
	// overrideBlendState - if false, will NOT override the currently set blend state
	SResult DrawSubsets(const SRenderDesc& renderDesc, bool overrideBlendState);

	void SetEyePosition(const Vec3f& eyePos);
	void SetDepthTestFunction(EDepthTestFunction depthTestFunc);

	// timer - enumerated by EDirectX11RenderBudgetTimer
	void StartOrResumeBudgetTimer(unsigned int timer, const char* name);
	void StopBudgetTimer(unsigned int timer);
	void ResetBudgetTimerStats();

public:	
	void FrameDump(const string& msg)
	{
		if (m_bDumpFrame)
			CLog::Log(S_DEBUG, msg);
	}
	void FrameDump(const string& str, const string& strname)
	{
		if (m_bDumpFrame)
			CLog::Log(S_DEBUG, "str %s: \"%s\"", strname.c_str(), str.c_str());
	}
	void FrameDump(const Mat44& mtx, const string& mtxname)
	{
		if (m_bDumpFrame)
		{
			CLog::Log(S_DEBUG, "mtx %s: ", mtxname.c_str());
			CLog::Log(S_DEBUG, "  (%.2f, %.2f, %.2f, %.2f)", mtx._11, mtx._12, mtx._13, mtx._14);
			CLog::Log(S_DEBUG, "  (%.2f, %.2f, %.2f, %.2f)", mtx._21, mtx._22, mtx._23, mtx._24);
			CLog::Log(S_DEBUG, "  (%.2f, %.2f, %.2f, %.2f)", mtx._31, mtx._32, mtx._33, mtx._34);
			CLog::Log(S_DEBUG, "  (%.2f, %.2f, %.2f, %.2f)", mtx._41, mtx._42, mtx._43, mtx._44);
		}
	}
	void FrameDump(const SVector3& vec, const string& vecname)
	{
		if (m_bDumpFrame)
			CLog::Log(S_DEBUG, "vec %s: (%.2f, %.2f, %.2f)", vecname.c_str(), vec.x, vec.y, vec.z);
	}
	void FrameDump(unsigned int i, const string& intname)
	{
		if (m_bDumpFrame)
			CLog::Log(S_DEBUG, "int %s: %d", intname.c_str(), i);
	}
	void FrameDump(float f, const string& floatname)
	{
		if (m_bDumpFrame)
			CLog::Log(S_DEBUG, "flt %s: %.4f", floatname.c_str(), f);
	}
	void FrameDump(bool b, const string& boolname)
	{
		if (m_bDumpFrame)
			CLog::Log(S_DEBUG, "bol %s: %s", boolname.c_str(), (b ? "true" : "false"));
	}



	DX11Renderer();
	~DX11Renderer();

	////////////////////////////////////////////////////////////////////////////
	// Specific to the DirectX Implementation

	SResult AutoSelectAdapter();
	SResult SetRenderStateDefaults(void);

	SResult InitDefaultViewport(HWND hWnd, int nW, int nH, float zNear = 0.2f, float zFar = 100.0f);		


	SResult CreateDX11Device();

	ID3D11Device* GetD3D11Device() const { return m_pD3DDevice; }
	ID3D11DeviceContext* GetD3D11DeviceContext() const { return m_pD3DDeviceContext; }

	DXGI_MODE_DESC GetD3D11AutoSelectedDisplayModeDesc() { return m_AutoSelectedDisplayModeDesc; }
	IDXGIAdapter1* GetAutoSelectedAdapter()
	{
		return m_pAutoSelectedAdapter;
	}

	SResult D3D11_CreateSwapChain(DXGI_SWAP_CHAIN_DESC* pDesc, IDXGISwapChain** ppSwapChain);
	SResult D3D11_CreateRTV(ID3D11Resource* pBBResource, D3D11_RENDER_TARGET_VIEW_DESC* pDesc, ID3D11RenderTargetView** ppRTV);			

	SResult D3D11_CreateConstantsBuffer(ID3D11Buffer** ppCB, usint32 customByteSize);
	SResult D3D11_LockConstantsBuffer(ID3D11Buffer* pCB, void** pData);	
	SResult D3D11_UnlockConstantsBuffer(ID3D11Buffer* pCB);
	SResult InitConstantBuffers();

	void D3D11_SetBlendState(ID3D11BlendState* pBlendState, const float blendFactor[4] = 0, UINT sampleMask = 0xffffffff);

	SResult UnleashFontRenderSchedule();

	////////////////////////////////////////////////////////////////////////////
	// Derived:

	virtual S_RENDERER_TYPE GetType(void);

	virtual const SRendererInitParams& GetParams() const { return m_InitParams; }

	virtual SResult GetAutoSelectedDisplayModeDesc(SDisplayModeDescription* pDesc);

	virtual SResult Initialize(const SRendererInitParams& params);

	virtual IFontRenderer* GetFontRenderer() const;

	virtual bool IsInited(void);
	virtual SResult Shutdown(void);

	// Note:
	//	Not implemented yet!
	virtual bool AdapterModeSupported(usint32 nW, usint32 nH)
	{
		return false;
	}

	virtual IFBO* CreateRT()
	{
		return new DX11FBO(this);
	}
	
	virtual SResult BindRTCollection(const std::vector<IFBO*>& fboCollection, IFBO* depthFBO, bool depthReadonly = false, const char* dump_name = 0);
	virtual SResult BindSingleRT(IFBO* pFBO, bool depthReadonly = false);
	virtual SResult BindSingleRT(IViewport* pViewport);

	virtual bool BoundMultipleRTs() const
	{
		return (m_BoundRenderTargets.size() > 1);
	}

	virtual vector<IFBO*> GetBoundRTs() const;

	virtual SResult BindVertexShaderTexture(ITexture* pTex, usint32 lvl = 0);
	virtual SResult BindTexture(ITexture* pTex, usint32 lvl = 0);
	virtual SResult BindTexture(IFBO* pFBO, usint32 lvl = 0);
	virtual SResult BindDepthBufferAsTexture(IFBO* pFBO, usint32 lvl = 0);
	virtual void UnbindTexture(usint32 lvl);
	virtual void UnbindTexture(ITexture* pTexture);

	virtual ITexture* GetDummyTexture() const;
	virtual ITexture* GetDummyNormalmap() const;

	// Description:
	//	creates an additional swap chain for the same window as the default viewport
	virtual SResult CreateAdditionalViewport(IViewport** pViewport, const SViewportDescription& desc);	

	virtual SResult BeginScene(void);
	virtual SResult EndScene(void);

	virtual IShader* CreateShader() const;

	virtual IShaderPass* BindShaderPass(EShaderPassType type);
	virtual IShaderPass* GetShaderPass(EShaderPassType type) const;
	virtual IShaderPass* GetCurrentShaderPass() const;

	virtual void ReloadShaders() const;

	virtual string GetShaderPath(EShaderFileType type) const;

	virtual SResult Render(const SRenderDesc& renderDesc);
	virtual SResult RenderInstanced(const SInstancedRenderDesc& renderDesc);
	virtual SResult RenderTerrain(const STerrainRenderDesc& terrainRenderDesc);
	virtual SResult RenderDeferredLight(const SRenderDesc& renderDesc);
	virtual SResult RenderFullScreenQuad(bool enableDepthTest = false);

	virtual SResult PresentTargetViewport(void);

	virtual SResult SetVBStream(IVertexBuffer* pVB, unsigned int index = 0);
	virtual SResult SetIBStream(IIndexBuffer* pIB);
	virtual SResult SetInstanceStream(ITypelessInstanceBuffer* pInstanceBuffer, unsigned int index = 1);

	virtual SResult ClearRT(IFBO* pFBO, bool color = true, bool depth = true);
	virtual SResult ClearBoundRTs(bool color = true, bool depth = true);

	virtual SResult SetTargetViewport(IViewport* pViewport);
	virtual IViewport* GetTargetViewport(void);
	virtual IViewport* GetDefaultViewport(void);	


	virtual IConstantsBuffer* CreateConstantsBuffer() const;	
	virtual void BindConstantsBuffer(const IConstantsBuffer* cb, bool vs = false);
	virtual SSceneConstants* GetSceneConstants() const;
	virtual void UpdateSceneConstants();
	
	virtual STerrainRenderDesc* GetTerrainRenderDesc();

	virtual SFontRenderSlot* GetFontRenderSlot();
	virtual void ReleaseFontRenderSlot(SFontRenderSlot** pFRS);

	// Summary:
	//	Draws the given geometry desc directly to the back buffer and the depth buffer
	virtual SResult Draw(const SDrawCallDesc& desc);


	virtual SResult UpdateCullMode(EFrontFace cullmode);
	virtual SResult EnableBackfaceCulling(bool state = true);
	virtual SResult UpdatePolygonType(S_PRIMITIVE_TYPE type);
	virtual void EnableWireframe(bool state = true);
	virtual void EnableDepthTest(bool enableDepthTest = true, bool enableDepthWrite = true);

	virtual IResourcePool* GetResourcePool();

	ILINE virtual void DumpFrameOnce();
	ILINE virtual bool DumpingThisFrame() const;


	// Summary:
	//	Builds an instance of the DirectX11 Renderer.
	// Description:
	//	Used to completely encapsulate the SpeedPoint Engine Core from the DirectX11 Implementation
	static IRenderer* GetInstance()
	{
		return new DX11Renderer();
	}

protected:
	virtual void SetViewProjMatrix(IViewport* pViewport = 0);
	virtual void SetViewProjMatrix(const Mat44& mtxView, const Mat44& mtxProj);
};




//////////////////////////////////////////////////////////////////////////////////////////////////

SP_NMSPACE_END
