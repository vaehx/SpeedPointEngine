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

class S_API SpeedPointEngine;
class S_API IRenderPipeline;
class S_API ISolid;



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
	SpeedPointEngine* m_pEngine;
	DirectX11Settings m_Settings;

	ID3D11Device* m_pD3DDevice;
	ID3D11DeviceContext* m_pD3DDeviceContext;

	D3D_FEATURE_LEVEL m_D3DFeatureLevel;
	ID3D11RasterizerState* m_pDefaultRSState;

	IDXGIFactory* m_pDXGIFactory;

	vector<IDXGIAdapter*> m_vAdapters;	// enumeration list of all possible adapters
	DXGI_MODE_DESC m_AutoSelectedDisplayModeDesc;	// automatically selected in AutoSelectAdapter()
	DXGI_ADAPTER_DESC m_AutoSelectedAdapterDesc;	
	IDXGIAdapter* m_pAutoSelectedAdapter;	// dont release this. it will be released in m_vAdapters! possibly use shared_ptr
		
	IRenderPipeline* m_pRenderPipeline;
		
	DirectX11Viewport m_Viewport;

	SRenderTargetCollection m_CurrentRenderTargetCollection;	// temporary render targets that will be bound
	map<ERenderTargetCollectionID, SRenderTargetCollection>* m_pRenderTargetCollections;

	IFBO* m_pTargetViewportBackBuffer;
	IViewport* m_pTargetViewport;
	ID3D11RenderTargetView* m_pDXRenderTarget;	

	ERenderTargetCollectionID m_iCurRTCollection;

	ID3D11DepthStencilState* m_pDepthStencilState;

public:				
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

	SResult SetRenderTarget(IViewport* pViewport);
	SResult SetRenderTarget(DirectX11FBO* pFBO);

	////////////////////////////////////////////////////////////////////////////
	// Derived:

	virtual S_RENDERER_TYPE GetType(void);

	virtual SResult GetAutoSelectedDisplayModeDesc(SDisplayModeDescription* pDesc);

	// Note:
	//	!! bIgnoreAdapter is not evaluated in the D3D11Renderer !!	
	virtual SResult Initialize(SpeedPointEngine* pEngine, HWND hWnd, int nW, int nH, bool bIgnoreAdapter);

	virtual bool IsInited(void);
	virtual SResult Shutdown(void);

	// Note:
	//	Not implemented yet!
	virtual bool AdapterModeSupported(usint32 nW, usint32 nH)
	{
		return false;
	}

	virtual SResult AddBindedFBO(usint32 index, IFBO* pFBO);
	virtual SResult BindFBOs(ERenderTargetCollectionID collcetionID, bool bStore = false);
	virtual SResult BindFBO(IFBO* pFBO);

	// Description:
	//	creates an additional swap chain for the same window as the default viewport
	virtual SResult CreateAdditionalViewport(IViewport** pViewport, const SViewportDescription& desc);	

	virtual IRenderPipeline* GetRenderPipeline(void);

	virtual SResult BeginScene(void);
	virtual SResult EndScene(void);

	virtual SResult SetVBStream(IVertexBuffer* pVB, unsigned int index = 0);
	virtual SResult SetIBStream(IIndexBuffer* pIB);

	virtual SResult ClearRenderTargets(void);

	virtual SResult SetTargetViewport(IViewport* pViewport);
	virtual IViewport* GetTargetViewport(void);

	virtual IViewport* GetDefaultViewport(void);

	// Note:
	//	Doesn't actually do anything in DX11 due to removed fixed pipeline
	virtual SResult UpdateViewportMatrices(IViewport* pViewport);

	virtual SResult RenderSolid(ISolid* pSolid, bool bTextured);	

	virtual IRendererSettings* GetSettings()
	{
		return (IRendererSettings*)&m_Settings;
	}
};




//////////////////////////////////////////////////////////////////////////////////////////////////

SP_NMSPACE_END