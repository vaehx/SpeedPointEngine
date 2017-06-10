//////////////////////////////////////////////////////////////////////////////////////////////////
//
//	SpeedPoint Game Engine
//	Copyright (c) 2011-2017 Pascal Rosenkranz, All rights reserved.
//
//////////////////////////////////////////////////////////////////////////////////////////////////

#include "DX11Renderer.h"
#include "DX11ResourcePool.h"
#include "DX11Utilities.h"
#include "DX11VertexBuffer.h"
#include "DX11IndexBuffer.h"
#include "DX11InstanceBuffer.h"
#include "DX11Texture.h"
#include "DX11FBO.h"
#include "DX11Font.h"
#include "DX11ConstantsBuffer.h"
#include <Common\SVertex.h>
#include <Common\SColor.h>
#include <Common\ProfilingSystem.h>

#include <initguid.h>
#include <dxgi1_3.h>
#include <dxgidebug.h>

SP_NMSPACE_BEG

#ifndef EXEC_CONDITIONAL
#define EXEC_CONDITIONAL(res, exec) if (SpeedPoint::Success(res)) { res = exec; }
#endif

using ::std::vector;


//////////////////////////////////////////////////////////////////////////////////////////////////
// DX11Renderer

S_API DX11Renderer::DX11Renderer()
: m_pResourcePool(0),
m_pFontRenderer(0),
m_pD3DDevice(0),
m_pD3DDeviceContext(0),
m_pAutoSelectedAdapter(0),
m_pTargetViewport(0),
m_pDXGIFactory(0),
m_pBoundDSV(0),
m_pDefBlendState(0),
m_pAlphaTestBlendState(0),
m_pTerrainBlendState(0),
m_pSetBlendState(0),
m_pDepthStencilState(0),
m_pTerrainDepthState(0),
m_pBilinearSamplerState(0),
m_pPointSamplerState(0),
m_pShadowSamplerState(0),
m_pBoundCB(nullptr),
m_bInScene(false),
m_bDumpFrame(false),
m_SetPrimitiveType(PRIMITIVE_TYPE_UNKNOWN),
m_iMaxBoundVSResource(0),
m_iMaxBoundPSResource(0)
{
	for (int i = 0; i < NUM_SHADERPASS_TYPES; ++i)
		m_Passes[i] = 0;
};

// -----------------------------------------------------------------------------------------------
S_API DX11Renderer::~DX11Renderer()
{
	Shutdown();
}

// -----------------------------------------------------------------------------------------------
S_API void DX11Renderer::DumpFrameOnce()
{
	m_bDumpFrame = true;
}

// -----------------------------------------------------------------------------------------------
S_API bool DX11Renderer::DumpingThisFrame() const
{
	return m_bDumpFrame;
}

// -----------------------------------------------------------------------------------------------
S_API void DX11Renderer::StartOrResumeBudgetTimer(unsigned int timer, const char* name)
{
	if (timer >= NUM_DIRECTX11_BUDGET_TIMERS)
		return;

	SRenderBudgetTimer& renderBudgetTimer = m_BudgetTimers[timer];
	if (renderBudgetTimer.numUsed == 0)
		renderBudgetTimer.timerId = ProfilingSystem::StartSection(name);
	else
		ProfilingSystem::ResumeSection(renderBudgetTimer.timerId);

	renderBudgetTimer.numUsed++;
}

// -----------------------------------------------------------------------------------------------
S_API void DX11Renderer::StopBudgetTimer(unsigned int timer)
{
	if (timer >= NUM_DIRECTX11_BUDGET_TIMERS)
		return;

	ProfilingSystem::EndSection(m_BudgetTimers[timer].timerId);
}

// -----------------------------------------------------------------------------------------------
S_API void DX11Renderer::ResetBudgetTimerStats()
{
	memset(m_BudgetTimers, 0, NUM_DIRECTX11_BUDGET_TIMERS * sizeof(SRenderBudgetTimer));
	/*for (unsigned int i = 0; i < NUM_DIRECTX11_BUDGET_TIMERS; ++i)
	{
		m_BudgetTimers[i].numUsed = 0;
	}*/
}


// -----------------------------------------------------------------------------------------------
S_API IResourcePool* DX11Renderer::GetResourcePool()
{
	if (m_pResourcePool == 0)
	{
		m_pResourcePool = new DX11ResourcePool();
		m_pResourcePool->Initialize(this);
	}

	return m_pResourcePool;
}

// -----------------------------------------------------------------------------------------------
S_API SResult DX11Renderer::AutoSelectAdapter()
{
	HRESULT hRes = S_OK;

	// Create and Validate the DXGI Factory
	CreateDXGIFactory1(__uuidof(IDXGIFactory1), (void**)&m_pDXGIFactory);
	SP_ASSERTR(m_pDXGIFactory, S_ERROR);


	// Load and save all possible adapters
	IDXGIAdapter1* pAdapter;
	usint32 iBestAdapter = 0;
	DXGI_ADAPTER_DESC bestAdapterDesc;
	bestAdapterDesc.DedicatedVideoMemory = 0;
	for (usint32 iAdapter = 0; m_pDXGIFactory->EnumAdapters1(iAdapter, &pAdapter) != DXGI_ERROR_NOT_FOUND; ++iAdapter)
	{
		DXGI_ADAPTER_DESC adapterDesc;
		pAdapter->GetDesc(&adapterDesc);

		/*
		wchar_t adapterTxt[1024];
		memset(adapterTxt, 0, 1024);
		swprintf_s(adapterTxt, L"%s\nsharedSysMem: %.3f MB\ndedicatedSysMem: %.3f MB\ndedicatedVidMem: %.3f MB",
			adapterDesc.Description,
			(float)adapterDesc.SharedSystemMemory / (1024.0f * 1024.0f),
			(float)adapterDesc.DedicatedSystemMemory / (1024.0f * 1024.0f),
			(float)adapterDesc.DedicatedVideoMemory / (1024.0f * 1024.0f));

		MessageBoxW(0, adapterTxt, L"Found adapter", MB_ICONINFORMATION | MB_OK);
		*/

		if (bestAdapterDesc.DedicatedVideoMemory < adapterDesc.DedicatedVideoMemory)
		{
			iBestAdapter = iAdapter;
			memcpy(&bestAdapterDesc, &adapterDesc, sizeof(DXGI_ADAPTER_DESC));
		}

		m_vAdapters.push_back(pAdapter);
	}

	SP_ASSERTR(m_vAdapters.size() > 0, S_ERROR, "No adapters found!");


	// In fullscreen mode:
	//	Find adapter with matching size.
	// In Windowed mode:
	//	Find best adapter measured by memory size.

	DXGI_FORMAT desiredBackBufferFormat = DXGI_FORMAT_R8G8B8A8_UNORM;
	if (!m_InitParams.windowed)
	{
		m_bFullscreen = true;
		bool bFound = false;
		IDXGIOutput* pOutput;
		for (auto iAdapter = m_vAdapters.begin(); iAdapter != m_vAdapters.end(); ++iAdapter)
		{
			if (!(pAdapter = *iAdapter))
				continue;

			for (usint32 iOutput = 0; pAdapter->EnumOutputs(iOutput, &pOutput) != DXGI_ERROR_NOT_FOUND; ++iOutput)
			{
				usint32 nModes;
				DXGI_MODE_DESC* pModes = 0;
				hRes = pOutput->GetDisplayModeList(desiredBackBufferFormat, 0, &nModes, 0);

				if (nModes == 0)
					continue; // no matching modes found in this output

				pModes = new DXGI_MODE_DESC[nModes];
				hRes = pOutput->GetDisplayModeList(desiredBackBufferFormat, 0, &nModes, pModes);

				for (usint32 iMode = 0; iMode < nModes; ++iMode)
				{
					if (pModes[iMode].Width == m_InitParams.resolution[0] && pModes[iMode].Height == m_InitParams.resolution[1])
					{
						// okay, found matching mode, output and adapter. So store it now
						hRes = pAdapter->GetDesc(&m_AutoSelectedAdapterDesc);
						m_AutoSelectedDisplayModeDesc = pModes[iMode];
						m_pAutoSelectedAdapter = pAdapter;

						bFound = true;
						break;
					}
				}

				SP_SAFE_DELETE_ARR(pModes, nModes);
				SP_SAFE_RELEASE(pOutput);

				if (bFound)
					break;
			}

			if (bFound)
				break;
		}
		SP_ASSERTR(bFound, S_NOTFOUND, "No matching adapter or display mode found!");
	}
	else
	{
		m_AutoSelectedAdapterDesc = bestAdapterDesc;
		m_pAutoSelectedAdapter = m_vAdapters[iBestAdapter];
		m_bFullscreen = false;

		m_AutoSelectedDisplayModeDesc.Format = desiredBackBufferFormat;
		m_AutoSelectedDisplayModeDesc.Width = m_InitParams.resolution[0];
		m_AutoSelectedDisplayModeDesc.Height = m_InitParams.resolution[1];

		// In windowed mode RefreshRate, Scaling and ScanlineOrdering are ignored.
		m_AutoSelectedDisplayModeDesc.RefreshRate.Denominator = 1;
		m_AutoSelectedDisplayModeDesc.RefreshRate.Numerator = 0;
	}


	return S_SUCCESS;
}

// -----------------------------------------------------------------------------------------------
SResult DX11Renderer::GetAutoSelectedDisplayModeDesc(SDisplayModeDescription* pDesc)
{
	if (pDesc)
	{
		if (m_InitParams.windowed)
		{
			pDesc->width = m_InitParams.resolution[0];
			pDesc->height = m_InitParams.resolution[1];
			pDesc->refreshRate = 0;
		}
		else
		{
			pDesc->width = m_AutoSelectedDisplayModeDesc.Width;
			pDesc->height = m_AutoSelectedDisplayModeDesc.Height;
			pDesc->refreshRate = m_AutoSelectedDisplayModeDesc.RefreshRate.Numerator
				/ m_AutoSelectedDisplayModeDesc.RefreshRate.Denominator;
		}

		return pDesc->IsValid() ? S_SUCCESS : S_ERROR;
	}

	return S_INVALIDPARAM;
}

