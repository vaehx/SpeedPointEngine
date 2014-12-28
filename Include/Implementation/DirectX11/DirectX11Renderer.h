//////////////////////////////////////////////////////////////////////////////////////////////////
//
//	This file is part of the SpeedPoint Game Engine
//
//	written by Pascal R. aka iSmokiieZz
//	(c) 2011-2014, All rights reserved.
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

	IDXGIFactory* m_pDXGIFactory;

	vector<IDXGIAdapter*> m_vAdapters;	// enumeration list of all possible adapters
	DXGI_MODE_DESC m_AutoSelectedDisplayModeDesc;	// automatically selected in AutoSelectAdapter()
	DXGI_ADAPTER_DESC m_AutoSelectedAdapterDesc;	
	IDXGIAdapter* m_pAutoSelectedAdapter;	// dont release this. it will be released in m_vAdapters! possibly use shared_ptr
		
	//IRenderPipeline* m_pRenderPipeline;
		
	DirectX11Viewport m_Viewport;

	SRenderTargetCollection m_CurrentRenderTargetCollection;	// RTs to be stored
	map<ERenderTargetCollectionID, SRenderTargetCollection>* m_pRenderTargetCollections;
	
	IViewport* m_pTargetViewport;
	DirectX11FBO* m_pTargetFBO;	

	ERenderTargetCollectionID m_iCurRTCollection;	// RT collection that is currently bound

	ID3D11DepthStencilState* m_pDepthStencilState;

	SPerSceneConstantBuffer m_PerSceneCB;
	SPerObjectConstantBuffer m_PerObjectCB;	
	ID3D11Buffer* m_pPerSceneCB;
	ID3D11Buffer* m_pPerObjectCB;
	ID3D11Buffer* m_pTerrainCB;	// instance of STerrainConstantBuffer stored in Terrain Render Desc
	bool m_bPerObjectCBBound;


	EPrimitiveType m_SetPrimitiveType;


	DirectX11Texture m_DummyTexture;
	DirectX11Texture m_DummyNormalMap;	// contains pure (128,128,0) color.

	// The Frame Buffer objects

	DirectX11FBO m_GBufferDepth;
	DirectX11FBO m_GBufferNormal;

	DirectX11FBO m_LightAccumulation;


	// The required shaders

	DirectX11Effect m_GBufferEffect;
	DirectX11Effect m_LightingEffect;
	DirectX11Effect m_DeferredMergeEffect;
	
	DirectX11Effect m_ForwardEffect;
	DirectX11Effect m_TerrainEffect;


	bool m_bInScene;
	map<usint32, SRenderDesc>* m_pRenderSchedule;	// The Render schedule filled by RenderGeometrical()
	usint32 m_RenderScheduleIDCounter;	// counter for the id used to resort the render schedule to get the highest performance while rendering
	STerrainRenderDesc m_TerrainRenderDesc;





	// TODO: Do this framedump stuff way better (e.g. by using a macro)

	bool m_bDumpFrame;	





	SResult UpdateRasterizerState();

	SResult DrawTerrain(const SDrawCallDesc& dcd);

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

	SResult AutoSelectAdapter(usint32 nW, usint32 nH);
	SResult SetRenderStateDefaults(void);

	SResult InitDefaultViewport(HWND hWnd, int nW, int nH);		


	SResult CreateDX11Device();

	ID3D11Device* GetD3D11Device() const { return m_pD3DDevice; }
	ID3D11DeviceContext* GetD3D11DeviceContext() const { return m_pD3DDeviceContext; }

	DXGI_MODE_DESC GetD3D11AutoSelectedDisplayModeDesc() { return m_AutoSelectedDisplayModeDesc; }

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

	////////////////////////////////////////////////////////////////////////////
	// Derived:

	virtual S_RENDERER_TYPE GetType(void);

	virtual SResult GetAutoSelectedDisplayModeDesc(SDisplayModeDescription* pDesc);

	// Note:
	//	!! bIgnoreAdapter is not evaluated in the D3D11Renderer !!	
	virtual SResult Initialize(IGameEngine* pEngine, HWND hWnd, int nW, int nH, bool bIgnoreAdapter);

	virtual bool IsInited(void);
	virtual SResult Shutdown(void);

	// Note:
	//	Not implemented yet!
	virtual bool AdapterModeSupported(usint32 nW, usint32 nH)
	{
		return false;
	}

	virtual SResult AddRTCollectionFBO(usint32 index, IFBO* pFBO);
	virtual SResult StoreRTCollection(ERenderTargetCollectionID asId);
	virtual SResult BindRTCollection(ERenderTargetCollectionID collcetionID);
	
	virtual SResult BindSingleFBO(IFBO* pFBO);		
	virtual SResult BindSingleFBO(IViewport* pViewport);

	virtual SResult BindTexture(ITexture* pTex, usint32 lvl = 0);
	virtual SResult BindTexture(IFBO* pFBO, usint32 lvl = 0);

	// Description:
	//	creates an additional swap chain for the same window as the default viewport
	virtual SResult CreateAdditionalViewport(IViewport** pViewport, const SViewportDescription& desc);	

	virtual SResult BeginScene(void);
	virtual SResult EndScene(void);

	virtual SResult PresentTargetViewport(void);

	virtual SResult SetVBStream(IVertexBuffer* pVB, unsigned int index = 0);
	virtual SResult SetIBStream(IIndexBuffer* pIB);

	virtual SResult ClearBoundRTs(void);

	virtual SResult SetTargetViewport(IViewport* pViewport);
	virtual IViewport* GetTargetViewport(void);

	virtual IViewport* GetDefaultViewport(void);	



	virtual SResult RenderGeometry(const SRenderDesc& dsc);
	virtual SResult RenderTerrain(const STerrainRenderDesc& tdsc);
	
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
	virtual SResult SetViewportMatrices(IViewport* pViewport = 0);
	virtual SResult SetViewportMatrices(const SMatrix& mtxView, const SMatrix& mtxProj);
	virtual SResult SetWorldMatrix(const STransformationDesc& transform);
};




//////////////////////////////////////////////////////////////////////////////////////////////////

SP_NMSPACE_END