//////////////////////////////////////////////////////////////////////////////////////////////////
//
//	This file is part of the SpeedPoint Game Engine
//
//	written by Pascal R. aka iSmokiieZz
//	(c) 2011-2015, All rights reserved.
//
//////////////////////////////////////////////////////////////////////////////////////////////////

#pragma once

#include <SPrerequisites.h>
#include <Abstract\IRenderer.h>
#include "DirectX11Settings.h"
#include "DirectX11Viewport.h"
#include "DirectX11FBO.h"
#include "DirectX11Effect.h"
#include "DirectX11Texture.h"
#include <Util\SQueue.h>
#include <Abstract\ChunkedObjectPool.h>

// DirectX11 Specific headers
#include "DirectX11.h"

#include <map>

using std::vector;
using std::map;
using std::pair;

SP_NMSPACE_BEG


//////////////////////////////////////////////////////////////////////////////////////////////////
// Forward declarations

struct S_API IGameEngine;
struct S_API IRenderPipeline;
struct S_API ISolid;
struct S_API IResourcePool;



//////////////////////////////////////////////////////////////////////////////////////////////////
// DirectX11 implementation of the device capabilities interface
class S_API DirectX11RenderDeviceCaps : public IRenderDeviceCapabilities
{
public:
	DirectX11RenderDeviceCaps();

	virtual SResult Collect(IRenderer* pRenderer);
	virtual unsigned int GetMaximumMSQuality()
	{
		return 0;
	}

private:
	unsigned int m_MaxMSQuality;
};

//////////////////////////////////////////////////////////////////////////////////////////////////
// SpeedPoint DirectX 11 Renderer Implementation

class S_API DirectX11Renderer : public IRenderer
{
private:
	IGameEngine* m_pEngine;
	DirectX11Settings m_Settings;	

	IResourcePool* m_pResourcePool;

	ID3D11Device* m_pD3DDevice;
	ID3D11DeviceContext* m_pD3DDeviceContext;

	D3D_FEATURE_LEVEL m_D3DFeatureLevel;
	D3D11_RASTERIZER_DESC m_rsDesc;	
	ID3D11RasterizerState* m_pRSState;	
	ID3D11SamplerState* m_pDefaultSamplerState;	// SamplerStates mainly set in shader, we need a default one though
	
	D3D11_BLEND_DESC m_DefBlendDesc;
	ID3D11BlendState* m_pDefBlendState;

	IDXGIFactory1* m_pDXGIFactory;

	vector<IDXGIAdapter1*> m_vAdapters;	// enumeration list of all possible adapters
	DXGI_MODE_DESC m_AutoSelectedDisplayModeDesc;	// automatically selected in AutoSelectAdapter()
	DXGI_ADAPTER_DESC m_AutoSelectedAdapterDesc;	
	IDXGIAdapter1* m_pAutoSelectedAdapter;	// dont release this. it will be released in m_vAdapters! possibly use shared_ptr

	bool m_bFullscreen;
		
	//IRenderPipeline* m_pRenderPipeline;
		
	DirectX11Viewport m_Viewport;	// Default Viewport?
	IViewport* m_pTargetViewport;

	DirectX11FBO* m_pRenderTargets[MAX_BOUND_RTS];
	ID3D11DepthStencilView* m_pDSV;	// the bound DSV
	unsigned int m_nRenderTargets;

	D3D11_DEPTH_STENCIL_DESC m_depthStencilDesc;
	ID3D11DepthStencilState* m_pDepthStencilState;	

	D3D11_DEPTH_STENCIL_DESC m_terrainDepthDesc;
	ID3D11DepthStencilState* m_pTerrainDepthState;
	
	// Todo: Do not store CBs in Renderer, but in the objects itself
	SIllumConstantBuffer m_IllumCB;
	SHelperConstantBuffer m_HelperCB;
	ID3D11Buffer* m_pIllumCB;
	ID3D11Buffer* m_pHelperCB;

	SPerSceneConstantBuffer m_PerSceneCB;
	ID3D11Buffer* m_pPerSceneCB;	
	ID3D11Buffer* m_pTerrainCB;	// instance of STerrainConstantBuffer stored in Terrain Render Desc
	//bool m_bPerObjectCBBound;
	
	ID3D11Buffer* m_pBoundCB; // Bound per-object CB, scene cb is assumed to be always bound