// -----------------------------------------------------------------------------------------------
S_API SResult DX11Renderer::SetRenderStateDefaults(void)
{
	SP_ASSERTR(IsInited(), S_NOTINIT);

	// Depth state
	m_depthStencilDesc.DepthEnable = true; // we probably won't change this, so this if fixed
	m_depthStencilDesc.DepthFunc = D3D11_COMPARISON_LESS;
	m_depthStencilDesc.DepthWriteMask = D3D11_DEPTH_WRITE_MASK_ALL;

	m_depthStencilDesc.StencilEnable = true;
	m_depthStencilDesc.StencilReadMask = 0xff;
	m_depthStencilDesc.StencilWriteMask = 0xff;

	m_depthStencilDesc.FrontFace.StencilFailOp = D3D11_STENCIL_OP_KEEP;
	m_depthStencilDesc.FrontFace.StencilDepthFailOp = D3D11_STENCIL_OP_INCR;
	m_depthStencilDesc.FrontFace.StencilPassOp = D3D11_STENCIL_OP_KEEP;
	m_depthStencilDesc.FrontFace.StencilFunc = D3D11_COMPARISON_ALWAYS;

	m_depthStencilDesc.BackFace.StencilFailOp = D3D11_STENCIL_OP_KEEP;
	m_depthStencilDesc.BackFace.StencilDepthFailOp = D3D11_STENCIL_OP_DECR;
	m_depthStencilDesc.BackFace.StencilPassOp = D3D11_STENCIL_OP_KEEP;
	m_depthStencilDesc.BackFace.StencilFunc = D3D11_COMPARISON_ALWAYS;

	if (Failure(m_pD3DDevice->CreateDepthStencilState(&m_depthStencilDesc, &m_pDepthStencilState)))
		return CLog::Log(S_ERROR, "Failed create depth stencil state!");
#ifdef _DEBUG
	else
		CLog::Log(S_DEBUG, "Created depth stencil state.");
#endif

	m_pD3DDeviceContext->OMSetDepthStencilState(m_pDepthStencilState, 1);
#ifdef _DEBUG
	CLog::Log(S_DEBUG, "Set Depth Stencil state to default one.");
#endif


	// Create Terrain Depth Stencil State
	ZeroMemory(&m_terrainDepthDesc, sizeof(D3D11_DEPTH_STENCIL_DESC));
	m_terrainDepthDesc.DepthEnable = true;
	m_terrainDepthDesc.DepthFunc = D3D11_COMPARISON_LESS_EQUAL;
	m_terrainDepthDesc.DepthWriteMask = D3D11_DEPTH_WRITE_MASK_ALL;

	m_terrainDepthDesc.StencilEnable = false;

	if (Failure(m_pD3DDevice->CreateDepthStencilState(&m_terrainDepthDesc, &m_pTerrainDepthState)))
		CLog::Log(S_ERROR, "Failed create terrain depth stencil state!");


	// In DX11 we fist need a RSState interface
	memset((void*)&m_rsDesc, 0, sizeof(D3D11_RASTERIZER_DESC));
	m_rsDesc.AntialiasedLineEnable = false;	// ???
	m_rsDesc.CullMode = D3D11_CULL_BACK;
	m_rsDesc.FrontCounterClockwise = (m_InitParams.frontFace == eFF_CW);
	m_rsDesc.DepthBias = 0;
	m_rsDesc.DepthBiasClamp = 0;
	m_rsDesc.SlopeScaledDepthBias = 0;
	m_rsDesc.DepthClipEnable = true;
	m_rsDesc.FillMode = D3D11_FILL_SOLID;
	m_rsDesc.MultisampleEnable = m_InitParams.antiAliasingQuality != eAAQUALITY_LOW;
	m_rsDesc.ScissorEnable = FALSE; // maybe change this to true someday

	HRESULT hRes;
	if ((hRes = m_pD3DDevice->CreateRasterizerState(&m_rsDesc, &m_pRSState)))
	{
		return CLog::Log(S_ERROR, "Faield create Rasterizer State!");
	}

	m_pD3DDeviceContext->RSSetState(m_pRSState);


	// Setup bilinear Sampler State
	// NOTE: Currently using same default sampler state for Texturemap and Normalmap!
	D3D11_SAMPLER_DESC bilinearSamplerDesc;
	bilinearSamplerDesc.Filter = D3D11_FILTER_MIN_MAG_MIP_LINEAR;
	bilinearSamplerDesc.AddressU = D3D11_TEXTURE_ADDRESS_WRAP;
	bilinearSamplerDesc.AddressV = D3D11_TEXTURE_ADDRESS_WRAP;
	bilinearSamplerDesc.AddressW = D3D11_TEXTURE_ADDRESS_WRAP;
	bilinearSamplerDesc.MinLOD = -FLT_MAX;
	bilinearSamplerDesc.MaxLOD = FLT_MAX;
	bilinearSamplerDesc.MipLODBias = 0.0f;
	bilinearSamplerDesc.MaxAnisotropy = 1;	

	if (Failure(m_pD3DDevice->CreateSamplerState(&bilinearSamplerDesc, &m_pBilinearSamplerState)))
		return CLog::Log(S_ERROR, "Failed create bilinear sampler state!");

	// Nearest-Neighbor filtering Sampler state (used for skybox e.g.)
	D3D11_SAMPLER_DESC pointSamplerDesc;
	pointSamplerDesc.Filter = D3D11_FILTER_MIN_MAG_MIP_POINT;
	pointSamplerDesc.AddressU = D3D11_TEXTURE_ADDRESS_WRAP;
	pointSamplerDesc.AddressV = D3D11_TEXTURE_ADDRESS_WRAP;
	pointSamplerDesc.AddressW = D3D11_TEXTURE_ADDRESS_WRAP;
	pointSamplerDesc.MinLOD = -FLT_MAX;
	pointSamplerDesc.MaxLOD = FLT_MAX;
	pointSamplerDesc.MipLODBias = 0.0f;
	pointSamplerDesc.MaxAnisotropy = 1;

	if (Failure(m_pD3DDevice->CreateSamplerState(&pointSamplerDesc, &m_pPointSamplerState)))
		return CLog::Log(S_ERROR, "Failed create point sampler state!");

	// Shadowing sampler state
	D3D11_SAMPLER_DESC shadowSamplerDesc;
	shadowSamplerDesc.Filter = D3D11_FILTER_COMPARISON_MIN_MAG_LINEAR_MIP_POINT;
	shadowSamplerDesc.ComparisonFunc = D3D11_COMPARISON_LESS_EQUAL; // sample <= projCoord.z
	shadowSamplerDesc.AddressU = D3D11_TEXTURE_ADDRESS_BORDER;
	shadowSamplerDesc.AddressV = D3D11_TEXTURE_ADDRESS_BORDER;
	shadowSamplerDesc.AddressW = D3D11_TEXTURE_ADDRESS_BORDER;
	for (int i = 0; i < 4; ++i)
		shadowSamplerDesc.BorderColor[i] = 1.0f; // max depth
	shadowSamplerDesc.MinLOD = -FLT_MAX;
	shadowSamplerDesc.MaxLOD = FLT_MAX;
	shadowSamplerDesc.MipLODBias = 0.0f;
	shadowSamplerDesc.MaxAnisotropy = 1;

	if (Failure(m_pD3DDevice->CreateSamplerState(&shadowSamplerDesc, &m_pShadowSamplerState)))
		return CLog::Log(S_ERROR, "Failed create shadow sampler state!");


	// Set samplers
	unsigned int numSamplerStates = 3;
	ID3D11SamplerState* samplerStates[] = 
	{
		/*s0*/ m_pPointSamplerState,
		/*s1*/ m_pBilinearSamplerState,
		/*s2*/ m_pShadowSamplerState
	};	

	m_pD3DDeviceContext->PSSetSamplers(0, numSamplerStates, samplerStates);

	return S_SUCCESS;
}

// -----------------------------------------------------------------------------------------------
S_API void DX11Renderer::InitBlendStates()
{
	HRESULT hr;

	// Default Blend Desc and Blend State
	ZeroMemory(&m_DefBlendDesc, sizeof(m_DefBlendDesc));
	m_DefBlendDesc.AlphaToCoverageEnable = false;
	m_DefBlendDesc.IndependentBlendEnable = false;
	m_DefBlendDesc.RenderTarget[0].BlendEnable = false;
	m_DefBlendDesc.RenderTarget[0].BlendOp = D3D11_BLEND_OP_ADD;
	m_DefBlendDesc.RenderTarget[0].SrcBlend = D3D11_BLEND_ONE;
	m_DefBlendDesc.RenderTarget[0].DestBlend = D3D11_BLEND_ZERO;
	m_DefBlendDesc.RenderTarget[0].BlendOpAlpha = D3D11_BLEND_OP_ADD;
	m_DefBlendDesc.RenderTarget[0].SrcBlendAlpha = D3D11_BLEND_ONE;
	m_DefBlendDesc.RenderTarget[0].DestBlendAlpha = D3D11_BLEND_ZERO;
	m_DefBlendDesc.RenderTarget[0].RenderTargetWriteMask = D3D11_COLOR_WRITE_ENABLE_ALL;

	hr = m_pD3DDevice->CreateBlendState(&m_DefBlendDesc, &m_pDefBlendState);


	// Terrain Blend Desc and Blend State
	ZeroMemory(&m_TerrainBlendDesc, sizeof(m_TerrainBlendDesc));
	m_TerrainBlendDesc.AlphaToCoverageEnable = false;
	m_TerrainBlendDesc.IndependentBlendEnable = false;
	m_TerrainBlendDesc.RenderTarget[0].BlendEnable = true;
	m_TerrainBlendDesc.RenderTarget[0].BlendOp = D3D11_BLEND_OP_ADD;
	m_TerrainBlendDesc.RenderTarget[0].SrcBlend = D3D11_BLEND_SRC_ALPHA;
	m_TerrainBlendDesc.RenderTarget[0].DestBlend = D3D11_BLEND_INV_SRC_ALPHA;
	m_TerrainBlendDesc.RenderTarget[0].BlendOpAlpha = D3D11_BLEND_OP_ADD;
	m_TerrainBlendDesc.RenderTarget[0].SrcBlendAlpha = D3D11_BLEND_ZERO;
	m_TerrainBlendDesc.RenderTarget[0].DestBlendAlpha = D3D11_BLEND_ZERO;
	m_TerrainBlendDesc.RenderTarget[0].RenderTargetWriteMask = D3D11_COLOR_WRITE_ENABLE_ALL;

	hr = m_pD3DDevice->CreateBlendState(&m_TerrainBlendDesc, &m_pTerrainBlendState);


	// Alpha test Blend Desc and Blend State
	ZeroMemory(&m_AlphaTestBlendDesc, sizeof(m_AlphaTestBlendDesc));
	m_AlphaTestBlendDesc.AlphaToCoverageEnable = false;
	m_AlphaTestBlendDesc.IndependentBlendEnable = false;
	m_AlphaTestBlendDesc.RenderTarget[0].BlendEnable = true;
	m_AlphaTestBlendDesc.RenderTarget[0].BlendOp = D3D11_BLEND_OP_ADD;
	m_AlphaTestBlendDesc.RenderTarget[0].SrcBlend = D3D11_BLEND_SRC_ALPHA;
	m_AlphaTestBlendDesc.RenderTarget[0].DestBlend = D3D11_BLEND_INV_SRC_ALPHA;
	m_AlphaTestBlendDesc.RenderTarget[0].BlendOpAlpha = D3D11_BLEND_OP_ADD;
	m_AlphaTestBlendDesc.RenderTarget[0].SrcBlendAlpha = D3D11_BLEND_ONE;
	m_AlphaTestBlendDesc.RenderTarget[0].DestBlendAlpha = D3D11_BLEND_ZERO;
	m_AlphaTestBlendDesc.RenderTarget[0].RenderTargetWriteMask = D3D11_COLOR_WRITE_ENABLE_ALL;

	hr = m_pD3DDevice->CreateBlendState(&m_AlphaTestBlendDesc, &m_pAlphaTestBlendState);
}