	// States:	
	EPrimitiveType m_SetPrimitiveType;


	DirectX11Texture m_DummyTexture;
	DirectX11Texture m_DummyNormalMap;	// contains pure (128,128,0) color.

	// The Frame Buffer objects

	DirectX11FBO m_GBufferDepth;
	DirectX11FBO m_GBufferNormal;

	DirectX11FBO m_LightAccumulation;


	// The required shaders

	DirectX11Effect m_IllumEffect;
	DirectX11Effect m_HelperEffect;
	DirectX11Effect m_TerrainEffect;


	bool m_bInScene;


	// Render Schedule	
	ChunkedObjectPool<SRenderSlot, 50> m_RenderSchedule;	
	
	STerrainRenderDesc m_TerrainRenderDesc;
	D3D11_BLEND_DESC m_TerrainBlendDesc;	
	ID3D11BlendState* m_pTerrainBlendState;

	ChunkedObjectPool<SFontRenderSlot, 20> m_FontRenderSchedule;




	// TODO: Do this framedump stuff way better (e.g. by using a macro)

	bool m_bDumpFrame;	





	void InitBlendStates();

	SResult UpdateRasterizerState();
	SResult UpdateDepthStencilState();

	SResult DrawTerrainSubset(const STerrainDrawCallDesc& dcd);

	SResult DrawForwardSubsets(const SRenderDesc& renderDesc);

	void SetEyePosition(const Vec3f& eyePos);

public:	
	void FrameDump(const SString& msg)
	{
		if (m_bDumpFrame && IS_VALID_PTR(m_pEngine))
			m_pEngine->LogD(msg);
	}
	void FrameDump(const SString& str, const SString& strname)
	{
		if (m_bDumpFrame && IS_VALID_PTR(m_pEngine))
			m_pEngine->LogD(str, strname);
	}
	void FrameDump(const SMatrix4& mtx, const SString& mtxname)
	{
		if (m_bDumpFrame && IS_VALID_PTR(m_pEngine))
			m_pEngine->LogD(mtx, mtxname);
	}
	void FrameDump(const SVector3& vec, const SString& vecname)
	{
		if (m_bDumpFrame && IS_VALID_PTR(m_pEngine))
			m_pEngine->LogD(vec, vecname);
	}
	void FrameDump(unsigned int i, const SString& intname)
	{
		if (m_bDumpFrame && IS_VALID_PTR(m_pEngine))
			m_pEngine->LogD(i, intname);
	}
	void FrameDump(float f, const SString& floatname)
	{
		if (m_bDumpFrame && IS_VALID_PTR(m_pEngine))
			m_pEngine->LogD(f, floatname);
	}
	void FrameDump(bool b, const SString& boolname)
	{
		if (m_bDumpFrame && IS_VALID_PTR(m_pEngine))
			m_pEngine->LogD(b, boolname);
	}



	DirectX11Renderer();
	~DirectX11Renderer();

	////////////////////////////////////////////////////////////////////////////
	// Specific to the DirectX Implementation

	SResult AutoSelectAdapter();
	SResult SetRenderStateDefaults(void);

	SResult InitDefaultViewport(HWND hWnd, int nW, int nH);		


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

	// Arguments:
	//	pTerrainCB - only required if cb == CONSTANTBUFFER_TERRAIN
	SResult UpdateConstantBuffer(EConstantBufferType cb, const STerrainConstantBuffer* pTerrainCB = nullptr);

	// Draw all things schedule in the render schedule
	SResult UnleashRenderSchedule();

	SResult UnleashFontRenderSchedule();

	////////////////////////////////////////////////////////////////////////////
	// Derived:

	virtual S_RENDERER_TYPE GetType(void);

	virtual SResult GetAutoSelectedDisplayModeDesc(SDisplayModeDescription* pDesc);

	// Note:
	//	!! bIgnoreAdapter is not evaluated in the D3D11Renderer !!	
	virtual SResult Initialize(IGameEngine* pEngine, bool bIgnoreAdapter);

	virtual IFontRenderer* InitFontRenderer();

	virtual IGameEngine* GetEngine()
	{
		return m_pEngine;
	}

	virtual bool IsInited(void);
	virtual SResult Shutdown(void);

	// Note:
	//	Not implemented yet!
	virtual bool AdapterModeSupported(usint32 nW, usint32 nH)
	{
		return false;
	}