// -----------------------------------------------------------------------------------------------
S_API SResult DX11Renderer::InitDefaultViewport(HWND hWnd, int nW, int nH)
{
	SP_ASSERTR(m_pD3DDevice && m_pD3DDeviceContext && m_pDXGIFactory, S_NOTINIT);
	SP_ASSERTR(hWnd, S_INVALIDPARAM);

	SViewportDescription vpDesc;

	vpDesc.projectionDesc.fov = 60;
	vpDesc.projectionDesc.projectionType = S_PROJECTION_PERSPECTIVE;
	vpDesc.projectionDesc.farZ = 100.0f;

	vpDesc.width = nW;
	vpDesc.height = nH;
	vpDesc.useDepthStencil = true;
	vpDesc.allowAsTexture = false;
	vpDesc.allowDepthAsTexture = true;
	vpDesc.hWnd = hWnd;
	vpDesc.windowed = m_InitParams.windowed;
	vpDesc.antiAliasingCount = m_InitParams.antiAliasingCount;
	vpDesc.antiAliasingQuality = m_InitParams.antiAliasingQuality;

	if (Failure(m_Viewport.Initialize(this, vpDesc)))
		return S_ERROR;

	// m_pD3DDeviceContext->RSSetViewports() is called in the SetTargetViewport after the call of this function.


	// okay. done
	return S_SUCCESS;
}

// -----------------------------------------------------------------------------------------------
S_API SResult DX11Renderer::CreateDX11Device()
{
	SP_ASSERTR(m_pAutoSelectedAdapter, S_NOTINIT);

	// Setup creation flags
	usint32 createFlags = 0;
	if (!m_InitParams.multithreaded)
		createFlags |= D3D11_CREATE_DEVICE_SINGLETHREADED;

#ifdef _DEBUG
	createFlags |= D3D11_CREATE_DEVICE_DEBUG;
#endif

	// To be sure we only use correct feature levels in our dx11 device, we have to explicitly set them
	// !!! Notice: To optionally support DX11.1, you have to add the missing D3D_FEATURE_LEVEL_11_1 !!!
	D3D_FEATURE_LEVEL pD3D11FeatureLevels[] =
	{
		D3D_FEATURE_LEVEL_11_0,
		D3D_FEATURE_LEVEL_10_1,
		D3D_FEATURE_LEVEL_10_0,

		// D3D9 features used to support old-hardware compatibility
		D3D_FEATURE_LEVEL_9_3,
		D3D_FEATURE_LEVEL_9_2,
		D3D_FEATURE_LEVEL_9_1
	};
	usint32 nFeatureLevels = 6;

	// Now create the device with the highest specs
	HRESULT hRes;
	hRes = D3D11CreateDevice(
		m_pAutoSelectedAdapter,
		D3D_DRIVER_TYPE_UNKNOWN,	// DX requires that, because we have a non-null existing adapter
		0,
		createFlags,
		pD3D11FeatureLevels, nFeatureLevels, D3D11_SDK_VERSION,
		&m_pD3DDevice, &m_D3DFeatureLevel, &m_pD3DDeviceContext);	// consider using deferred context for multithreading!

	assert(IS_VALID_PTR(m_pD3DDevice));

#ifdef _DEBUG
	const char nm[] = "SPD3D11Device";
	m_pD3DDevice->SetPrivateData(WKPDID_D3DDebugObjectName, sizeof(nm) - 1, nm);
#endif

	//delete[] pD3D11FeatureLevels;
	//SP_SAFE_DELETE_ARR(pD3D11FeatureLevels, nFeatureLevels);

	// If everything failed, then don't go further
	if (Failure(hRes) || !m_pD3DDevice || !m_pD3DDeviceContext)
	{
		return CLog::Log(S_ERROR, "Failed create D3D11 Device!");
	}





	return S_SUCCESS;
}

// -----------------------------------------------------------------------------------------------
S_API SResult DX11Renderer::D3D11_CreateSwapChain(DXGI_SWAP_CHAIN_DESC* pDesc, IDXGISwapChain** ppSwapChain)
{
	SP_ASSERTR(ppSwapChain && pDesc, S_INVALIDPARAM);
	SP_ASSERTR(m_pDXGIFactory, S_NOTINIT);

	m_pDXGIFactory->CreateSwapChain(m_pD3DDevice, pDesc, ppSwapChain);

	return S_SUCCESS;
}

// -----------------------------------------------------------------------------------------------
S_API SResult DX11Renderer::D3D11_CreateRTV(ID3D11Resource* pBBResource, D3D11_RENDER_TARGET_VIEW_DESC* pDesc, ID3D11RenderTargetView** ppRTV)
{
	SP_ASSERTR(pBBResource && ppRTV, S_INVALIDPARAM);
	SP_ASSERTR(m_pD3DDevice, S_NOTINIT);

	if (Failure(m_pD3DDevice->CreateRenderTargetView(pBBResource, pDesc, ppRTV)))
		return S_ERROR;
	else
		return S_SUCCESS;
}

// -----------------------------------------------------------------------------------------------
S_API S_RENDERER_TYPE DX11Renderer::GetType(void)
{
	return S_DIRECTX11;
}

// -----------------------------------------------------------------------------------------------
S_API SResult DX11Renderer::Initialize(const SRendererInitParams& params)
{
	if (IsInited())
		Shutdown();	// shutdown before initializing again

	m_InitParams = params;

	// Auto Select Adapter and display mode
	if (Failure(AutoSelectAdapter()))
	{
		CLog::Log(S_ERROR, "Failed automatic selection of Video adapter and display mode. Shut down.");
		Shutdown();
		return S_ERROR;
	}
	// Now start the actual Interface to the initialized DirectX Device.
	// Will fail if neither hardware nor mixed nor software processing is working.
	if (Failure(CreateDX11Device()))
	{
		Shutdown();
		return CLog::Log(S_ERROR, "Failed Create DX11 Device!");
	}
	// Set Default RenderStates and Texture Sampler States
	if (Failure(SetRenderStateDefaults()))
	{
		return CLog::Log(S_ERROR, "Failed set render state defaults!");
	}





	// Initialize the default Viewport
	// This viewport is forced to be generated. Client is only able to add more SwapChains
	// !! Do NOT put this call before CreateDX11Device(). InitDefaultViewport depends on an existing device in DX11 !!
	if (Failure(InitDefaultViewport(params.hWnd, params.resolution[0], params.resolution[1])))
	{
		Shutdown();
		return CLog::Log(S_ERROR, "Failed Init default viewport!");
	}


	// Initialize the matrix Constants buffer
	// Has to be done as immediate as possible, as other initialization methods might want to setup the constants.
	if (Failure(InitConstantBuffers()))
		return S_ERROR;


	// Set default viewport as target viewport
	if (Failure(SetTargetViewport((IViewport*)&m_Viewport)))
	{
		return CLog::Log(S_ERROR, "Failed set Target Viewport!");
	}


	// Set the base primitive topology
	m_pD3DDeviceContext->IASetPrimitiveTopology(D3D11_PRIMITIVE_TOPOLOGY_TRIANGLELIST);

	// Do not render terrain immediately
	m_TerrainRenderDesc.bRender = false;


	// Initialize Terrain Shader
	SShaderInfo terrainSI;
	terrainSI.filename = GetShaderPath(eSHADERFILE_TERRAIN);
	terrainSI.entry = "terrain";
	if (Failure(m_TerrainShader.Load(this, terrainSI)))
		return S_ERROR;

	// Initialize shader passes
	InitShaderPasses();
	m_CurrentPass = eSHADERPASS_NONE;

	// Initialize resource pool
	IResourcePool* pResources = GetResourcePool();

	// Create unset texture dummy
	m_pDummyTexture = pResources->GetTexture("no_texture");
	if (Failure(m_pDummyTexture->CreateEmpty(64, 64, 1, eTEXTURE_R8G8B8A8_UNORM, SColor(0.0f, 1.0f, 0.0f))))
	{
		CLog::Log(S_ERROR, "Could not create empty dummy texture (notexture)!");
	}


	// Create (128,128,0) replacement normal map
	m_pDummyNormalMap = pResources->GetTexture("no_normalmap");
	if (Failure(m_pDummyNormalMap->CreateEmpty(64, 64, 1, eTEXTURE_R8G8B8A8_UNORM, SColor(0.5f, 0.5f, 1.0f))))
	{
		CLog::Log(S_ERROR, "Could not create empty dummy normal map (nonormalmap)!");
	}


	InitBlendStates();

	// Reset bound-resources cache
	memset(m_BoundVSResources, 0, sizeof(ID3D11ShaderResourceView*) * 8);
	memset(m_BoundPSResources, 0, sizeof(ID3D11ShaderResourceView*) * 8);


	// Initialize font renderer
	m_pFontRenderer = new DX11FontRenderer();
	m_pFontRenderer->Init(this);

	return S_SUCCESS;
}