	/*
	virtual SResult AddRTCollectionFBO(usint32 index, IFBO* pFBO);
	virtual SResult StoreRTCollection(ERenderTargetCollectionID asId);
	virtual SResult BindRTCollection(ERenderTargetCollectionID collcetionID);
	*/
	
	virtual SResult BindSingleRT(IFBO* pFBO);
	virtual SResult BindSingleRT(IViewport* pViewport);
	
	virtual SResult BindRTCollection(std::vector<IFBO*>& fboCollection, IFBO* depthFBO, const char* dump_name = 0);

	virtual IFBO* GetBoundSingleRT();

	virtual bool BoundMultipleRTs() const
	{
		return (m_nRenderTargets > 1);
	}	



	virtual SResult BindTexture(ITexture* pTex, usint32 lvl = 0, bool vs = false);
	virtual SResult BindTexture(IFBO* pFBO, usint32 lvl = 0);

	// Description:
	//	creates an additional swap chain for the same window as the default viewport
	virtual SResult CreateAdditionalViewport(IViewport** pViewport, const SViewportDescription& desc);	

	virtual SResult BeginScene(void);
	virtual SResult EndScene(void);

	virtual SResult Render(const SRenderDesc& renderDesc);
	virtual SResult RenderTerrain(const STerrainRenderDesc& terrainRenderDesc);

	virtual SResult RenderDeferredLight(const SLightDesc& light);

	virtual SResult PresentTargetViewport(void);

	virtual SResult SetVBStream(IVertexBuffer* pVB, unsigned int index = 0);
	virtual SResult SetIBStream(IIndexBuffer* pIB);

	virtual SResult ClearBoundRTs(void);

	virtual SResult SetTargetViewport(IViewport* pViewport);
	virtual IViewport* GetTargetViewport(void);

	virtual IViewport* GetDefaultViewport(void);	





	/*
	virtual SResult RenderGeometry(const SRenderDesc& dsc);
	virtual SResult RenderTerrain(const STerrainRenderDesc& tdsc);	
	*/
	
	virtual SRenderSlot* GetRenderSlot();
	virtual void ReleaseRenderSlot(SRenderSlot** pSlot);
	virtual STerrainRenderDesc* GetTerrainRenderDesc();

	virtual SFontRenderSlot* GetFontRenderSlot();
	virtual void ReleaseFontRenderSlot(SFontRenderSlot** pFRS);

	// Summary:
	//	Draws the given geometry desc to the GBuffer and its depth buffer
	virtual SResult DrawDeferred(const SDrawCallDesc& desc);
	virtual SResult DrawDeferredLighting();
	virtual SResult MergeDeferred();

	// Summary:
	//	Draws the given geometry desc directly to the back buffer and the depth buffer
	virtual SResult DrawForward(const SDrawCallDesc& desc);


	virtual SResult UpdateSettings(const SSettingsDesc& dsc);

	virtual IRendererSettings* GetSettings()
	{		
		return (IRendererSettings*)&m_Settings;
	}

	virtual SResult UpdateCullMode(EFrontFace cullmode);
	virtual SResult EnableBackfaceCulling(bool state = true);
	virtual SResult UpdatePolygonType(S_PRIMITIVE_TYPE type);
	virtual void EnableWireframe(bool state = true);
	virtual void EnableDepthTest(bool state = true);

	virtual IResourcePool* GetResourcePool();

	virtual void DumpFrameOnce()
	{
		m_bDumpFrame = true;
	}


	// Summary:
	//	Builds an instance of the DirectX11 Renderer.
	// Description:
	//	Used to completely encapsulate the SpeedPoint Engine Core from the DirectX11 Implementation
	static IRenderer* GetInstance()
	{
		return new DirectX11Renderer();
	}

protected:
	virtual void SetViewProjMatrix(IViewport* pViewport = 0);
	virtual void SetViewProjMatrix(const SMatrix& mtxView, const SMatrix& mtxProj);
	virtual void SetViewProjMatrix(const SMatrix& mtxViewProj);	

	// Returns false if setting shader resources failed and the object should not be rendered.
	virtual bool SetShaderResources(const SShaderResources& shaderResources, const SMatrix4& worldMat);
};




//////////////////////////////////////////////////////////////////////////////////////////////////

SP_NMSPACE_END