// -----------------------------------------------------------------------------------------------
S_API bool DX11Renderer::IsInited(void)
{
	return (m_pDXGIFactory && m_pD3DDevice && m_pD3DDeviceContext);
}

// -----------------------------------------------------------------------------------------------
S_API string DX11Renderer::GetShaderPath(EShaderFileType type) const
{
	const char* filename;
	switch (type)
	{
	case eSHADERFILE_SHADOW:
		filename = "shadow.fx";
		break;

		// FORWARD:
	case eSHADERFILE_SKYBOX:
		filename = "skybox.fx";
		break;
	case eSHADERFILE_FORWARD:
		filename = "illum.fx";	// PS_forward
		break;
	case eSHADERFILE_FORWARD_HELPER:
		filename = "helper.fx";
		break;
	case eSHADERFILE_FONT:
		filename = "font.fx";
		break;
	case eSHADERFILE_GUI:
		filename = "gui.fx";
		break;
	case eSHADERFILE_PARTICLES:
		filename = "particles.fx";
		break;

		// Deferred Shading:
	case eSHADERFILE_DEFERRED_ZPASS:
		filename = "zpass.fx";	// PS_zpass
		break;
	case eSHADERFILE_DEFERRED_SHADING:
		filename = "illum.fx";	// PS_illum
		break;
	case eSHADERFILE_TERRAIN:
		filename = "terrain.fx";
		break;

	default:
		return "???";
	}

	return m_InitParams.shaderResourceDirectory + "\\" + filename;
}

// -----------------------------------------------------------------------------------------------
S_API void DX11Renderer::InitShaderPasses()
{
	m_Passes[eSHADERPASS_FORWARD] = new ForwardShaderPass();
	m_Passes[eSHADERPASS_FORWARD]->Initialize(this);

	m_Passes[eSHADERPASS_GBUFFER] = new GBufferShaderPass();
	m_Passes[eSHADERPASS_GBUFFER]->Initialize(this);

	m_Passes[eSHADERPASS_SHADOWMAP] = new ShadowmapShaderPass();
	m_Passes[eSHADERPASS_SHADOWMAP]->Initialize(this);

	m_Passes[eSHADERPASS_SHADING] = new ShadingShaderPass(
		dynamic_cast<GBufferShaderPass*>(m_Passes[eSHADERPASS_GBUFFER]),
		dynamic_cast<ShadowmapShaderPass*>(m_Passes[eSHADERPASS_SHADOWMAP]));
	m_Passes[eSHADERPASS_SHADING]->Initialize(this);

	m_Passes[eSHADERPASS_GUI] = new GUIShaderPass();
	m_Passes[eSHADERPASS_GUI]->Initialize(this);

	m_Passes[eSHADERPASS_PARTICLES] = new ParticleShaderPass();
	m_Passes[eSHADERPASS_PARTICLES]->Initialize(this);

	m_Passes[eSHADERPASS_POSTEFFECT] = new PosteffectShaderPass();
	m_Passes[eSHADERPASS_POSTEFFECT]->Initialize(this);
}

// -----------------------------------------------------------------------------------------------
S_API IShaderPass* DX11Renderer::GetShaderPass(EShaderPassType type) const
{
	return m_Passes[type];
}

// -----------------------------------------------------------------------------------------------
S_API IFontRenderer* DX11Renderer::GetFontRenderer() const
{
	return m_pFontRenderer;
}

// -----------------------------------------------------------------------------------------------
S_API SResult DX11Renderer::Shutdown(void)
{
	delete m_pFontRenderer;
	m_pFontRenderer = 0;

	SP_SAFE_RELEASE(m_pDXGIFactory);
	SP_SAFE_RELEASE_CLEAR_VECTOR(m_vAdapters);

	m_TerrainShader.Clear();

	for (int i = 0; i < NUM_SHADERPASS_TYPES; ++i)
	{
		if (m_Passes[i])
		{
			if (m_Passes[i])
			{
				m_Passes[i]->Clear();
				delete m_Passes[i];
				m_Passes[i] = 0;
			}
		}
	}

	m_GBuffer1.Clear();
	m_GBuffer2.Clear();
	m_LightAccumulation.Clear();

	m_Viewport.Clear();

	SP_SAFE_RELEASE(m_pDummyTexture);
	SP_SAFE_RELEASE(m_pDummyNormalMap);

	if (IS_VALID_PTR(m_pResourcePool))
	{
		m_pResourcePool->ClearAll();
		delete m_pResourcePool;
	}
	m_pResourcePool = nullptr;

	m_pSetBlendState = 0;
	SP_SAFE_RELEASE(m_pDefBlendState);
	SP_SAFE_RELEASE(m_pAlphaTestBlendState);
	SP_SAFE_RELEASE(m_pTerrainBlendState);
	SP_SAFE_RELEASE(m_pBilinearSamplerState);
	SP_SAFE_RELEASE(m_pPointSamplerState);
	SP_SAFE_RELEASE(m_pShadowSamplerState);
	SP_SAFE_RELEASE(m_pDepthStencilState);
	SP_SAFE_RELEASE(m_pTerrainDepthState);
	SP_SAFE_RELEASE(m_pRSState);

	m_SceneConstants.Clear();
	m_TerrainConstants.Clear();

	// Reset bound-resources cache
	m_iMaxBoundPSResource = 0;
	m_iMaxBoundVSResource = 0;
	memset(m_BoundVSResources, 0, sizeof(ID3D11ShaderResourceView*) * 8);
	memset(m_BoundPSResources, 0, sizeof(ID3D11ShaderResourceView*) * 8);

	if (IS_VALID_PTR(m_pD3DDeviceContext))
	{
		m_pD3DDeviceContext->ClearState();
		m_pD3DDeviceContext->Flush();
	}


	SP_SAFE_RELEASE(m_pD3DDevice);
	SP_SAFE_RELEASE(m_pD3DDeviceContext);

#ifdef _DEBUG
	// Print live objects to detect memory leaks
	IDXGIDebug1* pDXGIDebug;
	DXGIGetDebugInterface1(0, __uuidof(IDXGIDebug1), reinterpret_cast<void**>(&pDXGIDebug));
	pDXGIDebug->ReportLiveObjects(DXGI_DEBUG_ALL, DXGI_DEBUG_RLO_DETAIL);
#endif

	m_FontRenderSchedule.Clear();

	return S_SUCCESS;
}



// -----------------------------------------------------------------------------------------------
S_API IShader* DX11Renderer::CreateShader() const
{
	return new DX11Shader();
}

// -----------------------------------------------------------------------------------------------
S_API void DX11Renderer::BindShaderPass(EShaderPassType type)
{
	if (IS_VALID_PTR(m_Passes[type]) && m_CurrentPass != type)
	{
		if (m_CurrentPass < NUM_SHADERPASS_TYPES)
			m_Passes[m_CurrentPass]->OnUnbind();

		m_Passes[type]->Bind();
		m_CurrentPass = type;
	}
}

// -----------------------------------------------------------------------------------------------
S_API IShaderPass* DX11Renderer::GetCurrentShaderPass() const
{
	return m_Passes[m_CurrentPass];
}





// -----------------------------------------------------------------------------------------------
S_API SResult DX11Renderer::BindRTCollection(const std::vector<IFBO*>& fboCollection, IFBO* depthFBO, bool depthReadonly /*= false*/, const char* dump_name /*= 0*/)
{
	int boundOnLvl;
	SP_ASSERTR(m_pD3DDevice, S_NOTINIT);

	if (fboCollection.empty())
		return CLog::Log(S_ERROR, "Failed BindRTCollection(): fboCollection is empty");

	// Collect DSV
	ID3D11DepthStencilView* pDSV = 0;
	DX11FBO* pDXDepthFBO = dynamic_cast<DX11FBO*>(depthFBO);
	if (IS_VALID_PTR(pDXDepthFBO))
	{
		if (depthReadonly)
			pDSV = pDXDepthFBO->GetReadonlyDSV();
		else
			pDSV = pDXDepthFBO->GetDSV();

		if (!depthReadonly && (boundOnLvl = IsBoundAsTexture(pDXDepthFBO->GetDepthSRV())) != -1)
			return CLog::Log(S_ERROR, "Failed BindRTCollection(): Depth buffer bound as texture already on level %d", boundOnLvl);

		if (!pDSV)
			return CLog::Log(S_ERROR, "Failed BindRTCollection(): DSV is not bindable as read-only");
	}

	m_bBoundDSVReadonly = depthReadonly;
	m_pBoundDSV = pDSV;

	// Collect RTVs
	m_BoundRenderTargets.clear();
	ID3D11RenderTargetView** pRTVs = new ID3D11RenderTargetView*[fboCollection.size()];
	int i = 0;
	for (auto itFBO = fboCollection.begin(); itFBO != fboCollection.end(); ++itFBO, ++i)
	{
		DX11FBO* pDXFBO = dynamic_cast<DX11FBO*>(*itFBO);
		if (!IS_VALID_PTR(pDXFBO))
			continue;

		if ((boundOnLvl = IsBoundAsTexture(pDXFBO->GetSRV())) != -1)
		{
			CLog::Log(S_ERROR, "Failed BindRTCollection(): Frame buffer #%d bound as texture already on level %d", i, boundOnLvl);
			continue;
		}

		m_BoundRenderTargets.push_back(pDXFBO);
		pRTVs[m_BoundRenderTargets.size()] = pDXFBO->GetRTV();
	}

	// Bind
	m_pD3DDeviceContext->OMSetRenderTargets(m_BoundRenderTargets.size(), pRTVs, pDSV);

	delete[] pRTVs;
	pRTVs = 0;

	FrameDump(string("Bound RT Collection ") + (dump_name ? dump_name : "") + " (" + std::to_string(m_BoundRenderTargets.size()) + " RTs)!");

	return S_SUCCESS;
}

// -----------------------------------------------------------------------------------------------
S_API SResult DX11Renderer::BindSingleRT(IFBO* pFBO, bool depthReadonly /*= 0*/)
{
	SP_ASSERTR(IsInited(), S_NOTINIT);
	SP_ASSERTR(IS_VALID_PTR(pFBO), S_INVALIDPARAM);

	// Collect RTV and DSV
	DX11FBO* pDXFBO = dynamic_cast<DX11FBO*>(pFBO);
	if (!IS_VALID_PTR(pDXFBO))
		return S_INVALIDPARAM;

	ID3D11RenderTargetView* pRTV = pDXFBO->GetRTV();
	ID3D11DepthStencilView* pDSV = (depthReadonly ? pDXFBO->GetReadonlyDSV() : pDXFBO->GetDSV());
	if (!pDSV)
		return CLog::Log(S_ERROR, "Failed BindSingleRT(): DSV is not bindable as read-only");

	// Check if already bound
	if (!m_BoundRenderTargets.empty() && !BoundMultipleRTs() && m_BoundRenderTargets[0] == pFBO && m_pBoundDSV == pDSV)
		return S_SUCCESS;

	// Check if already bound as texture
	int boundOnLvl;
	if ((boundOnLvl = IsBoundAsTexture(pDXFBO->GetSRV())) != -1)
		return CLog::Log(S_ERROR, "Failed BindSingleRT(): Frame buffer bound as texture already on level %d", boundOnLvl);

	if (!depthReadonly && (boundOnLvl = IsBoundAsTexture(pDXFBO->GetDepthSRV())) != -1)
		return CLog::Log(S_ERROR, "Failed BindSingleRT(): Depth buffer bound as texture already on level %d", boundOnLvl);

	// Bind
	m_pD3DDeviceContext->OMSetRenderTargets(1, &pRTV, pDSV);

	m_BoundRenderTargets.clear();
	m_BoundRenderTargets.push_back(pDXFBO);

	m_pBoundDSV = pDSV;
	m_bBoundDSVReadonly = depthReadonly;

	return S_SUCCESS;
}

// -----------------------------------------------------------------------------------------------
S_API SResult DX11Renderer::BindSingleRT(IViewport* pViewport)
{
	IFBO* pBackBuffer = 0;
	SP_ASSERTR(pViewport && (pBackBuffer = pViewport->GetBackBuffer()), S_INVALIDPARAM);
	SP_ASSERTR(m_pD3DDevice && m_pD3DDeviceContext, S_NOTINIT);

	if (!pBackBuffer->IsInitialized())
		return S_INVALIDPARAM;

	if (Failure(BindSingleRT(pBackBuffer)))
		return S_ERROR;

	return S_SUCCESS;
}

// -----------------------------------------------------------------------------------------------
S_API vector<IFBO*> DX11Renderer::GetBoundRTs() const
{
	vector<IFBO*> boundRTs;
	for (auto itDXFBO = m_BoundRenderTargets.begin(); itDXFBO != m_BoundRenderTargets.end(); ++itDXFBO)
		boundRTs.push_back(*itDXFBO);

	return boundRTs;
}

// -----------------------------------------------------------------------------------------------
S_API SResult DX11Renderer::SetVBStream(IVertexBuffer* pVB, unsigned int index)
{
	if (!IsInited()) return S_NOTINIT;
	if (!IS_VALID_PTR(pVB)) return S_INVALIDPARAM;

	DX11VertexBuffer* pDXVB = dynamic_cast<DX11VertexBuffer*>(pVB);
	if (!pDXVB)
		return S_INVALIDPARAM;

	ID3D11Buffer* pD3DVB = pDXVB->D3D11_GetBuffer();
	usint32 stride = sizeof(SVertex);
	usint32 offset = 0;

	m_pD3DDeviceContext->IASetVertexBuffers(index, 1, &pD3DVB, &stride, &offset);

	return S_SUCCESS;
}

// -----------------------------------------------------------------------------------------------
S_API SResult DX11Renderer::SetIBStream(IIndexBuffer* pIB)
{
	if (!IsInited())
		return S_NOTINIT;

	if (IS_VALID_PTR(pIB))
	{
		S_INDEXBUFFER_FORMAT ibFmt = pIB->GetFormat();
		DXGI_FORMAT ibFmtDX = DXGI_FORMAT_R16_UINT;
		switch (ibFmt)
		{
		case S_INDEXBUFFER_16: ibFmtDX = DXGI_FORMAT_R16_UINT; break;
		case S_INDEXBUFFER_32: ibFmtDX = DXGI_FORMAT_R32_UINT; break;
		default:
		return S_ERROR;
		}

		DX11IndexBuffer* pDXIB = dynamic_cast<DX11IndexBuffer*>(pIB);
		if (!pDXIB)
			return S_INVALIDPARAM;

		m_pD3DDeviceContext->IASetIndexBuffer(pDXIB->D3D11_GetBuffer(), ibFmtDX, 0);
	}
	else
	{
		m_pD3DDeviceContext->IASetIndexBuffer(0, DXGI_FORMAT_UNKNOWN, 0);
	}

	return S_SUCCESS;
}

// -----------------------------------------------------------------------------------------------
S_API SResult DX11Renderer::SetInstanceStream(ITypelessInstanceBuffer* pInstanceBuffer, unsigned int index /*= 1*/)
{
	if (!IsInited())
		return S_NOTINIT;

	if (!IS_VALID_PTR(pInstanceBuffer))
		return S_INVALIDPARAM;

	DX11InstanceBuffer* pDXInstanceBuffer = dynamic_cast<DX11InstanceBuffer*>(pInstanceBuffer);
	if (!pDXInstanceBuffer)
		return S_INVALIDPARAM;

	ID3D11Buffer* pD3DBuffer = pDXInstanceBuffer->D3D11_GetHWBuffer();
	usint32 stride = pDXInstanceBuffer->GetStrideSize();
	usint32 offset = 0;

	m_pD3DDeviceContext->IASetVertexBuffers(index, 1, &pD3DBuffer, &stride, &offset);

	return S_SUCCESS;
}


// -----------------------------------------------------------------------------------------------
S_API SResult DX11Renderer::BindVertexShaderTexture(ITexture* pTex, usint32 lvl /*= 0*/)
{
	DX11Texture* pDXTex = dynamic_cast<DX11Texture*>(pTex);
	if (!pDXTex)
		return S_INVALIDPARAM;

	ID3D11ShaderResourceView* pSRV = (pDXTex ? pDXTex->D3D11_GetSRV() : 0);

	if (pSRV != m_BoundVSResources[lvl])
	{
		m_pD3DDeviceContext->VSSetShaderResources(lvl, 1, &pSRV);
		m_BoundVSResources[lvl] = pSRV;

		if (lvl > m_iMaxBoundVSResource)
			m_iMaxBoundVSResource = lvl;
		else
			for (; m_iMaxBoundVSResource > 0 && !m_BoundVSResources[m_iMaxBoundVSResource]; --m_iMaxBoundVSResource) {}
	}

	return S_SUCCESS;
}

// -----------------------------------------------------------------------------------------------
S_API SResult DX11Renderer::BindTexture(ITexture* pTex, usint32 lvl /*=0*/)
{
	SP_ASSERTR(IsInited(), S_NOTINIT);

	if (lvl > MAX_BOUND_SHADER_RESOURCES)
		return S_INVALIDPARAM;

	if (!pTex)
		pTex = GetDummyTexture();

	ID3D11ShaderResourceView* pSRV;
	DX11Texture* pDXTexture = dynamic_cast<DX11Texture*>(pTex);
	if (!pDXTexture)
		return S_ERROR;

	pSRV = pDXTexture->D3D11_GetSRV();

	if (pSRV != m_BoundPSResources[lvl])
	{
		m_pD3DDeviceContext->PSSetShaderResources(lvl, 1, &pSRV);
		m_BoundPSResources[lvl] = pSRV;

		if (lvl > m_iMaxBoundPSResource)
			m_iMaxBoundPSResource = lvl;
		else
			for (; m_iMaxBoundPSResource > 0 && !m_BoundPSResources[m_iMaxBoundPSResource]; --m_iMaxBoundPSResource) {}
	}

	return S_SUCCESS;
}

// -----------------------------------------------------------------------------------------------
S_API SResult DX11Renderer::BindTexture(IFBO* pFBO, usint32 lvl)
{
	SP_ASSERTR(pFBO, S_INVALIDPARAM);

	ITexture* pFrameBufferTexture = pFBO->GetTexture();
	if (!pFrameBufferTexture)
		return CLog::Log(S_ERROR, "Cannot BindTexture() from FBO on level %d: Not initialized as texture", lvl);

	return BindTexture(pFrameBufferTexture, lvl);
}

// -----------------------------------------------------------------------------------------------
S_API SResult DX11Renderer::BindDepthBufferAsTexture(IFBO* pFBO, usint32 lvl /*= 0*/)
{
	SP_ASSERTR(pFBO, S_INVALIDPARAM);

	DX11FBO* pDXFBO = dynamic_cast<DX11FBO*>(pFBO);
	if (!IS_VALID_PTR(pDXFBO))
		return S_INVALIDPARAM;

	ID3D11DepthStencilView* pWriteableDSV = pDXFBO->GetDSV();
	if (m_pBoundDSV == pWriteableDSV)
		return CLog::Log(S_ERROR, "Failed BindDepthBufferAsTexture(): Is bound as writeable depth buffer");

	ITexture* pDepthBufferTexture = pDXFBO->GetDepthBufferTexture();
	if (!pDepthBufferTexture)
		return CLog::Log(S_ERROR, "Cannot BindDepthBufferAsTexture() from FBO on level %d: Not initialized as texture", lvl);

	return BindTexture(pDepthBufferTexture, lvl);
}

// -----------------------------------------------------------------------------------------------
S_API void DX11Renderer::UnbindTexture(usint32 lvl)
{
	ID3D11ShaderResourceView* srv[] = { 0 };
	m_pD3DDeviceContext->PSSetShaderResources(lvl, 1, srv);
	m_BoundPSResources[lvl] = 0;

	for (; m_iMaxBoundPSResource > 0 && !m_BoundPSResources[m_iMaxBoundPSResource]; --m_iMaxBoundPSResource) {}
}

S_API void DX11Renderer::UnbindTexture(ITexture* pTexture)
{
	DX11Texture* pDXTexture = dynamic_cast<DX11Texture*>(pTexture);
	if (!pTexture)
		return;

	const ID3D11ShaderResourceView* pSRV = pDXTexture->D3D11_GetSRV();

	for (unsigned int i = 0; i <= m_iMaxBoundPSResource; ++i)
	{
		if (m_BoundPSResources[i] == pSRV)
			UnbindTexture(i);
	}
}

// -----------------------------------------------------------------------------------------------
S_API int DX11Renderer::IsBoundAsTexture(ID3D11ShaderResourceView* srv)
{
	if (srv)
	{
		unsigned int i;
		for (i = 0; i <= m_iMaxBoundPSResource; ++i)
		{
			if (m_BoundPSResources[i] == srv)
				return (int)i;
		}

		for (i = 0; i <= m_iMaxBoundVSResource; ++i)
		{
			if (m_BoundVSResources[i] == srv)
				return (int)i;
		}
	}

	return -1;
}

// -----------------------------------------------------------------------------------------------
S_API ITexture* DX11Renderer::GetDummyTexture() const
{
	return m_pDummyTexture;
}

// -----------------------------------------------------------------------------------------------
S_API ITexture* DX11Renderer::GetDummyNormalmap() const
{
	return m_pDummyNormalMap;
}


// -----------------------------------------------------------------------------------------------
S_API SResult DX11Renderer::CreateAdditionalViewport(IViewport** pViewport, const SViewportDescription& desc)
{
	SP_ASSERTR(m_pD3DDevice && m_pD3DDeviceContext && m_pDXGIFactory, S_NOTINIT);
	SP_ASSERTR(pViewport, S_INVALIDPARAM);

	// Instanciate the viewport first and initialize it
	DX11Viewport* pDXViewport = new DX11Viewport();
	if (Failure(pDXViewport->Initialize(this, desc)))
		return CLog::Log(S_ERROR, "Failed initialize additional viewport");

	*pViewport = pDXViewport;
	return S_SUCCESS;
}

















// -----------------------------------------------------------------------------------------------
S_API SResult DX11Renderer::BeginScene(void)
{
	if (m_bInScene)
		return S_ERROR;

	ResetBudgetTimerStats();

	if (m_bDumpFrame)
		CLog::Log(S_DEBUG, "Beginning rendering of scene... (Begin Frame)");

	m_bInScene = true;

	// Reset to target viewport camera
	SetViewProjMatrix(m_pTargetViewport);
	SetEyePosition(m_pTargetViewport->GetCamera()->position);

	return S_SUCCESS;
}

// -----------------------------------------------------------------------------------------------
S_API STerrainRenderDesc* DX11Renderer::GetTerrainRenderDesc()
{
	return &m_TerrainRenderDesc;
}



// -----------------------------------------------------------------------------------------------
S_API SFontRenderSlot* DX11Renderer::GetFontRenderSlot()
{
	return m_FontRenderSchedule.Get();
}

// -----------------------------------------------------------------------------------------------
S_API void DX11Renderer::ReleaseFontRenderSlot(SFontRenderSlot** pFRS)
{
	m_FontRenderSchedule.Release(pFRS);
}



// -----------------------------------------------------------------------------------------------
S_API SResult DX11Renderer::EndScene(void)
{
	if (!m_bInScene)
		return S_ERROR;

	m_bInScene = false;

	static SResult sr;

	sr = UnleashFontRenderSchedule();
	
	sr = PresentTargetViewport();

	for (int iShaderPass = 0; iShaderPass < NUM_SHADERPASS_TYPES; ++iShaderPass)
	{
		if (m_Passes[iShaderPass])
			m_Passes[iShaderPass]->OnEndFrame();
	}

	// Clear target viewport
	sr = BindSingleRT(m_pTargetViewport);
	sr = ClearBoundRTs();

	m_bDumpFrame = false;
	return sr;
}













// -----------------------------------------------------------------------------------------------
S_API SResult DX11Renderer::Render(const SRenderDesc& renderDesc)
{
	if (!m_bInScene)
	{
		FrameDump("Cannot Render Object: Not in scene!");
		return S_INVALIDSTAGE;
	}

	// Skip render desc without subsets
	if (renderDesc.nSubsets == 0 || !IS_VALID_PTR(renderDesc.pSubsets))
	{
		FrameDump("Skipping Render: No subsets");
		return S_SUCCESS;
	}

	// Set correct depth stencil state
	EnableDepthTest(renderDesc.bDepthStencilEnable);

	EDepthTestFunction depthTestFunc = (renderDesc.bInverseDepthTest ? eDEPTH_TEST_GREATER : eDEPTH_TEST_LESS);
	SetDepthTestFunction(depthTestFunc);

	// Set rasterizer state
	EnableBackfaceCulling(false);

	// Set blend state
	D3D11_SetBlendState(m_pDefBlendState);


	// Set the viewport matrices
	SSceneConstants origSceneConstants;
	if (renderDesc.bCustomViewProjMtx)
	{
		FrameDump("Setting custom viewproj mtx");

		// Backup current scene constants, so we can restore them later
		SSceneConstants* pSceneConstants = m_SceneConstants.GetConstants();
		memcpy(&origSceneConstants, pSceneConstants, sizeof(SSceneConstants));

		SetViewProjMatrix(renderDesc.viewMtx, renderDesc.projMtx);
	}


	// In case something else was bound to the slot before...
	BindSceneCB(m_SceneConstants.GetCB());


	DrawSubsets(renderDesc);


	// Restore scene constants if necessary
	if (renderDesc.bCustomViewProjMtx)
	{
		SSceneConstants* pSceneConstants = m_SceneConstants.GetConstants();
		memcpy(pSceneConstants, &origSceneConstants, sizeof(SSceneConstants));
		m_SceneConstants.Update();
	}

	return S_SUCCESS;
}






// -----------------------------------------------------------------------------------------------
S_API SResult DX11Renderer::RenderTerrain(const STerrainRenderDesc& terrainRenderDesc)
{
	if (!m_bInScene)
	{
		FrameDump("Cannot Render Terrain: Not in scene!");
		return S_INVALIDSTAGE;
	}


	// Update Per-Scene Constants Buffer
	SetViewProjMatrix(m_pTargetViewport);

	// In case something else was bound to the slot before...
	BindSceneCB(m_SceneConstants.GetCB());


	// Render Terrain
	if (terrainRenderDesc.bRender)
	{
		bool bTerrainRenderState = true;	// true = success
		FrameDump("Rendering Terrain...");

		m_CurrentPass = eSHADERPASS_NONE;

		// Render Terrain directly to the backbuffer
		BindSingleRT(m_pTargetViewport);

		if (!(bTerrainRenderState = IS_VALID_PTR(terrainRenderDesc.pVtxHeightMap))) CLog::Log(S_ERROR, "Invalid terrain vtx heightmap in render desc!");
		if (!(bTerrainRenderState = IS_VALID_PTR(terrainRenderDesc.pColorMap))) CLog::Log(S_ERROR, "Invalid color map in Terrin render Desc!");
		if (!(bTerrainRenderState = IS_VALID_PTR(terrainRenderDesc.pLayerMasks))) CLog::Log(S_ERROR, "Invalid layer masks array in Terarin Render Desc!");
		if (!(bTerrainRenderState = IS_VALID_PTR(terrainRenderDesc.pDetailMaps))) CLog::Log(S_ERROR, "Invalid detail maps array in Terarin Render Desc!");
		if (!(bTerrainRenderState = (terrainRenderDesc.nLayers > 0))) CLog::Log(S_ERROR, "Invalid layer count in Terrain Render Desc!");

		BindTexture(terrainRenderDesc.pVtxHeightMap, 0);
		BindVertexShaderTexture(terrainRenderDesc.pVtxHeightMap, 0);
		BindTexture(terrainRenderDesc.pColorMap, 1);

		// ~~
		// TODO: Avoid referencing a shader pass here
		//			- probably fixed with deferred rendering, because the shadowmap is then only bound in shading pass.
		ShadowmapShaderPass* pShadowmapPass = dynamic_cast<ShadowmapShaderPass*>(GetShaderPass(eSHADERPASS_SHADOWMAP));
		if (pShadowmapPass)
			BindDepthBufferAsTexture(pShadowmapPass->GetShadowmap(), 4);

		if (terrainRenderDesc.bUpdateCB)
		{
			memcpy(m_TerrainConstants.GetConstants(), &terrainRenderDesc.constants, sizeof(terrainRenderDesc.constants));
			m_TerrainConstants.Update();
		}

		EnableDepthTest(true);
		SetDepthTestFunction(eDEPTH_TEST_LESS);

		EnableBackfaceCulling(true);

		// bind terrain cb
		BindConstantsBuffer(m_TerrainConstants.GetCB());

		// render all chunks
		if (IS_VALID_PTR(terrainRenderDesc.pDrawCallDescs) && terrainRenderDesc.nDrawCallDescs > 0)
		{
			for (unsigned int c = 0; c < terrainRenderDesc.nDrawCallDescs; ++c)
			{
				// Draw first layer without alpha blend and default depth state
				D3D11_SetBlendState(m_pDefBlendState);
				m_pD3DDeviceContext->OMSetDepthStencilState(m_pDepthStencilState, 1);

				// For each layer
				for (unsigned int iLayer = 0; iLayer < terrainRenderDesc.nLayers; ++iLayer)
				{
					if (iLayer == 1)
					{
						// Draw each further layer with alpha blending and terrain depth stencil state
						D3D11_SetBlendState(m_pTerrainBlendState);
						m_pD3DDeviceContext->OMSetDepthStencilState(m_pTerrainDepthState, 1);
					}

					// Bind textures
					BindTexture(terrainRenderDesc.pDetailMaps[iLayer], 2);
					BindTexture(terrainRenderDesc.pLayerMasks[iLayer], 3);

					// Draw the subset
					DrawTerrainSubset(terrainRenderDesc.pDrawCallDescs[c]);
				}
			}

			// Unbind terrain layer textures
			UnbindTexture(2);
			UnbindTexture(3);

			// Unbind Vertex Shader texture
			BindVertexShaderTexture((ITexture*)0, 0);
		}

		UnbindTexture(4);

		D3D11_SetBlendState(m_pDefBlendState);
	}
	else
	{
		FrameDump("Terrain not scheduled to be rendered this frame.");
	}

	return S_SUCCESS;
}





/////////////////////////////////////////////////////////////////////////////////////////////////////////////
/////////////////////////////////////////////////////////////////////////////////////////////////////////////

//		DRAW

/////////////////////////////////////////////////////////////////////////////////////////////////////////////
/////////////////////////////////////////////////////////////////////////////////////////////////////////////



// -----------------------------------------------------------------------------------------------
S_API SResult DX11Renderer::DrawSubsets(const SRenderDesc& renderDesc)
{
	for (unsigned int iSubset = 0; iSubset < renderDesc.nSubsets; ++iSubset)
	{
		SRenderSubset& subset = renderDesc.pSubsets[iSubset];

		if (!subset.render)
		{
			FrameDump("[DX11Renderer] subset.render=false in DrawForwardSubsets()");
			continue;
		}

		if (m_CurrentPass != eSHADERPASS_FORWARD && subset.shaderResources.illumModel == eILLUM_HELPER)
		{
			FrameDump("[DX11Renderer] Cannot render helper with deferred pipeline!");
			continue;
		}

		D3D11_SetBlendState(subset.enableAlphaTest ? m_pAlphaTestBlendState : m_pDefBlendState);

		GetCurrentShaderPass()->SetShaderResources(subset.shaderResources, SMatrixTranspose(renderDesc.transform));

		Draw(subset.drawCallDesc);


		if (subset.bOnce)
		{
			subset.render = false;
			FrameDump("[DX11Renderer] subset.bOnce=true");
		}
	}

	FrameDump(renderDesc.nSubsets, "renderDesc.nSubsets");
	return S_SUCCESS;
}

// -----------------------------------------------------------------------------------------------
S_API SResult DX11Renderer::Draw(const SDrawCallDesc& desc)
{
	FrameDump("DX11Renderer::Draw()");

	// bind vertex data stream
	if (Failure(SetVBStream(desc.pVertexBuffer)))
	{
		StopBudgetTimer(eDX11_BUDGET_DRAW_FORWARD);
		return S_ERROR;
	}

	bool bLines = (desc.primitiveType == PRIMITIVE_TYPE_LINES || desc.primitiveType == PRIMITIVE_TYPE_LINESTRIP);

	// Make sure the correct primitive type is set, but don't change too often
	if (bLines)
	{
		if (Failure(SetIBStream(0)))
			return S_ERROR;

		if (m_SetPrimitiveType != desc.primitiveType)
		{
			D3D11_PRIMITIVE_TOPOLOGY d3dTopology;
			if (desc.primitiveType == PRIMITIVE_TYPE_LINES)
				d3dTopology = D3D11_PRIMITIVE_TOPOLOGY_LINELIST;
			else
				d3dTopology = D3D11_PRIMITIVE_TOPOLOGY_LINESTRIP;

			m_pD3DDeviceContext->IASetPrimitiveTopology(d3dTopology);
			m_SetPrimitiveType = desc.primitiveType;
		}

		m_pD3DDeviceContext->Draw(desc.iEndVBIndex - desc.iStartVBIndex + 1, desc.iStartVBIndex);
	}
	else
	{
		if (Failure(SetIBStream(desc.pIndexBuffer)))
			return S_ERROR;

		if (m_SetPrimitiveType != PRIMITIVE_TYPE_TRIANGLELIST)
		{
			m_pD3DDeviceContext->IASetPrimitiveTopology(D3D11_PRIMITIVE_TOPOLOGY_TRIANGLELIST);
			m_SetPrimitiveType = PRIMITIVE_TYPE_TRIANGLELIST;
		}

		m_pD3DDeviceContext->DrawIndexed(desc.iEndIBIndex - desc.iStartIBIndex + 1, desc.iStartIBIndex, desc.iStartVBIndex);
	}

	return S_SUCCESS;
}

// -----------------------------------------------------------------------------------------------
S_API SResult DX11Renderer::DrawTerrainSubset(const STerrainDrawCallDesc& dcd)
{
	if (!dcd.bRender)
		return S_SUCCESS;

	if (Failure(SetVBStream(dcd.pVertexBuffer))) return S_ERROR;
	if (Failure(SetIBStream(dcd.pIndexBuffer))) return S_ERROR;

	if (Failure(m_TerrainShader.Bind()))
		return CLog::Log(S_ERROR, "Enabling terrain effect failed");

	if (m_SetPrimitiveType != PRIMITIVE_TYPE_TRIANGLELIST)
	{
		m_pD3DDeviceContext->IASetPrimitiveTopology(D3D11_PRIMITIVE_TOPOLOGY_TRIANGLELIST);
		m_SetPrimitiveType = PRIMITIVE_TYPE_TRIANGLELIST;
	}

	m_pD3DDeviceContext->DrawIndexed(dcd.iEndIBIndex - dcd.iStartIBIndex + 1, dcd.iStartIBIndex, dcd.iStartVBIndex);

	return S_SUCCESS;
}

// -----------------------------------------------------------------------------------------------
S_API SResult DX11Renderer::UnleashFontRenderSchedule()
{
	if (m_FontRenderSchedule.GetUsedObjectCount() == 0)
		return S_SUCCESS;

	if (!IS_VALID_PTR(m_pFontRenderer))
		return S_NOTINIT;

	StartOrResumeBudgetTimer(eDX11_BUDGET_UNLEASH_FONT_SCHEDULE, "DX11Renderer::UnleashFontRenderSchedule()");

	BindSingleRT(m_pTargetViewport);

	m_pFontRenderer->BeginRender();

	unsigned int iFRSIterator;
	SFontRenderSlot* pFRS = m_FontRenderSchedule.GetFirstUsedObject(iFRSIterator);
	while (pFRS)
	{
		if (pFRS->render)
		{
			SPixelPosition pp;
			pp.x = pFRS->screenPos[0];
			pp.y = pFRS->screenPos[1];
			m_pFontRenderer->RenderText(pFRS->text, pFRS->color, pp, pFRS->fontSize, pFRS->alignRight);
		}

		if (!pFRS->keep)
		{
			m_FontRenderSchedule.Release(&pFRS);
		}

		pFRS = m_FontRenderSchedule.GetNextUsedObject(iFRSIterator);
	}

	m_pFontRenderer->EndRender();

	StopBudgetTimer(eDX11_BUDGET_UNLEASH_FONT_SCHEDULE);

	return S_SUCCESS;
}



















// -----------------------------------------------------------------------------------------------
S_API SResult DX11Renderer::PresentTargetViewport(void)
{
	SP_ASSERTR(IsInited(), S_NOTINIT);

	DX11Viewport* pDXTargetViewport = dynamic_cast<DX11Viewport*>(m_pTargetViewport);
	SP_ASSERTR(pDXTargetViewport, S_ERROR);

	IDXGISwapChain* pSwapChain = *pDXTargetViewport->D3D11_GetSwapChainPtr();

	unsigned int syncInterval = 0;
	if (m_InitParams.enableVSync)
		syncInterval = m_InitParams.vsyncInterval;

	if (Failure(pSwapChain->Present(syncInterval, 0)))
		return S_ERROR;

	return S_SUCCESS;
}

// -----------------------------------------------------------------------------------------------
S_API SResult DX11Renderer::ClearBoundRTs(bool color /*= true*/, bool depth /*= true*/)
{
	SP_ASSERTR(IsInited(), S_NOTINIT);

	if (color)
	{
		float clearColor[4];
		SPGetColorFloatArray(clearColor, m_InitParams.clearColor);

		for (auto itBoundRT = m_BoundRenderTargets.begin(); itBoundRT != m_BoundRenderTargets.end(); ++itBoundRT)
		{
			ID3D11RenderTargetView* pRTV = (*itBoundRT)->GetRTV();
			if (!IS_VALID_PTR(pRTV))
				continue;

			m_pD3DDeviceContext->ClearRenderTargetView(pRTV, clearColor);
		}
	}

	if (depth)
	{
		if (IS_VALID_PTR(m_pBoundDSV) && !m_bBoundDSVReadonly)
			m_pD3DDeviceContext->ClearDepthStencilView(m_pBoundDSV, D3D11_CLEAR_DEPTH, 1.0f, 0);
	}

	return S_SUCCESS;
}

// -----------------------------------------------------------------------------------------------
S_API SResult DX11Renderer::SetTargetViewport(IViewport* pViewport)
{
	SP_ASSERTR(pViewport, S_INVALIDPARAM);

	m_pTargetViewport = pViewport;
	SetViewProjMatrix(pViewport);
	if (m_pD3DDeviceContext)
	{
		DX11Viewport* pDXViewport = dynamic_cast<DX11Viewport*>(pViewport);
		if (!IS_VALID_PTR(pDXViewport))
			return S_INVALIDPARAM;

		m_pD3DDeviceContext->RSSetViewports(1, pDXViewport->D3D11_GetViewportDescPtr());

		SIZE vpSz = pViewport->GetSize();

		SSceneConstants* sceneConstants = m_SceneConstants.GetConstants();
		sceneConstants->screenRes[0] = vpSz.cx;
		sceneConstants->screenRes[1] = vpSz.cy;
	}

	return S_SUCCESS;
}

// -----------------------------------------------------------------------------------------------
S_API IViewport* DX11Renderer::GetTargetViewport(void)
{
	return m_pTargetViewport;
}

// -----------------------------------------------------------------------------------------------
S_API IViewport* DX11Renderer::GetDefaultViewport(void)
{
	return (IViewport*)&m_Viewport;
}

// -----------------------------------------------------------------------------------------------
S_API SResult DX11Renderer::InitConstantBuffers()
{
	// Per-Object constants
	if (Failure(m_SceneConstants.Initialize(this)))
		return CLog::Log(S_ERROR, "Failed initialize scene constants buffer!");

	BindSceneCB(m_SceneConstants.GetCB());


	// Terrain constants
	if (Failure(m_TerrainConstants.Initialize(this)))
		return CLog::Log(S_ERROR, "Failed initialize terrain constants buffer!");


	return S_SUCCESS;
}

// -----------------------------------------------------------------------------------------------
S_API IConstantsBuffer* DX11Renderer::CreateConstantsBuffer() const
{
	return new DX11ConstantsBuffer();
}

// -----------------------------------------------------------------------------------------------
S_API void DX11Renderer::BindConstantsBuffer(const IConstantsBuffer* cb, bool vs /*=false*/)
{
	const DX11ConstantsBuffer* dxcb = dynamic_cast<const DX11ConstantsBuffer*>(cb);
	ID3D11Buffer* pBuffer = dxcb->GetBuffer();

	if (m_pBoundCB != pBuffer)
	{
		m_pD3DDeviceContext->PSSetConstantBuffers(1, 1, &pBuffer);
		m_pD3DDeviceContext->VSSetConstantBuffers(1, 1, &pBuffer);

		m_pBoundCB = pBuffer;
	}
}

// -----------------------------------------------------------------------------------------------
void DX11Renderer::BindSceneCB(const IConstantsBuffer* cb)
{
	const DX11ConstantsBuffer* pSceneDXCB = dynamic_cast<const DX11ConstantsBuffer*>(cb);
	ID3D11Buffer* pBuffer = pSceneDXCB->GetBuffer();
	m_pD3DDeviceContext->PSSetConstantBuffers(0, 1, &pBuffer);
	m_pD3DDeviceContext->VSSetConstantBuffers(0, 1, &pBuffer);
}

// -----------------------------------------------------------------------------------------------
S_API SSceneConstants* DX11Renderer::GetSceneConstants() const
{
	return m_SceneConstants.GetConstants();
}

// -----------------------------------------------------------------------------------------------
S_API void DX11Renderer::UpdateSceneConstants()
{
	m_SceneConstants.Update();
}

// -----------------------------------------------------------------------------------------------
S_API SResult DX11Renderer::D3D11_CreateConstantsBuffer(ID3D11Buffer** ppCB, usint32 byteSize)
{
	SP_ASSERTR(m_pD3DDevice, S_NOTINIT);
	SP_ASSERTR(ppCB, S_INVALIDPARAM);

	D3D11_BUFFER_DESC cbDesc;
	cbDesc.BindFlags = D3D11_BIND_CONSTANT_BUFFER;
	cbDesc.CPUAccessFlags = D3D11_CPU_ACCESS_WRITE;
	cbDesc.MiscFlags = 0;
	cbDesc.StructureByteStride = 0;
	cbDesc.Usage = D3D11_USAGE_DYNAMIC;
	cbDesc.ByteWidth = byteSize;


	if (Failure(m_pD3DDevice->CreateBuffer(&cbDesc, 0, ppCB)))
		return CLog::Log(S_ERROR, "Failed Create constants buffer! See stdout for more info.");


	return S_SUCCESS;
}

// -----------------------------------------------------------------------------------------------
S_API SResult DX11Renderer::D3D11_LockConstantsBuffer(ID3D11Buffer* pCB, void** pData)
{
	SP_ASSERTR(m_pD3DDeviceContext, S_NOTINIT);
	SP_ASSERTR(pCB && pData, S_INVALIDPARAM);

	D3D11_MAPPED_SUBRESOURCE cbSubRes;
	if (Failure(m_pD3DDeviceContext->Map(pCB, 0, D3D11_MAP_WRITE_DISCARD, 0, &cbSubRes)))
		return CLog::Log(S_ERROR, "Failed lock constants buffer (map failed)!");

	*pData = cbSubRes.pData;

	return S_SUCCESS;
}

// -----------------------------------------------------------------------------------------------
S_API SResult DX11Renderer::D3D11_UnlockConstantsBuffer(ID3D11Buffer* pCB)
{
	SP_ASSERTR(m_pD3DDeviceContext, S_NOTINIT);
	SP_ASSERTR(pCB, S_INVALIDPARAM);

	m_pD3DDeviceContext->Unmap(pCB, 0);

	return S_SUCCESS;
}

// -----------------------------------------------------------------------------------------------
S_API void DX11Renderer::SetViewProjMatrix(IViewport* pViewport)
{
	IViewport* pV = (pViewport) ? pViewport : GetTargetViewport();

	pV->RecalculateCameraViewMatrix();

	SetViewProjMatrix(pV->GetCameraViewMatrix(), pV->GetProjectionMatrix());
}

// -----------------------------------------------------------------------------------------------
S_API void DX11Renderer::SetViewProjMatrix(const Mat44& mtxView, const Mat44& mtxProj)
{
	SSceneConstants* pConstants = m_SceneConstants.GetConstants();
	pConstants->mtxView = mtxView;
	pConstants->mtxProj = mtxProj;
	pConstants->mtxProjInv = SMatrixInvert(SMatrixTranspose(mtxProj));
	m_SceneConstants.Update();
}

// -----------------------------------------------------------------------------------------------
S_API void DX11Renderer::SetEyePosition(const Vec3f& eyePos)
{
	m_SceneConstants.GetConstants()->eyePosition = float4(eyePos.x, eyePos.y, eyePos.z, 0);

	//FrameDump(m_ObjectConstants.mtxViewProj, "m_ObjectConstants.mtxViewProj");

	m_SceneConstants.Update();
}

// -----------------------------------------------------------------------------------------------
S_API SResult DX11Renderer::UpdateRasterizerState()
{
	SP_SAFE_RELEASE(m_pRSState);
	if (Failure(m_pD3DDevice->CreateRasterizerState(&m_rsDesc, &m_pRSState)))
		return CLog::Log(S_ERROR, "Failed recreate rasterizer state for updating rasterizer state.");

	m_pD3DDeviceContext->RSSetState(m_pRSState);
	return S_SUCCESS;
}

// -----------------------------------------------------------------------------------------------
S_API SResult DX11Renderer::UpdateDepthStencilState()
{
	SP_SAFE_RELEASE(m_pDepthStencilState);
	if (Failure(m_pD3DDevice->CreateDepthStencilState(&m_depthStencilDesc, &m_pDepthStencilState)))
		return CLog::Log(S_ERROR, "Failed recreate depth stencil state for updating rasterizer state");

	m_pD3DDeviceContext->OMSetDepthStencilState(m_pDepthStencilState, 1);
	return S_SUCCESS;
}


// -----------------------------------------------------------------------------------------------
S_API SResult DX11Renderer::UpdateCullMode(EFrontFace ff)
{
	SP_ASSERTR(IsInited(), S_NOTINIT);

	// update the rasterizer settings
	if (m_InitParams.frontFace != ff)
	{
		m_InitParams.frontFace = ff;
		m_rsDesc.FrontCounterClockwise = (ff == eFF_CCW);
		return UpdateRasterizerState();
	}

	return S_SUCCESS;
}

// -----------------------------------------------------------------------------------------------
S_API SResult DX11Renderer::EnableBackfaceCulling(bool state)
{
	assert(IsInited());

	if ((m_rsDesc.CullMode == D3D11_CULL_NONE && !state) ||
		(m_rsDesc.CullMode != D3D11_CULL_NONE && state))
	{
		return S_SUCCESS;
	}

	// update rasterizer state
	m_rsDesc.CullMode = (state) ? D3D11_CULL_BACK : D3D11_CULL_NONE;

	return UpdateRasterizerState();
}

// -----------------------------------------------------------------------------------------------
S_API SResult DX11Renderer::UpdatePolygonType(S_PRIMITIVE_TYPE type)
{
	SP_ASSERTR(IsInited(), S_NOTINIT);

	D3D11_PRIMITIVE_TOPOLOGY targetTP;
	switch (type)
	{
	case S_PRIM_RENDER_LINELIST: targetTP = D3D11_PRIMITIVE_TOPOLOGY_LINELIST; break;
	case S_PRIM_RENDER_POINTLIST: targetTP = D3D11_PRIMITIVE_TOPOLOGY_POINTLIST; break;
	case S_PRIM_RENDER_TRIANGLESTRIP: targetTP = D3D11_PRIMITIVE_TOPOLOGY_TRIANGLESTRIP; break;
	default:
		targetTP = D3D11_PRIMITIVE_TOPOLOGY_TRIANGLELIST;
	}

	D3D11_PRIMITIVE_TOPOLOGY currentTP;
	m_pD3DDeviceContext->IAGetPrimitiveTopology(&currentTP);

	if (currentTP != targetTP)
		m_pD3DDeviceContext->IASetPrimitiveTopology(targetTP);

	return S_SUCCESS;
}

// -----------------------------------------------------------------------------------------------
S_API void DX11Renderer::EnableWireframe(bool state /*=true*/)
{
	D3D11_FILL_MODE wireframeMode = state ? D3D11_FILL_WIREFRAME : D3D11_FILL_SOLID;
	if (m_rsDesc.FillMode != wireframeMode)
	{
		m_rsDesc.FillMode = wireframeMode;
		UpdateRasterizerState();
	}
}

// -----------------------------------------------------------------------------------------------
S_API void DX11Renderer::EnableDepthTest(bool enableDepthTest /*= true*/, bool enableDepthWrite /*= true*/)
{
	bool stateChanged = false;

	// Enable depth test
	if ((m_depthStencilDesc.DepthEnable ? true : false) != enableDepthTest)
	{
		m_depthStencilDesc.DepthEnable = enableDepthTest;
		stateChanged = true;
	}

	// Enable depth write
	if ((m_depthStencilDesc.DepthWriteMask == D3D11_DEPTH_WRITE_MASK_ALL ? true : false) != enableDepthWrite)
	{
		m_depthStencilDesc.DepthWriteMask = (enableDepthWrite ? D3D11_DEPTH_WRITE_MASK_ALL : D3D11_DEPTH_WRITE_MASK_ZERO);
		stateChanged = true;
	}

	if (stateChanged)
		UpdateDepthStencilState();
}

// -----------------------------------------------------------------------------------------------
S_API void DX11Renderer::SetDepthTestFunction(EDepthTestFunction depthTestFunc)
{
	D3D11_COMPARISON_FUNC d3dFunc;

	switch (depthTestFunc)
	{
	case eDEPTH_TEST_GREATER:
		d3dFunc = D3D11_COMPARISON_GREATER;
		break;
	default:
		d3dFunc = D3D11_COMPARISON_LESS;
		break;
	}

	if (d3dFunc != m_depthStencilDesc.DepthFunc)
	{
		m_depthStencilDesc.DepthFunc = d3dFunc;
		UpdateDepthStencilState();
	}
}

// -----------------------------------------------------------------------------------------------
S_API void DX11Renderer::D3D11_SetBlendState(ID3D11BlendState* pBlendState, const float blendFactor[4] /*=0*/, UINT sampleMask /*=0xffffffff*/)
{
	if (m_pSetBlendState == pBlendState)
		return;

	m_pD3DDeviceContext->OMSetBlendState(pBlendState, blendFactor, sampleMask);
	m_pSetBlendState = pBlendState;
}

SP_NMSPACE_END
